/*****************************************************************************\
*                                                                           *
* File(s): FFMpegUtility.hpp                                                *
*                                                                           *
* Content: Utility class to make using FFMpeg a little bit easier.          *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_FFMPEGUTILITY_HPP__
#define __CROSSFORGE_FFMPEGUTILITY_HPP__

extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <crossforge/assetio/entities/Image2DEntity.h>


namespace crossforge {
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
        static AVFrame* allocAVFrame(const int32_t width, const int32_t height, const AVPixelFormat pixelFormat) {
            AVFrame* pRval = nullptr;

            pRval = av_frame_alloc();
            pRval->format = pixelFormat;
            pRval->width = width;
            pRval->height = height;
            av_frame_get_buffer(pRval, 0);

            if (nullptr == pRval) throw CrossForgeExcept("Unable to allocate AVFrame!");

            return pRval;
        }//alloc_picture

        /**
        * \brief Clears the structure.
        *
        * \param[in] pFrame Structure to clear.
        */
        static void freeAVFrame(AVFrame*& pFrame) {
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
        static AVFrame* convertPixelFormat(const AVFrame* pSrc, AVPixelFormat dstFormat) {
            AVFrame* pResult = nullptr;
            convertPixelFormat(pSrc, &pResult, dstFormat);
            return pResult;
        }//convertPixelFormat

        /**
        * \brief Convert a source frame into a target frame with a new format.
        *
        * \param[in] pSrc Source frame.
        * \param[in, out] ppTarget Target frame. If it does no exist it will be created. You have to clean up.
        * \param[in] DstFormat Format to convert to.
        */
        static void convertPixelFormat(const AVFrame* pSrc, AVFrame** ppTarget, AVPixelFormat dstFormat) {
            const int32_t width = pSrc->width;
            const int32_t height = pSrc->height;

            AVFrame* pRval = ((*ppTarget) == nullptr) ? allocAVFrame(width, height, dstFormat) : (*ppTarget);
            if (nullptr == pRval) return;

            SwsContext* pConversionCtx = sws_getContext(width, height,
                (AVPixelFormat)pSrc->format,
                width,
                height,
                dstFormat,
                SWS_FAST_BILINEAR,
                NULL,
                NULL,
                NULL);
            sws_scale(pConversionCtx, pSrc->data, pSrc->linesize, 0, height, pRval->data, pRval->linesize);
            sws_freeContext(pConversionCtx);

            pRval->format = dstFormat;
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
        static AVFrame* resizeFrame(const AVFrame* pSrc, uint32_t width, uint32_t height) {
            AVFrame* pResult = nullptr;
            resizeFrame(pSrc, &pResult, width, height);
            return pResult;
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
        static void resizeFrame(const AVFrame* pSrc, AVFrame** ppTarget, uint32_t width, uint32_t height) {
            AVFrame* pResult = ((*ppTarget) == nullptr) ? allocAVFrame(width, height, AVPixelFormat(pSrc->format)) : (*ppTarget);
            if (nullptr == pResult) return;

            SwsContext* pConversionCtx = sws_getContext(pSrc->width, pSrc->height,
                (AVPixelFormat)pSrc->format,
                width,
                height,
                AVPixelFormat(pResult->format),
                SWS_BICUBIC,
                NULL,
                NULL,
                NULL);
            sws_scale(pConversionCtx, pSrc->data, pSrc->linesize, 0, pSrc->height, pResult->data, pResult->linesize);
            sws_freeContext(pConversionCtx);

            pResult->format = pSrc->format;
            pResult->width = width;
            pResult->height = height;

            if (nullptr != ppTarget) (*ppTarget) = pResult;
        }//resizeImage

        /**
        * \brief Converts a T2DImage structure to an AVFrame.
        *
        * \param[in] pSource Source image.
        * \return New AVFrame with data from pSource. You have to clean up.
        */
        static AVFrame* toAVFrame(Image2DEntityPtr pImage) {
            if (nullptr == pImage) throw NullpointerExcept("pImage");
            auto pRawImageComp = pImage->getImage2DComponent();
            if (nullptr == pRawImageComp) throw MissingComponentException(Image2DComponent::identification);

            AVFrame* pResult = allocAVFrame(pRawImageComp->width(), pRawImageComp->height(), AV_PIX_FMT_RGB24);
            uint32_t size = pRawImageComp->width() * pRawImageComp->height() * pRawImageComp->getBytesPerPixel();
            memcpy(pResult->data[0], pRawImageComp->pixelData().data(), size);
            return pResult;
        }//toAVFrame

        /**
        * \brief Converts a given T2DImage structure to an AVFrame.
        *
        * \param[in, out] ppFrame AVFrame that will contain the data. Will be created if it does not exist.
        * \param[in] pSource Source image.
        */
        static void toAVFrame(AVFrame** ppFrame, Image2DEntityPtr pImage) {
            if (nullptr == pImage) throw NullpointerExcept("pImage");
            if (nullptr == ppFrame) throw NullpointerExcept("ppFrame");
            auto pRawImageComp = pImage->getImage2DComponent();
            if (nullptr == pRawImageComp) throw MissingComponentException(Image2DComponent::identification);

            if (nullptr == (*ppFrame)) *ppFrame = allocAVFrame(pRawImageComp->width(), pRawImageComp->height(), AV_PIX_FMT_RGB24);
            uint32_t size = pRawImageComp->width() * pRawImageComp->height() * pRawImageComp->getBytesPerPixel();
            memcpy((*ppFrame)->data[0], pRawImageComp->pixelData().data(), size);
        }//toAVFrame

        /**
        * \brief Converts an AVFrame to a T2DImage structure.
        *
        * \param[in] pSource Source AVFrame.
        * \param[out] pTarget Target 2DImage that will contain the data.
        */
        static void to2DImage(const AVFrame* pSource, Image2DEntityPtr pTarget) {
            if (nullptr == pSource) throw NullpointerExcept("pSource");
            if (nullptr == pTarget) throw NullpointerExcept("pTarget");
           
            auto pImageDataComp = pTarget->getImage2DComponent();
            if (nullptr == pImageDataComp) throw MissingComponentException(Image2DComponent::identification);
            pImageDataComp->clear();
            uint32_t size = pSource->linesize[0] * pSource->height;
            pImageDataComp->pixelData().resize(size);
            memcpy(pImageDataComp->pixelData().data(), pSource->data[0], size);
            pImageDataComp->width() = pSource->width;
            pImageDataComp->height() = pSource->height;
            pImageDataComp->colorSpace() = Image2DComponent::COLOR_SPACE_RGB;
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