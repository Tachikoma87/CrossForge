
extern "C" {
	#include <libavutil/channel_layout.h>
	#include <libavutil/opt.h>
	#include <libswresample/swresample.h>
	#include <libavutil/channel_layout.h>
}

#include "../Utility/FFMpegUtility.hpp"
#include "VideoRecorder.h"

#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P

namespace CForge {

	struct OutputStream {
		AVStream* pStream;
		AVCodecContext* pEnc;

		int64_t NextPts;	// pts of the next frame that will be generated
		int32_t SamplesCount;

		AVFrame* pFrame;
		AVFrame* pTmpFrame;

		AVPacket* pTmpPkt;

		float t, tincr, tincr2;

		struct SwsContext* pSwsCtx;
		struct SwrContext* pSwrCtx;

		OutputStream() {
			pStream = nullptr;
			pEnc = nullptr;
			NextPts = 0;
			SamplesCount = 0;

			pFrame = nullptr;
			pTmpFrame = nullptr;
			pTmpPkt = nullptr;
			t = tincr = tincr2 = 0;
			pSwsCtx = nullptr;
			pSwrCtx = nullptr;
		}
	};//OutputStream

	struct VideoData {
		OutputStream VideoStream;
		OutputStream AudioStream;

		const AVOutputFormat* pFmt;
		AVFormatContext* pFmtCtx;
		const AVCodec* pAudioCodec;
		const AVCodec* pVideoCodec;
		AVDictionary* pAVDict;

		int32_t HasAudio;
		int32_t HasVideo;

		std::string Filename;
		FILE* pOutputFile;

		int32_t FrameWidth;
		int32_t FrameHeight;
		int32_t FPS;

		VideoData() {
			HasAudio = 0;
			HasVideo = 0;

			pFmt = nullptr;
			pFmtCtx = nullptr;
			pAudioCodec = nullptr;
			pVideoCodec = nullptr;
			pAVDict = nullptr;
		}
	};

