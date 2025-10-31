/*****************************************************************************\
*                                                                           *
* File(s): FFMpegUtility.hpp                                                *
*                                                                           *
* Content: Utility class to make using FFMpeg a little bit easier.          *
*                                                                           *
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

#include "../AssetIO/T2DImage.hpp"

namespace CForge {
    /**
    * \brief Utility class to make using FFMpeg a little bit easier.
    * \ingroup Utility
    * 
    */
    class FFMpegUtility {
    public:
        /**
        * \brief Allocate a new AVFrame structure.
        * 
        * \param[in] Width Width of the frame.
        * \param[in] Height Height of the frame.
        * \param[in] PixelFormat Pixel format.
        * \return New AVFrame. Use freeAVFrame to delete the structure when not needed anymore.
        * \throws CrossForgeException Exception thrown if frame could not be allocated.
        */
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

        /**
        * \brief Clears the structure.
        * 
        * \param[in] pFrame Structure to clear.
        */
        static void freeAVFrame(AVFrame* &pFrame) {
            if (nullptr != pFrame) 
            {
                av_frame_free(&pFrame);
                pFrame = nullptr;
            }
        }//freeAVFrame

        /**
        * \brief Converts an AVFrame to a new format.
        * 
        * \param[in] pSrc Source frame.
        * \param[in] DstFormat Destination format.
        * \return Converted AVFrame. Use freeAVFrame to clear once it is not needed anymore.
        */
        static AVFrame* convertPixelFormat(const AVFrame* pSrc, AVPixelFormat DstFormat) {
            AVFrame* pRval = nullptr;
            convertPixelFormat(pSrc, &pRval, DstFormat);
            return pRval;
        }//convertPixelFormat

        /**
        * \brief Convert a source frame into a target frame with a new format.
        * 
        * \param[in] pSrc Source frame.
        * \param[in, out] ppTarget Target frame. If it does no exist it will be created. You have to clean up.
        * \param[in] DstFormat Format to convert to.
        */
        static void convertPixelFormat(const AVFrame* pSrc, AVFrame** ppTarget, AVPixelFormat DstFormat) {
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

            if (nullptr != ppTarget) (*ppTarget) = pRval;
        }//convertPixelFormat

        /**
        * \brief Resizes a given frame to new dimensions.
        * 
        * \param[in] pSrc Source frame.
        * \param[in] Width Target width.
        * \param[in] Target height.
        * \return Resizes frame. You have to clean up.
        */
        static AVFrame* resizeFrame(const AVFrame* pSrc, uint32_t Width, uint32_t Height) {
            AVFrame* pRval = nullptr;
            resizeFrame(pSrc, &pRval, Width, Height);
            return pRval;
        }//convertPixelFormat

        /**
        * \brief Resizes a frame to new dimensions.
        * 
        * \param[in] pSrc Source frame.
        * \param[in,out] ppTarget Target frame which will contain the resizes frame. Will be created if it does not exist.
        * \param[in] Width Target width.
        * \param[in] Height Target height.
        * 
        */
        static void resizeFrame(const AVFrame* pSrc, AVFrame** ppTarget, uint32_t Width, uint32_t Height) {
            AVFrame* pRval = ((*ppTarget) == nullptr) ? allocAVFrame(Width, Height, AVPixelFormat(pSrc->format)) : (*ppTarget);
            if (nullptr == pRval) return;

            SwsContext* pConversionCtx = sws_getContext(pSrc->width, pSrc->height,
                (AVPixelFormat)pSrc->format,
                Width,
                Height,
                AVPixelFormat(pRval->format),
                SWS_BICUBIC,
                NULL,
                NULL,
                NULL);
            sws_scale(pConversionCtx, pSrc->data, pSrc->linesize, 0, pSrc->height, pRval->data, pRval->linesize);
            sws_freeContext(pConversionCtx);

            pRval->format = pSrc->format;
            pRval->width = Width;
            pRval->height = Height;

            if (nullptr != ppTarget) (*ppTarget) = pRval;
        }//resizeImage

        /**
        * \brief Converts a T2DImage structure to an AVFrame.
        * 
        * \param[in] pSource Source image.
        * \return New AVFrame with data from pSource. You have to clean up.
        */
        static AVFrame* toAVFrame(const T2DImage<uint8_t>* pSource) {
            AVFrame* pRval = allocAVFrame(pSource->width(), pSource->height(), AV_PIX_FMT_RGB24);
            uint32_t Size = pSource->width() * pSource->height() * pSource->componentsPerPixel();
            memcpy(pRval->data[0], pSource->data(), Size);
            return pRval;
        }//toAVFrame

        /**
        * \brief Converts a given T2DImage structure to an AVFrame.
        * 
        * \param[in, out] ppFrame AVFrame that will contain the data. Will be created if it does not exist.
        * \param[in] pSource Source image.
        */
        static void toAVFrame(AVFrame** ppFrame, const T2DImage<uint8_t>* pSource) {
            if (nullptr == *ppFrame) (*ppFrame) = allocAVFrame(pSource->width(), pSource->height(), AV_PIX_FMT_RGB24);
            uint32_t Size = pSource->width() * pSource->height() * pSource->componentsPerPixel();
            memcpy((*ppFrame)->data[0], pSource->data(), Size);
        }//toAVFrame

        /**
        * \brief Converts an AVFrame to a T2DImage structure.
        * 
        * \param[in] pSource Source AVFrame.
        * \param[out] pTarget Target 2DImage that will contain the data.
        */
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

    protected:
        /**
        * \brief Constructor
        */
        FFMpegUtility() {

        }//Constructor

        /**
        * \brief Destructor.
        */
        ~FFMpegUtility() {

        }//Destructor

    };//FFMpegUtility

}

#endif 