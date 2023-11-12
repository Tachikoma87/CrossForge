
extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libavutil/channel_layout.h>
}

#include "../Multimedia/FFMpegUtility.hpp"

#include "VideoRecorder.h"

#define STREAM_DURATION 10.0
#define STREAM_FRAME_RATE 25
#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P
#define SCALE_FLAGS SWS_BICUBIC

namespace CForge {

	typedef struct OutputStream {
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
	};//OutputStream

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

	// prepare a dummy image
	void fillYUVImage(AVFrame* pPict, int32_t FrameIndex, int32_t Width, int32_t Height) {
		int32_t i = FrameIndex;

		// y plane
		for (int32_t y = 0; y < Height; y++) {
			for (int32_t x = 0; x < Width; ++x) {
				pPict->data[0][y * pPict->linesize[0] + x] = x + y + i * 3;
			}
		}

		// cb and cr
		for (int32_t y = 0; y < Height / 2; ++y) {
			for (int32_t x = 0; x < Width / 2; ++x) {
				pPict->data[1][y * pPict->linesize[1] + x] = 128 + y + i * 2;
				pPict->data[2][y * pPict->linesize[2] + x] = 64 + x + i * 5;
			}
		}
	}//fillYUVImage

	AVFrame* getVideoFrame(OutputStream* pStream) {
		AVCodecContext* pCodecCtx = pStream->pEnc;

		// check if we want to generate more frames
		AVRational tbb;
		tbb.num = tbb.den = 1;
		if (av_compare_ts(pStream->NextPts, pCodecCtx->time_base, STREAM_DURATION, tbb) > 0) return nullptr;

		// when we pass a frame to the encoder, it may keep a reference to it intenally
		// make sure we do not overwrite it here
		if (av_frame_make_writable(pStream->pFrame) < 0) throw CForgeExcept("Unable to make frame writable!");

		if (pCodecCtx->pix_fmt != AV_PIX_FMT_YUV420P) {
			// as we only generate a YUV420P picture, we must convert it o the codec pixel format if needed
			if (nullptr == pStream->pSwrCtx) {
				pStream->pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, SCALE_FLAGS, nullptr, nullptr, nullptr);
				if (nullptr == pStream->pSwsCtx) throw CForgeExcept("Could not initialize the conversion context!");
			}

			fillYUVImage(pStream->pTmpFrame, pStream->NextPts, pCodecCtx->width, pCodecCtx->height);
			sws_scale(pStream->pSwsCtx, (const uint8_t* const*)pStream->pTmpFrame->data, pStream->pTmpFrame->linesize, 0, pCodecCtx->height, pStream->pFrame->data, pStream->pFrame->linesize);
		}
		else {
			fillYUVImage(pStream->pFrame, pStream->NextPts, pCodecCtx->width, pCodecCtx->height);
		}
		pStream->pFrame->pts = pStream->NextPts++;
		return pStream->pFrame;
	}//getVideoFrame

	int32_t writeVideoFrame(AVFormatContext* pFormatCtx, OutputStream* pStream) {
		return writeFrame(pFormatCtx, pStream->pEnc, pStream->pStream, getVideoFrame(pStream), pStream->pTmpPkt);
	}//writeVideoFrame

	void addStream(OutputStream* pStream, AVFormatContext* pFormatCtx, const AVCodec** ppCodec, enum AVCodecID CodecID) {
		AVCodecContext *pCodecCtx = nullptr;
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
			pCodecCtx->width = 352;
			pCodecCtx->height = 288;
			// timebase: This is the fundamental unit of time (in seconds) in terms of which frame timestamps are represented
			// For fixed-fps content, timebase should be 1/framerate and timestamp increments should be identical to 1
			pStream->pStream->time_base.num = 1;
			pStream->pStream->time_base.den = STREAM_FRAME_RATE;
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

	void muxTest() {

		OutputStream VideoStream = { 0 };
		OutputStream AudioStream = { 0 };

		const AVOutputFormat* pFmt = nullptr;
		AVFormatContext* pFormatCtx = nullptr;
		const AVCodec* pAudioCodec = nullptr;
		const AVCodec* pVideoCodec = nullptr;

		int32_t HasAudio = 0;
		int32_t HasVideo = 0;
		int32_t EncodeVideo = 0;
		int32_t EncodeAudio = 0;

		AVDictionary* pAVDict = nullptr;

		int32_t Rval = 0;

		std::string Filename = "MyAssets/MuxTestVideo.mp4";

		char ErrorBuffer[AV_ERROR_MAX_STRING_SIZE];

		// allocate the output media context
		avformat_alloc_output_context2(&pFormatCtx, nullptr, nullptr, Filename.c_str());
		
		if (nullptr == pFormatCtx) {
			printf("Could not deduce output format from file extension: using MPEG.\n");
			avformat_alloc_output_context2(&pFormatCtx, nullptr, "mpeg", Filename.c_str());
		}
		if (nullptr == pFormatCtx) throw CForgeExcept("Unable to allocate format context!");
		
		pFmt = pFormatCtx->oformat;

		// Add the audio and video streams using the default format codecs and initialize the codecs
		if (pFmt->video_codec != AV_CODEC_ID_NONE) {
			addStream(&VideoStream, pFormatCtx, &pVideoCodec, pFmt->video_codec);
			HasVideo = 1;
			EncodeVideo = 1;
		}
		if (pFmt->audio_codec != AV_CODEC_ID_NONE) {
			//addStream(&AudioStream, pFormatCtx, &pAudioCodec, pFmt->audio_codec);
			HasAudio = 0;
			EncodeAudio = 0;
		}

		// Now that all the parameters are set, we can open the audio and video codecs and allocate the necessary encode buffers
		if (HasVideo) openVideo(pFormatCtx, pVideoCodec, &VideoStream, pAVDict);
		//if (HasAudio) openAudio(pFormatCtx, pAudioCodec, &AudioStream, pAVDict);

		av_dump_format(pFormatCtx, 0, Filename.c_str(), 1);

		// open the output file, if needed
		if (!(pFmt->flags & AVFMT_NOFILE)) {
			Rval = avio_open(&pFormatCtx->pb, Filename.c_str(), AVIO_FLAG_WRITE);
			if (Rval < 0) {	
				av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), Rval);
				throw CForgeExcept("Could not open file " + Filename + ": " + ErrorBuffer);
			}
		}

		// write the stream header, if any
		Rval = avformat_write_header(pFormatCtx, &pAVDict);
		if (Rval < 0) {
			av_make_error_string(ErrorBuffer, sizeof(ErrorBuffer), Rval);
			throw CForgeExcept("Error occurred when writing format header: " + std::string(ErrorBuffer));
		}

		while (EncodeVideo || EncodeAudio) {
			// select the stream to encode
			if (EncodeVideo && (!EncodeAudio || av_compare_ts(VideoStream.NextPts, VideoStream.pEnc->time_base, AudioStream.NextPts, AudioStream.pEnc->time_base) <= 0)) {
				EncodeVideo = !writeVideoFrame(pFormatCtx, &VideoStream);
			}
			else {
				//EncodeAudio = !write_audio_frame(pFormatCtx, &AudioStream);
			}
		}
		
		av_write_trailer(pFormatCtx);

		// close each codec
		if (HasVideo) closeStream(pFormatCtx, &VideoStream);
		if (HasAudio) closeStream(pFormatCtx, &AudioStream);

		// close the output file
		if (!(pFmt->flags & AVFMT_NOFILE)) {
			avio_close(pFormatCtx->pb);
		}

		// free the stream
		avformat_free_context(pFormatCtx);
	}//muxTest

	void encode(AVCodecContext* pContext, AVFrame *pFrame, AVPacket *pPkt, File *pOutFile) {

		av_frame_make_writable(pFrame);

		int RVal = avcodec_send_frame(pContext, pFrame);
		if (RVal < 0) throw CForgeExcept("Error sending a frame for encoding!");

		while (RVal >= 0) {
			RVal = avcodec_receive_packet(pContext, pPkt);
			if(RVal == AVERROR(EAGAIN) || RVal == AVERROR_EOF) break;
			else if (RVal < 0) {
				char Buffer[AV_ERROR_MAX_STRING_SIZE];
				av_make_error_string(Buffer, sizeof(Buffer), RVal);
				throw CForgeExcept("Error during encoding: " + std::string(Buffer));
			}

			pOutFile->write(pPkt->data, pPkt->size);
		}

	}//encode


	VideoRecorder::VideoRecorder(void): CForgeObject("VideoRecorder") {
		m_FrameWidth = 0;
		m_FrameHeight = 0;
		m_FPS = 0.0f;

		m_pCodecCtx = nullptr;
		m_pPkt = nullptr;
	}//Constructor

	VideoRecorder::~VideoRecorder(void) {
		clear();
	}//Destructor

	void VideoRecorder::init() {

	}//initialize

	void VideoRecorder::startRecording(const std::string Filename, uint32_t Width, uint32_t Height, const float FPS) {

		muxTest();

		//clear();
		//m_Filename = Filename;
		//m_FPS = FPS;

		//AVCodecID Codec = AV_CODEC_ID_MPEG2VIDEO;
		//AVCodecContext* pCodecCtx = nullptr;
		//AVPacket* pPkt = nullptr;

		//const AVCodec* pCodec = avcodec_find_encoder(Codec);
		//if (nullptr == pCodec) throw CForgeExcept("Unable to find video codec!");
		//pCodecCtx = avcodec_alloc_context3(pCodec);
		//if (nullptr == pCodecCtx) throw CForgeExcept("Unable to allocate video context!");

		//pPkt = av_packet_alloc();
		//if (nullptr == pPkt) throw CForgeExcept("Unable to allocate packet!");

		//pCodecCtx->bit_rate = 26500*1000;
		//// resolution
		//pCodecCtx->width = Width;
		//pCodecCtx->height = Height;
		//// frames per second
		//pCodecCtx->time_base.num = 1;
		//pCodecCtx->time_base.den = FPS;

		//pCodecCtx->framerate.num = FPS;
		//pCodecCtx->framerate.den = 1;

		//pCodecCtx->gop_size = 10;
		//pCodecCtx->max_b_frames = 1;
		//pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;


		//// open it
		//int32_t RVal = avcodec_open2(pCodecCtx, pCodec, nullptr);
		//char Buffer[AV_ERROR_MAX_STRING_SIZE];
		//
		//if (RVal < 0) {
		//	av_make_error_string(Buffer, sizeof(Buffer), RVal);
		//	throw CForgeExcept(std::string("Could not open codec: ") + Buffer);
		//}

		//m_pCodecCtx = pCodecCtx;
		//m_pPkt = pPkt;
		//m_FrameCounter = 0;

		//// open File
		//m_VideoFile.begin(Filename, "wb");

	}//startRecording

	void VideoRecorder::stopRecording() {
		//AVCodecContext* pCodecCtx = (AVCodecContext*)(m_pCodecCtx);
		//AVPacket* pPkt = (AVPacket*)(m_pPkt);
		//// write endcode
		//uint8_t Endcode[] = { 0, 0, 1, 0xb7 };
		//// flush the encoder

		//m_VideoFile.write(Endcode, sizeof(Endcode));
		//m_VideoFile.end();
		//
		//avcodec_free_context(&pCodecCtx);
		//av_packet_free(&pPkt);

		
	}//stopRecording

	void VideoRecorder::clear(void) {
		
		m_FrameWidth = 0;
		m_FrameHeight = 0;
		m_FPS = 0.0f;

	}//clear

	void VideoRecorder::release(void) {
		delete this;
	}//release

	void VideoRecorder::addFrame(const T2DImage<uint8_t>* pImg, uint64_t Timestamp) {
		/*if (nullptr == pImg) throw NullpointerExcept("pImg");
		if (pImg->width() == 0 || pImg->height() == 0) throw CForgeExcept("Empty image specified!");

		AVCodecContext* pCodecCtx = (AVCodecContext*)(m_pCodecCtx);
		AVPacket* pPkt = (AVPacket*)(m_pPkt);

		AVFrame *pFrame = FFMpegUtility::toAVFrame(pImg);
		AVFrame *pEncoderFrame = FFMpegUtility::convertPixelFormat(pFrame, AV_PIX_FMT_YUV420P);
		pEncoderFrame->pts = m_FrameCounter++;

		encode(pCodecCtx, pEncoderFrame, pPkt, &m_VideoFile);
		
		FFMpegUtility::freeAVFrame(pFrame);
		FFMpegUtility::freeAVFrame(pEncoderFrame);*/
	}//addFrame

	void VideoRecorder::finish(void) {
		clear();
	}//finish

	float VideoRecorder::fps(void)const {
		return m_FPS;
	}//fps

	uint32_t VideoRecorder::width(void)const {
		return m_FrameWidth;
	}//width

	uint32_t VideoRecorder::height(void)const {
		return m_FrameHeight;
	}//height

	bool VideoRecorder::isRecording(void)const {
		return (m_FrameWidth != 0);
	}//isRecording

}//name space