	int32_t writeFrame(AVFormatContext* pFmtCtx, AVCodecContext* pCodecCtx, AVStream* pStream, AVFrame* pFrame, AVPacket* pPkt) {
		int32_t Rval = 0;
		char ErrorBuffer[AV_ERROR_MAX_STRING_SIZE];
		// send frame to encoder
		Rval = avcodec_send_frame(pCodecCtx, pFrame);
		if (Rval < 0) {
			av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), Rval);
			throw CForgeExcept("Error sending frame to the encoder: " + std::string(ErrorBuffer));
		}

		while (Rval >= 0) {
			Rval = avcodec_receive_packet(pCodecCtx, pPkt);
			if (Rval == AVERROR(EAGAIN) || Rval == AVERROR_EOF) break;
			else if (Rval < 0) {
				av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), Rval);
				throw CForgeExcept("Error encoding a frame: " + std::string(ErrorBuffer));
			}
			// rescale output packet timestamp value from codec to stream timebase
			av_packet_rescale_ts(pPkt, pCodecCtx->time_base, pStream->time_base);
			pPkt->stream_index = pStream->index;

			// write the compressed frame to the media file
			//log_packet(pFormatCtx, pPkt);
			Rval = av_interleaved_write_frame(pFmtCtx, pPkt);
			// pPkt is now blank (av_interleaved_write_frame() takes ownership of its contents and resets pkt), so taht no unreferencing is necessary
			// this would be different if one used av_write_frame()
			if (Rval < 0) {
				av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), Rval);
				throw CForgeExcept("Error while writing output packet: " + std::string(ErrorBuffer));
			}
		}//while

		return (Rval == AVERROR_EOF) ? 1 : 0;
	}//writeFrame

	AVFrame* getVideoFrame(OutputStream* pStream, const T2DImage<uint8_t>* pImg) {
		AVCodecContext* pCodecCtx = pStream->pEnc;

		// check if we want to generate more frames
		AVRational tbb;
		tbb.num = tbb.den = 1;

		// when we pass a frame to the encoder, it may keep a reference to it internally
		// make sure we do not overwrite it here
		if (av_frame_make_writable(pStream->pFrame) < 0) throw CForgeExcept("Unable to make frame writable!");

		AVFrame* pSourceImg = FFMpegUtility::toAVFrame(pImg);

		if (pSourceImg->width != pStream->pFrame->width || pSourceImg->height != pStream->pFrame->height) {
			AVFrame* pDstFrame = nullptr;
			FFMpegUtility::resizeFrame(pSourceImg, &pDstFrame, pStream->pFrame->width, pStream->pFrame->height);
			FFMpegUtility::freeAVFrame(pSourceImg);
			pSourceImg = pDstFrame;
		}

		FFMpegUtility::convertPixelFormat(pSourceImg, &pStream->pFrame, AV_PIX_FMT_YUV420P);
		FFMpegUtility::freeAVFrame(pSourceImg);

		pStream->pFrame->pts = pStream->NextPts++;
		return pStream->pFrame;
	}//getVideoFrame


	void addStream(OutputStream* pStream, AVFormatContext* pFormatCtx, const AVCodec** ppCodec, enum AVCodecID CodecID, VideoData* pData) {
		AVCodecContext* pCodecCtx = nullptr;
		char ErrorBuffer[AV_ERROR_MAX_STRING_SIZE];

		// find the encoder
		*ppCodec = avcodec_find_encoder(CodecID);
		if (nullptr == *ppCodec) {
			throw CForgeExcept("Could not find encoder for " + std::string(avcodec_get_name(CodecID)));
		}

		pStream->pTmpPkt = av_packet_alloc();
		if (nullptr == pStream->pTmpPkt) throw CForgeExcept("Could not allocate AVPacket!");

		pStream->pStream = avformat_new_stream(pFormatCtx, nullptr);
		if (nullptr == pStream->pStream) throw CForgeExcept("Could not allocate stream!");
		pStream->pStream->id = pFormatCtx->nb_streams - 1;
		pCodecCtx = avcodec_alloc_context3(*ppCodec);
		if (nullptr == pCodecCtx) throw CForgeExcept("Could not alloc an encoding context!");
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
			AVChannelLayout Layout;
			Layout;
			Layout.nb_channels = 2;
			Layout.order = AV_CHANNEL_ORDER_NATIVE;
			Layout.u.mask = ((1ul << AV_CHAN_FRONT_LEFT) | (1ul << AV_CHAN_FRONT_RIGHT));

			av_channel_layout_copy(&pCodecCtx->ch_layout, &Layout);
			pStream->pStream->time_base.num = 1;
			pStream->pStream->time_base.den = pCodecCtx->sample_rate;

		}break;
		case AVMEDIA_TYPE_VIDEO: {
			pCodecCtx->codec_id = CodecID;
			pCodecCtx->width = pData->FrameWidth;
			pCodecCtx->height = pData->FrameHeight;
			// timebase: This is the fundamental unit of time (in seconds) in terms of which frame timestamps are represented
			// For fixed-fps content, timebase should be 1/framerate and timestamp increments should be identical to 1
			pStream->pStream->time_base.num = 1;
			pStream->pStream->time_base.den = pData->FPS;
			pCodecCtx->time_base = pStream->pStream->time_base;
			pCodecCtx->gop_size = 12; // emit one intra frame every twelve frames at most
			pCodecCtx->pix_fmt = STREAM_PIX_FMT;
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
		int32_t Rval = 0;
		AVCodecContext* pCodecCtx = pStream->pEnc;
		AVDictionary* pDict = nullptr;
		av_dict_copy(&pDict, pOptDict, 0);

		char ErrorBuffer[AV_ERROR_MAX_STRING_SIZE];

		// open the codec
		Rval = avcodec_open2(pCodecCtx, pCodec, &pDict);
		av_dict_free(&pDict);
		if (Rval < 0) {
			av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), Rval);
			throw CForgeExcept("Could not open the video codec: " + std::string(ErrorBuffer));
		}

		// allocate and init a re-usable frame
		pStream->pFrame = FFMpegUtility::allocAVFrame(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt);
		if (nullptr == pStream->pFrame)	throw CForgeExcept("Could not allocate video frame!");

		// if the output format is not YUV420P, then a temporary YUV420P picture is needed too. It is the nconverted to the required output format
		pStream->pTmpFrame = nullptr;
		if (pCodecCtx->pix_fmt != AV_PIX_FMT_YUV420P) {
			pStream->pTmpFrame = FFMpegUtility::allocAVFrame(pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P);
			if (nullptr == pStream->pTmpFrame) throw CForgeExcept("Could not allocate temporary video frame!");
		}

		// copy the stream parameters to the muxer
		Rval = avcodec_parameters_from_context(pStream->pStream->codecpar, pCodecCtx);
		if (Rval < 0) {
			av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), Rval);
			throw CForgeExcept("Could not copy the stream parameters: " + std::string(ErrorBuffer));
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

	VideoRecorder::VideoRecorder(void) : CForgeObject("VideoRecorder") {
		m_pData = nullptr;
	}//Constructor

	VideoRecorder::~VideoRecorder(void) {
		clear();
	}//Destructor

	void VideoRecorder::init() {

	}//initialize

	void VideoRecorder::clear(void) {
		if (nullptr != m_pData) delete m_pData;
		m_pData = nullptr;
	}//clear

	void VideoRecorder::startRecording(const std::string Filename, uint32_t Width, uint32_t Height, const float FPS) {
		int32_t EncodeVideo;
		int32_t EncodeAudio;

		int32_t Rval = 0;

		if (nullptr != m_pData) delete m_pData;
		m_pData = new VideoData();

		m_pData->FrameWidth = Width;
		m_pData->FrameHeight = Height;
		m_pData->FPS = FPS;
		m_pData->Filename = Filename;

		char ErrorBuffer[AV_ERROR_MAX_STRING_SIZE];

		// allocate the output media context
		avformat_alloc_output_context2(&m_pData->pFmtCtx, nullptr, nullptr, m_pData->Filename.c_str());

		if (nullptr == m_pData->pFmtCtx) {
			printf("Could not deduce output format from file extension: using MPEG.\n");
			avformat_alloc_output_context2(&m_pData->pFmtCtx, nullptr, "mp4", m_pData->Filename.c_str());
		}
		if (nullptr == m_pData->pFmtCtx) throw CForgeExcept("Unable to allocate format context!");

		m_pData->pFmt = m_pData->pFmtCtx->oformat;

		// ...

		// Add the audio and video streams using the default format codecs and initialize the codecs
		if (m_pData->pFmt->video_codec != AV_CODEC_ID_NONE) {
			addStream(&m_pData->VideoStream, m_pData->pFmtCtx, &m_pData->pVideoCodec, m_pData->pFmt->video_codec, m_pData);
			m_pData->HasVideo = 1;
			EncodeVideo = 1;
		}
		// audio not yet implemented
		if (m_pData->pFmt->audio_codec != AV_CODEC_ID_NONE) {
			//addStream(&AudioStream, pFormatCtx, &pAudioCodec, pFmt->audio_codec);
			m_pData->HasAudio = 0;
			EncodeAudio = 0;
		}

		// Now that all the parameters are set, we can open the audio and video codecs and allocate the necessary encode buffers
		if (m_pData->HasVideo) openVideo(m_pData->pFmtCtx, m_pData->pVideoCodec, &m_pData->VideoStream, m_pData->pAVDict);
		//if (HasAudio) openAudio(pFormatCtx, pAudioCodec, &AudioStream, pAVDict);

		av_dump_format(m_pData->pFmtCtx, 0, m_pData->Filename.c_str(), 1);

		// open the output file, if needed
		if (!(m_pData->pFmt->flags & AVFMT_NOFILE)) {
			Rval = avio_open(&m_pData->pFmtCtx->pb, m_pData->Filename.c_str(), AVIO_FLAG_WRITE);
			if (Rval < 0) {
				av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), Rval);
				throw CForgeExcept("Could not open file " + m_pData->Filename + ": " + ErrorBuffer);
			}
		}

		// write the stream header, if any
		Rval = avformat_write_header(m_pData->pFmtCtx, &m_pData->pAVDict);
		if (Rval < 0) {
			av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), Rval);
			throw CForgeExcept("Error occurred when writing format header: " + std::string(ErrorBuffer));
		}
	}//startRecording

	void VideoRecorder::stopRecording() {
		if (nullptr == m_pData) throw CForgeExcept("Video recording not running!");
		av_write_trailer(m_pData->pFmtCtx);

		// close each codec
		if (m_pData->HasVideo) closeStream(m_pData->pFmtCtx, &m_pData->VideoStream);
		if (m_pData->HasAudio) closeStream(m_pData->pFmtCtx, &m_pData->AudioStream);

		// close the output file
		if (!(m_pData->pFmt->flags & AVFMT_NOFILE)) {
			avio_close(m_pData->pFmtCtx->pb);
		}

		// free the stream
		avformat_free_context(m_pData->pFmtCtx);

		delete m_pData;
		m_pData = nullptr;
	}//stopRecording


	void VideoRecorder::addFrame(const T2DImage<uint8_t>* pImg) {
		if (nullptr == m_pData) throw CForgeExcept("Video recording not running!");

		writeFrame(m_pData->pFmtCtx, m_pData->VideoStream.pEnc, m_pData->VideoStream.pStream, getVideoFrame(&m_pData->VideoStream, pImg), m_pData->VideoStream.pTmpPkt);

	}//addFrame

	float VideoRecorder::fps(void)const {
		return m_pData->FPS;
	}//fps

	uint32_t VideoRecorder::width(void)const {
		return m_pData->FrameWidth;
	}//width

	uint32_t VideoRecorder::height(void)const {
		return m_pData->FrameHeight;
	}//height

	bool VideoRecorder::isRecording(void)const {
		return (m_pData->FrameWidth != 0);
	}//isRecording


}//name space
