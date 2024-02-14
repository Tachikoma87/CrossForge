//extern "C" {
//#include <libswscale/swscale.h>
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//}

#include <crossforge/Utility/FFMpegUtility.hpp>


#include <crossforge/Utility/CForgeUtility.h>
#include <crossforge/AssetIO/SAssetIO.h>
#include "FFMPEG.h"


namespace CForge {
    //AVFrame* allocPicture(AVPixelFormat pix_fmt, int width, int height) {
    //    AVFrame * picture;
    // 
    //    picture = av_frame_alloc();
    //    picture->format = pix_fmt;
    //    picture->width = width;
    //    picture->height = height;
    //    av_frame_get_buffer(picture, 0);
    //
    //    if (!picture) {
    //        av_free(picture);
    //        return nullptr;    
    //    }
    //    return picture;
    //}//alloc_picture

    //void convertPixelFormat(AVFrame* src, AVFrame **ppDst, AVPixelFormat DstFormat) {
    //    int width = src->width;
    //    int height = src->height;

    //    AVFrame* dst = allocPicture(DstFormat, width, height);

    //    SwsContext* conversion = sws_getContext(width,
    //        height,
    //        (AVPixelFormat)src->format,
    //        width,
    //        height,
    //        DstFormat,
    //        SWS_FAST_BILINEAR,
    //        NULL,
    //        NULL,
    //        NULL);
    //    sws_scale(conversion, src->data, src->linesize, 0, height, dst->data, dst->linesize);
    //    sws_freeContext(conversion);

    //    dst->format = DstFormat;
    //    dst->width = src->width;
    //    dst->height = src->height;

    //    (*ppDst) = dst;
    //}//convertPixelFormat

    //void toAVFrame(T2DImage<uint8_t> *pSource, AVFrame** ppTarget) {

    //    AVFrame* pRes = allocPicture(AV_PIX_FMT_RGB24, pSource->width(), pSource->height());
    //    for (uint32_t i = 0; i < pSource->width() * pSource->height(); ++i) {
    //        pRes->data[0][i * 3 + 0] = pSource->data()[i * 3 + 0];
    //        pRes->data[0][i * 3 + 1] = pSource->data()[i * 3 + 1];
    //        pRes->data[0][i * 3 + 2] = pSource->data()[i * 3 + 2];
    //    }
    //    (*ppTarget) = pRes;
    //}//toAVFrame

    //void to2DImage(AVFrame* pSource, T2DImage<uint8_t> *pTarget) {
    //    pTarget->clear();
    //    
    //    uint32_t BufferSize = pSource->linesize[0] * pSource->height;
    //    uint8_t* pData = new uint8_t[BufferSize];

    //    for (uint32_t y = 0; y < pSource->height; ++y) {
    //        for (uint32_t x = 0; x < pSource->width; ++x) {
    //            uint32_t Index = y * pSource->width + x;
    //            pData[Index * 3 + 0] = pSource->data[0][Index * 3 + 0];
    //            pData[Index * 3 + 1] = pSource->data[0][Index * 3 + 1];
    //            pData[Index * 3 + 2] = pSource->data[0][Index * 3 + 2];
    //        }
    //    }
    //    pTarget->init(pSource->width, pSource->height, T2DImage<uint8_t>::COLORSPACE_RGB, pData);

    //    if (nullptr != pData) delete[] pData;

    //}//to2DImage

	FFMPEG::FFMPEG() {

	}//ffmpeg

	FFMPEG::~FFMPEG() {

	}//ffmpeg

    void FFMPEG::firstTest() {

        T2DImage<uint8_t> Img;
        AssetIO::load("MyAssets/Textures/ground13.jpg", &Img);

        AVFrame* pAVImg = FFMpegUtility::toAVFrame(&Img);
        AVFrame* pTarget = FFMpegUtility::convertPixelFormat(pAVImg, AV_PIX_FMT_BGR24);

        
        Img.clear();
        FFMpegUtility::to2DImage(pTarget, &Img);
        AssetIO::store("MyAssets/TestImg.jpg", &Img);

        FFMpegUtility::freeAVFrame(pAVImg);
        FFMpegUtility::freeAVFrame(pTarget);

    }//firstTest

    void FFMPEG::convertNV12(uint8_t* pImgData, uint32_t BufferSize, uint32_t Width, uint32_t Height, T2DImage<uint8_t>* pDest) {
        AVFrame* pImg = FFMpegUtility::allocAVFrame(Width, Height, AV_PIX_FMT_NV12);
        //AV_PIX_FMT_NV12

        // y plane

       // memcpy(&pImg->data[0][0], &pImgData[0], BufferSize);

        uint32_t YSize = Width * Height;
        memcpy(&pImg->data[0][0], &pImgData[0], YSize);

        uint32_t UVSize = Width * Height / 2;
        memcpy(&pImg->data[1][0], &pImgData[YSize], UVSize);

        AVFrame* pTarget = FFMpegUtility::convertPixelFormat(pImg, AV_PIX_FMT_RGB24);
        FFMpegUtility::to2DImage(pTarget, pDest);

        av_free(pImg);
        av_free(pTarget);
    }//convertNV12

	

}