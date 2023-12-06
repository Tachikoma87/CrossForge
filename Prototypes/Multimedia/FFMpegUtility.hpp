/*****************************************************************************\
*                                                                           *
* File(s): FFMpegUtility.hpp                *
*                                                                           *
* Content:    *
*                        *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_FFMPEGUTILITY_HPP__
#define __CFORGE_FFMPEGUTILITY_HPP__

extern "C" {
	#include <libswscale/swscale.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

#include <crossforge/AssetIO/T2DImage.hpp>

namespace CForge {

	class FFMpegUtility {
	public:

        static AVFrame* allocAVFrame(const int32_t Width, const int32_t Height, const AVPixelFormat PixelFormat) {
            AVFrame* pRval = nullptr;

			pRval = av_frame_alloc();
			pRval->format = PixelFormat;
			pRval->width = Width;
			pRval->height = Height;
            av_frame_get_buffer(pRval, 0);

			if (nullptr == pRval) throw CForgeExcept("Unable to allocate AVFrame!");
			
            return pRval;
        }//alloc_picture

		static void freeAVFrame(AVFrame* pFrame) {
			if (nullptr != pFrame) av_frame_free(&pFrame);
		}//freeAVFrame

        static AVFrame* convertPixelFormat(AVFrame* pSrc, AVPixelFormat DstFormat) {
            const int32_t Width = pSrc->width;
            const int32_t Height = pSrc->height;

            AVFrame* pRval = allocAVFrame(Width, Height, DstFormat);
            if (nullptr == pRval) return pRval;

            SwsContext* pConversionCtx = sws_getContext(Width, Height,
                (AVPixelFormat)pSrc->format,
                Width,
                Height,
                DstFormat,
                SWS_FAST_BILINEAR,
                NULL,
                NULL,
                NULL);
            sws_scale(pConversionCtx, pSrc->data, pSrc->linesize, 0, Height, pRval->data, pRval->linesize);
            sws_freeContext(pConversionCtx);

            pRval->format = DstFormat;
            pRval->width = pSrc->width;
            pRval->height = pSrc->height;

            return pRval;
        }//convertPixelFormat

        static void convertPixelFormat(AVFrame* pSrc, AVFrame **ppTarget, AVPixelFormat DstFormat) {
            const int32_t Width = pSrc->width;
            const int32_t Height = pSrc->height;

            
            AVFrame* pRval = ((*ppTarget) == nullptr) ? allocAVFrame(Width, Height, DstFormat) : (*ppTarget);
            if (nullptr == pRval) return;

            SwsContext* pConversionCtx = sws_getContext(Width, Height,
                (AVPixelFormat)pSrc->format,
                Width,
                Height,
                DstFormat,
                SWS_FAST_BILINEAR,
                NULL,
                NULL,
                NULL);
            sws_scale(pConversionCtx, pSrc->data, pSrc->linesize, 0, Height, pRval->data, pRval->linesize);
            sws_freeContext(pConversionCtx);

            pRval->format = DstFormat;
            pRval->width = pSrc->width;
            pRval->height = pSrc->height;

        }//convertPixelFormat

        static AVFrame* toAVFrame(const T2DImage<uint8_t>* pSource) {
            AVFrame* pRval = allocAVFrame(pSource->width(), pSource->height(), AV_PIX_FMT_RGB24);
            uint32_t Size = pSource->width() * pSource->height() * pSource->componentsPerPixel();
            memcpy(pRval->data[0], pSource->data(), Size);
            return pRval;
        }//toAVFrame

        static void toAVFrame(AVFrame** ppFrame, const T2DImage<uint8_t>* pSource) {
            if(nullptr == *ppFrame) (*ppFrame) = allocAVFrame(pSource->width(), pSource->height(), AV_PIX_FMT_RGB24);
            uint32_t Size = pSource->width() * pSource->height() * pSource->componentsPerPixel();
            memcpy((*ppFrame)->data[0], pSource->data(), Size);
        }//toAVFrame

        static void to2DImage(const AVFrame* pSource, T2DImage<uint8_t>* pTarget) {
            if (nullptr == pSource) throw NullpointerExcept("pSource");
            if (nullptr == pTarget) throw NullpointerExcept("pTarget");
            pTarget->clear();

            uint32_t Size = pSource->linesize[0] * pSource->height;
            uint8_t* pData = new uint8_t[Size];

            memcpy(pData, pSource->data[0], Size);

            pTarget->init(pSource->width, pSource->height, T2DImage<uint8_t>::COLORSPACE_RGB, pData);

            if (nullptr != pData) delete[] pData;

        }//to2DImage

		FFMpegUtility() {

		}//Constructor

		~FFMpegUtility() {

		}//Destructor
	protected:

	};//FFMpegUtility

}

#endif 