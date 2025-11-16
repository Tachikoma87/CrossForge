extern "C" {
	#include <libavutil/channel_layout.h>
	#include <libavutil/opt.h>
	#include <libswresample/swresample.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libswscale/swscale.h>
	#include <libavformat/avformat.h>
	#include <libavutil/imgutils.h>
}

#include "../../utility/FFMpegUtility.hpp"

#include "VideoController.h"

#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P

namespace crossforge {

	struct OutputStream {
		AVStream* pStream;
		AVCodecContext* pEnc;

		int64_t nextPts;	// pts of the next frame that will be generated
		int32_t samplesCount;

		AVFrame* pFrame;
		AVFrame* pTmpFrame;

		AVPacket* pTmpPkt;

		float t, tincr, tincr2;

		struct SwsContext* pSwsCtx;
		struct SwrContext* pSwrCtx;

		OutputStream() {
			pStream = nullptr;
			pEnc = nullptr;
			nextPts = 0;
			samplesCount = 0;

			pFrame = nullptr;
			pTmpFrame = nullptr;
			pTmpPkt = nullptr;
			t = tincr = tincr2 = 0;
			pSwsCtx = nullptr;
			pSwrCtx = nullptr;
		}

		~OutputStream() {
			LogDebug("Output stream destructor was called.");
		}

	};//OutputStream
	typedef std::shared_ptr<OutputStream> OutputStreamPtr;

	struct VideoData {
		OutputStream videoStream;
		OutputStream audioStream;

		const AVOutputFormat* pFmt;
		AVFormatContext* pFmtCtx;
		const AVCodec* pAudioCodec;
		const AVCodec* pVideoCodec;
		AVDictionary* pAVDict;

		int32_t hasAudio;
		int32_t hasVideo;

		std::string filename;
		FILE* pOutputFile;

		int32_t frameWidth;
		int32_t frameHeight;
		int32_t fps;

		VideoData() {
			hasAudio = 0;
			hasVideo = 0;

			pFmt = nullptr;
			pFmtCtx = nullptr;
			pAudioCodec = nullptr;
			pVideoCodec = nullptr;
			pAVDict = nullptr;

			fps = 0.0;
			frameHeight = 0;
			frameWidth = 0;
			pOutputFile = nullptr;
		}

		~VideoData() {
			LogDebug("Video data destructor was called.");
		}
	};
	typedef std::shared_ptr<VideoData> VideoDataPtr;

	
	bool writeFrame(AVFormatContext* pFmtCtx, AVCodecContext* pCodecCtx, AVStream* pStream, AVFrame* pFrame, AVPacket* pPkt) {
		int32_t result = 0;
		char errorBuffer[AV_ERROR_MAX_STRING_SIZE];

		// send frame to encoder
		result = avcodec_send_frame(pCodecCtx, pFrame);
		if (result < 0) {
			av_make_error_string(errorBuffer, sizeof(errorBuffer), result);
			throw CrossForgeExcept("Error sending frame to the encoder: " + std::string(errorBuffer));
		}

		while (result >= 0) {
			result = avcodec_receive_packet(pCodecCtx, pPkt);
			if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) break;
			else if (result < 0) {
				av_make_error_string(errorBuffer, sizeof(errorBuffer), result);
				throw CrossForgeExcept("Error encoding a frame: " + std::string(errorBuffer));
			}
			// rescale output packet timestamp value from codec to stream timebase
			av_packet_rescale_ts(pPkt, pCodecCtx->time_base, pStream->time_base);
			pPkt->stream_index = pStream->index;

			// write the compressed frame to the media file
			//log_packet(pFormatCtx, pPkt);
			result = av_interleaved_write_frame(pFmtCtx, pPkt);
			// pPkt is now blank (av_interleaved_write_frame() takes ownership of its contents and resets pkt), so taht no unreferencing is necessary
			// this would be different if one used av_write_frame()
			if (result < 0) {
				av_make_error_string(errorBuffer, sizeof(errorBuffer), result);
				throw CrossForgeExcept("Error while writing output packet: " + std::string(errorBuffer));
			}
		}//while

		return (result != AVERROR_EOF);
	}//writeFrame

	AVFrame* getVideoFrame(OutputStream* pStream, Image2DEntityPtr pImg) {
		AVCodecContext* pCodecCtx = pStream->pEnc;

		// check if we want to generate more frames
		AVRational tbb;
		tbb.num = tbb.den = 1;

		// when we pass a frame to the encoder, it may keep a reference to it internally
		// make sure we do not overwrite it here
		if (av_frame_make_writable(pStream->pFrame) < 0) throw CrossForgeExcept("Unable to make frame writable!");

		AVFrame* pSourceImg = FFMpegUtility::toAVFrame(pImg);

		if (pSourceImg->width != pStream->pFrame->width || pSourceImg->height != pStream->pFrame->height) {
			AVFrame* pDstFrame = nullptr;
			FFMpegUtility::resizeFrame(pSourceImg, &pDstFrame, pStream->pFrame->width, pStream->pFrame->height);
			FFMpegUtility::freeAVFrame(pSourceImg);
			pSourceImg = pDstFrame;
		}

		FFMpegUtility::convertPixelFormat(pSourceImg, &pStream->pFrame, AV_PIX_FMT_YUV420P);
		FFMpegUtility::freeAVFrame(pSourceImg);

		pStream->pFrame->pts = pStream->nextPts++;
		return pStream->pFrame;
	}//getVideoFrame


	void addStream(OutputStream* pStream, AVFormatContext* pFormatCtx, const AVCodec** ppCodec, enum AVCodecID CodecID, VideoDataPtr pData) {
		AVCodecContext* pCodecCtx = nullptr;
		char errorBuffer[AV_ERROR_MAX_STRING_SIZE];

		// find the encoder
		*ppCodec = avcodec_find_encoder(CodecID);
		if (nullptr == *ppCodec) {
			throw CrossForgeExcept("Could not find encoder for " + std::string(avcodec_get_name(CodecID)));
		}

		pStream->pTmpPkt = av_packet_alloc();
		if (nullptr == pStream->pTmpPkt) throw CrossForgeExcept("Could not allocate AVPacket!");

		pStream->pStream = avformat_new_stream(pFormatCtx, nullptr);
		if (nullptr == pStream->pStream) throw CrossForgeExcept("Could not allocate stream!");
		pStream->pStream->id = pFormatCtx->nb_streams - 1;
		pCodecCtx = avcodec_alloc_context3(*ppCodec);
		if (nullptr == pCodecCtx) throw CrossForgeExcept("Could not alloc an encoding context!");
		pStream->pEnc = pCodecCtx;

		switch ((*ppCodec)->type) {
		case AVMEDIA_TYPE_AUDIO: {
			pCodecCtx->sample_fmt = (*ppCodec)->sample_fmts ? (*ppCodec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
			pCodecCtx->bit_rate = 64000;
			pCodecCtx->sample_rate = 44100;
			if ((*ppCodec)->supported_samplerates) {
				pCodecCtx->sample_rate = (*ppCodec)->supported_samplerates[0];
				for (int32_t i = 0; i < (*ppCodec)->supported_samplerates[i]; ++i) {
					if ((*ppCodec)->supported_samplerates[i] == 44100) pCodecCtx->sample_rate = 44100;
				}
			}
			AVChannelLayout layout;
			layout;
			layout.nb_channels = 2;
			layout.order = AV_CHANNEL_ORDER_NATIVE;
			layout.u.mask = ((1ul << AV_CHAN_FRONT_LEFT) | (1ul << AV_CHAN_FRONT_RIGHT));

			av_channel_layout_copy(&pCodecCtx->ch_layout, &layout);
			pStream->pStream->time_base.num = 1;
			pStream->pStream->time_base.den = pCodecCtx->sample_rate;

		}break;
		case AVMEDIA_TYPE_VIDEO: {
			pCodecCtx->codec_id = CodecID;
			pCodecCtx->width = pData->frameWidth;
			pCodecCtx->height = pData->frameHeight;
			//pCodecCtx->skip_frame = AVDISCARD_NONKEY;
			// timebase: This is the fundamental unit of time (in seconds) in terms of which frame timestamps are represented
			// For fixed-fps content, timebase should be 1/framerate and timestamp increments should be identical to 1
			pStream->pStream->time_base.num = 1;
			pStream->pStream->time_base.den = pData->fps;

			pCodecCtx->time_base = pStream->pStream->time_base;
			pCodecCtx->gop_size = 12; // emit one intra frame every twelve frames at most
			pCodecCtx->pix_fmt = STREAM_PIX_FMT;

			avcodec_open2(pCodecCtx, *ppCodec, nullptr);

			if (pCodecCtx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
				// just for testing we also add b-frames
				pCodecCtx->max_b_frames = 2;
			}
			if (pCodecCtx->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
				pCodecCtx->mb_decision = 2;
			}

			
		}break;
		default: break;
		}

		if (pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	}//addStream

	void openVideo(AVFormatContext* pFormatCtx, const AVCodec* pCodec, OutputStream* pStream, AVDictionary* pOptDict) {
		int32_t result = 0;
		AVCodecContext* pCodecCtx = pStream->pEnc;
		AVDictionary* pDict = nullptr;
		av_dict_copy(&pDict, pOptDict, 0);

		char errorBuffer[AV_ERROR_MAX_STRING_SIZE];

		// open the codec
		result = avcodec_open2(pCodecCtx, pCodec, &pDict);
		av_dict_free(&pDict);
		if (result < 0) {
			av_make_error_string(errorBuffer, sizeof(errorBuffer), result);
			throw CrossForgeExcept("Could not open the video codec: " + std::string(errorBuffer));
		}

		// allocate and init a re-usable frame
		pStream->pFrame = FFMpegUtility::allocAVFrame(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt);
		if (nullptr == pStream->pFrame)	throw CrossForgeExcept("Could not allocate video frame!");

		// if the output format is not YUV420P, then a temporary YUV420P picture is needed too. It is the nconverted to the required output format
		pStream->pTmpFrame = nullptr;
		if (pCodecCtx->pix_fmt != AV_PIX_FMT_YUV420P) {
			pStream->pTmpFrame = FFMpegUtility::allocAVFrame(pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P);
			if (nullptr == pStream->pTmpFrame) throw CrossForgeExcept("Could not allocate temporary video frame!");
		}

		// copy the stream parameters to the muxer
		result = avcodec_parameters_from_context(pStream->pStream->codecpar, pCodecCtx);
		if (result < 0) {
			av_make_error_string(errorBuffer, sizeof(errorBuffer), result);
			throw CrossForgeExcept("Could not copy the stream parameters: " + std::string(errorBuffer));
		}
	}//openVideo

	void closeStream(AVFormatContext* pFormatCtx, OutputStream* pStream) {
		avcodec_free_context(&pStream->pEnc);
		av_frame_free(&pStream->pFrame);
		av_frame_free(&pStream->pTmpFrame);
		av_packet_free(&pStream->pTmpPkt);
		sws_freeContext(pStream->pSwsCtx);
		swr_free(&pStream->pSwrCtx);
	}//closeStream



	VideoController::VideoController(const std::string identification) : ControllerBase(VideoController::identification) {
		m_inheritance.push_back(identification);
	}

	VideoController::~VideoController() {

	}

	bool VideoController::startRecording(VideoEntityPtr pVideo){
		if (nullptr == pVideo) throw NullpointerExcept("pVideo");
		VideoDataComponentPtr pVideoData = pVideo->getVideoDataComponent();
		if (nullptr == pVideoData) throw MissingComponentException(VideoDataComponent::identification);

		bool result = false;
		if (0 == pVideoData->width() || 0 == pVideoData->height()) LogError("Specified video width and/or height is 0, thus not valid.");
		else if (pVideoData->filename().empty()) LogError("Specified filename is empty, thus not valid.");
		else {
			if (pVideoData->framerate() < 0.1f) pVideoData->framerate() = 30.0f;
			
			int32_t encodeVideo;
			int32_t encodeAudio;
			int32_t avResult = 0;
			char ErrorBuffer[AV_ERROR_MAX_STRING_SIZE];

			auto pAvVideoData = std::make_shared<VideoData>();
			pVideoData->videoData() = std::static_pointer_cast<void>(pAvVideoData);
			pAvVideoData->frameHeight = pVideoData->height();
			pAvVideoData->frameWidth = pVideoData->width();
			pAvVideoData->fps = pVideoData->framerate();
			pAvVideoData->filename = pVideoData->filename();

			// allocate the output media context	
			if (nullptr == pAvVideoData->pFmtCtx) {
				avformat_alloc_output_context2(&(pAvVideoData->pFmtCtx), nullptr, nullptr, pAvVideoData->filename.c_str());
			}
			if (nullptr == pAvVideoData->pFmtCtx) {
				LogWarning("Could not deduce output format from file extension using FFMPEG.");
				avformat_alloc_output_context2(&(pAvVideoData->pFmtCtx), nullptr, "mp4", pAvVideoData->filename.c_str());
			}

			if (nullptr == pAvVideoData->pFmtCtx) throw CrossForgeExcept("Unable to allocate format context!");
			pAvVideoData->pFmt = pAvVideoData->pFmtCtx->oformat;

			if (pAvVideoData->pFmt->video_codec != AV_CODEC_ID_NONE) {
				addStream(&(pAvVideoData->videoStream), pAvVideoData->pFmtCtx, &pAvVideoData->pVideoCodec, pAvVideoData->pFmt->video_codec, pAvVideoData);
				pAvVideoData->hasVideo = 1;
				encodeVideo = 1;
			}
			// audio not implemented
			if (pAvVideoData->pFmt->audio_codec != AV_CODEC_ID_NONE) {
				//addStream(&AudioStream, pFormatCtx, &pAudioCodec, pFmt->audio_codec);
				pAvVideoData->hasAudio = 0;
				encodeAudio = 0;
			}
			
			// Now that all the parameters are set, we can open the audio and video codecs and allocate the necessary encode buffers
			if (pAvVideoData->hasVideo) openVideo(pAvVideoData->pFmtCtx, pAvVideoData->pVideoCodec, &pAvVideoData->videoStream, pAvVideoData->pAVDict);
			//if (HasAudio) openAudio(pFormatCtx, pAudioCodec, &AudioStream, pAVDict);

			av_dump_format(pAvVideoData->pFmtCtx, 0, pAvVideoData->filename.c_str(), 1);

			// open the output file, if required
			if (!(pAvVideoData->pFmt->flags & AVFMT_NOFILE)) {
				avResult = avio_open(&pAvVideoData->pFmtCtx->pb, pAvVideoData->filename.c_str(), AVIO_FLAG_WRITE);
				if (avResult < 0) {
					av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), avResult);
					throw CrossForgeExcept("Could not open file " + pAvVideoData->filename + ": " + ErrorBuffer);
				}
			}

			// write the stream header, if any
			avResult = avformat_write_header(pAvVideoData->pFmtCtx, &pAvVideoData->pAVDict);
			if (avResult < 0) {
				av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), avResult);
				throw CrossForgeExcept("Error occurred while writing format header: " + std::string(ErrorBuffer));
			}

			pVideoData->isRecording() = true;
			result = true;
		}
		return result;

	}//startRecording

	bool VideoController::stopRecording(VideoEntityPtr pVideo) {
		if (nullptr == pVideo) throw NullpointerExcept("pVideo");
		auto pVideoDataComp = pVideo->getVideoDataComponent();
		if (nullptr == pVideoDataComp) throw MissingComponentException(VideoDataComponent::identification);
		bool result = false;

		if (pVideoDataComp->isRecording()) {
			auto pAvVideoData = std::static_pointer_cast<VideoData>(pVideoDataComp->videoData());
			av_write_trailer(pAvVideoData->pFmtCtx);

			// close each codec
			if (pAvVideoData->hasVideo) closeStream(pAvVideoData->pFmtCtx, &pAvVideoData->videoStream);
			if (pAvVideoData->hasAudio) closeStream(pAvVideoData->pFmtCtx, &pAvVideoData->audioStream);

			// close the output file
			if (!(pAvVideoData->pFmt->flags & AVFMT_NOFILE)) avio_close(pAvVideoData->pFmtCtx->pb);

			// free the stream
			avformat_free_context(pAvVideoData->pFmtCtx);
			pAvVideoData->pFmtCtx = nullptr;
			pAvVideoData->pFmt = nullptr;
			pVideoDataComp->isRecording() = false;
			result = true;
		}
		else {
			LogError("VideoEntity is currently not recording.");
		}
		return result;
		
	}//stopRecording


	bool VideoController::addFrame(VideoEntityPtr pVideo, Image2DEntityPtr pFrame) {
		if (nullptr == pVideo) throw NullpointerExcept("pVideo");
		if (nullptr == pFrame) throw NullpointerExcept("pFrame");
		bool result = false;

		auto pVideoDataComp = pVideo->getVideoDataComponent();
		if (nullptr == pVideoDataComp) throw MissingComponentException(VideoDataComponent::identification);
		if (pVideoDataComp->isRecording()) {
			auto pAvVideoData = std::static_pointer_cast<VideoData>(pVideoDataComp->videoData());
			if (nullptr != pAvVideoData) {
				try {
					result = writeFrame(pAvVideoData->pFmtCtx, pAvVideoData->videoStream.pEnc, pAvVideoData->videoStream.pStream, getVideoFrame(&pAvVideoData->videoStream, pFrame), pAvVideoData->videoStream.pTmpPkt);	
				}
				catch (CrossForgeException& e) {
					Logger::logException(e);
				}
				catch (...) {
					LogError("Not handled exception while writing video frame.");
				}
			}
			else {
				LogError("Av video data of video data component is null. Video stream is not valid.");
			}
		}
		else {
			LogError("Video entity is currently not recording!");
		}
		
		return result;
	}//addFrame

}