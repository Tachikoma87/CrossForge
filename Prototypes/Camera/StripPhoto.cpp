//
// Created by Tinkerer on 03.02.2021.
//

#include "StripPhoto.h"

#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace CForge {

    StripPhoto::StripPhoto(void) {
        m_StartTime = 0;
        m_Sensitivity = 1.25f;
        m_DiscardThreshold = 1000; // 0.5 seconds
        m_NoiseValue = 0;

    }//Constructor

    StripPhoto::~StripPhoto() {
        clear();
    }//Destructor

    void StripPhoto::init(float Sensitivity, int32_t DiscardThreshold) {
        clear();
        m_Sensitivity = Sensitivity;
        m_DiscardThreshold = DiscardThreshold;

    }//initialize

    void StripPhoto::startRecording(int32_t StartTime) {
        for (auto& i : m_Strips) delete i;
        m_Strips.clear();
        m_StartTime = StartTime;
        m_LastBarrierTrigger = 0;
    }//StartTime

    void StripPhoto::clear(void) {
        for (auto& i : m_Strips) delete i;
        m_Strips.clear();
        m_StartTime = 0;
        m_LastBarrierTrigger = 0;
        m_NoiseValue = 0;
        m_DiscardThreshold = 500; // default 500ms
        m_Sensitivity = 1.25; // default Sensitivity
    }//clear

    void StripPhoto::addStrip(T2DImage<uint8_t> *pImg, uint32_t Timestamp, bool CopyImage) {
        Strip* pStrip = new Strip();
        // initialize the new strip
        if (CopyImage) {
            pStrip->pImgData = new T2DImage<uint8_t>();
            pStrip->pImgData->init(pImg->width(), pImg->height(), pImg->colorSpace(), pImg->data());
        }
        else {
            pStrip->pImgData = pImg;
        }
        pStrip->Timestamp = Timestamp;
        // if it was not too long ago that the barrier was triggered, the new strip is definitely valid
        m_Strips.push_back(pStrip);
        return;

        if (Timestamp - m_LastBarrierTrigger < m_DiscardThreshold) {
            pStrip->State = true;
        }

        // we need at least two strips to do some useful computations
        if (m_Strips.size() <= 1) return;

        const float BarrierError = computeBarrierErrorValue(m_Strips[m_Strips.size() - 1], m_Strips[m_Strips.size() - 2]);
        if (BarrierError > m_Sensitivity + m_NoiseValue) {
            pStrip->LightBarrierTriggered = true;
            m_LastBarrierTrigger = Timestamp;

            // log barrier trigger, but not too often
#ifdef __ANDROID
            if (Timestamp - m_LastBarrierTrigger > 500)  __android_log_print(ANDROID_LOG_INFO, "StripPhoto", "Barrier triggered: %f %f", BarrierError, m_Sensitivity * m_NoiseThreshold);
#else 
            if(Timestamp - m_LastBarrierTrigger > 200) printf("Barrier triggered with value: %f\n", BarrierError);
#endif

            // go back in time and set valid till discard threshold
            for (uint32_t i = m_Strips.size() - 1; i > 0; --i) {
                Strip* pS = m_Strips[i];
                if (pS->State) break; // at this point in time and further back we know the state
                if (Timestamp - pS->Timestamp < m_DiscardThreshold) pS->State = true;
            }
        }
        else {
            // check if we can discard some data (only if already calibrated)
            for (uint32_t i = m_Strips.size() - 1; i > 0; --i) {
                Strip* pS = m_Strips[i];
                if (pS->State) break; // we already know state at this point
                // discard data if barrier trigger has been longer than threshold
                if ((Timestamp - pS->Timestamp) > m_DiscardThreshold) {
                    delete pS->pImgData;
                    pS->pImgData = nullptr;
                    pS->State = true;
                }
            }//for[strips back in time]
        }
    }//addStrip

    void StripPhoto::calibrate(bool DeleteCurrentStrips) {

       if (m_Strips.size() == 0) return;

        m_NoiseValue = 0.0f;
        for (uint32_t i = 1; i < m_Strips.size(); ++i) {
            float Error = computeBarrierErrorValue(i - 1, i);
            if (Error > m_NoiseValue) {
                m_NoiseValue = Error;
            }
        }

        if (DeleteCurrentStrips) {
            for (auto& i : m_Strips) delete i;
            m_Strips.clear();
        }
#ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_INFO, "StripPhoto", "Calibration finished. Noise threshold now %f from %d strips and sensitivity %f.", m_NoiseThreshold, ValidSamples, m_Sensitivity);
#else
        printf("Calibration ready. Noise value now: %f\n", m_NoiseValue);
#endif

    }//calibrate

    float StripPhoto::computeBarrierErrorValue(int32_t StripIDLeft, int32_t StripIDRight) {
        float Rval = 0.0f;
        if (StripIDLeft < 0 || StripIDRight >= m_Strips.size()) return Rval;
        if (StripIDRight < 0 || StripIDRight >= m_Strips.size()) return Rval;

        const Strip* pLeft = m_Strips[StripIDLeft];
        const Strip* pRight = m_Strips[StripIDRight];
 
        return computeBarrierErrorValue(pLeft, pRight);
    }//computeErrorValue

    float StripPhoto::computeBarrierErrorValue(const Strip* pLeft, const Strip* pRight) {
        float Rval = 0.0f;
        if (nullptr == pLeft || nullptr == pRight) return Rval;

        for (uint16_t y = 0; y < pLeft->pImgData->height(); ++y) {
            for (uint16_t x = 0; x < pLeft->pImgData->width(); ++x) {
               /* const uint8_t* pPixelLeft = pLeft->pImgData->pixelAt(x, y);
                const uint8_t* pPixelRight = pRight->pImgData->pixelAt(x, y);*/
                const uint8_t* pPixelLeft = pLeft->pImgData->pixel(x, y);
                const uint8_t* pPixelRight = pRight->pImgData->pixel(x, y);

                const int32_t RedDiff = abs((int16_t)pPixelLeft[0] - (int16_t)pPixelRight[0]);
                const int32_t GreenDiff = abs((int16_t)pPixelLeft[1] - (int16_t)pPixelRight[1]);
                const int32_t BlueDiff = abs((int16_t)pPixelLeft[2] - (int16_t)pPixelRight[2]);

            }//for[columns]
        }//for[rows]
        

        /*const int32_t Mid = pLeft->Width / 2;
        for (int32_t i = 0; i < pLeft->Height; ++i) {
            const int32_t Index = (i * pLeft->Width * 3) + (Mid * 3);
            const int32_t RedDiff = abs(pLeft->pData[Index + 0] - pRight->pData[Index + 0]);
            const int32_t GreenDiff = abs(pLeft->pData[Index + 1] - pRight->pData[Index + 1]);
            const int32_t BlueDiff = abs(pLeft->pData[Index + 2] - pRight->pData[Index + 2]);
            float PixelError = sqrtf(RedDiff * RedDiff + GreenDiff * GreenDiff + BlueDiff * BlueDiff);
            Rval += PixelError;
        }//for[Height]

        Rval /= (float)pLeft->Height;
        return Rval;
        */
        
        return Rval;
    }//computeErrorValue

    void StripPhoto::buildStripPhoto(void) {
        // collect valid strips
         // size of final image?
        uint32_t Width = 0;
        for (auto i : m_Strips) Width += i->pImgData->width();
        uint32_t Height = m_Strips[0]->pImgData->height();

        uint8_t* pData = new uint8_t[Width * Height * 3]; // final image data
        memset(pData, 0, Width * Height * 3 * sizeof(uint8_t));
        uint32_t CurrentX = 0; // x cursor (width)

        // now we copy the image data
        for (uint32_t i = 0; i < m_Strips.size(); ++i) {
            Strip* pS = m_Strips[i];
            for (uint32_t y = 0; y < pS->pImgData->height(); ++y) {
                const int32_t RowIndex = y * Width * 3; // start of row (final image)
                for (uint32_t x = 0; x < pS->pImgData->width(); ++x) {
                    const int32_t Index = RowIndex + (CurrentX + x) * 3; // start of pixel (final image)
                    const int32_t StripIndex = (y * pS->pImgData->width() + x) * 3; // current pixel (strip)
                    pData[Index + 0] = pS->pImgData->data()[StripIndex + 0];
                    pData[Index + 1] = pS->pImgData->data()[StripIndex + 1];
                    pData[Index + 2] = pS->pImgData->data()[StripIndex + 2];
                }//for[rows]
            }//for[all columns]

            const float Step = pS->pImgData->width() / (float)(pS->RelativeTimeInterval[1] - pS->RelativeTimeInterval[0]);

            //for (uint32_t x = 0; x < pS->pImgData->width(); ++x) {
            //    pTimestamps[CurrentX + x] = pS->RelativeTimeInterval[0] + (int32_t)round(Step * x);
            //}//for[all columns]

            CurrentX += pS->pImgData->width();
        }//for[all strips]

        m_StripPhoto.init(Width, Height, T2DImage<uint8_t>::COLORSPACE_RGB, pData);
        
        
//        std::vector<Strip*> FinalStrips;
//        m_StripPhoto.clear();
//        generateFinalStrips(&FinalStrips);
//        if (FinalStrips.size() == 0) return;
//
//        // size of final image?
//        uint32_t Width = 0;
//        for (auto i : FinalStrips) Width += i->Width;
//        uint32_t Height = FinalStrips[0]->Height;
//#ifdef __ANDROID__
//        __android_log_print(ANDROID_LOG_INFO, "StripPhoto", "Building strip photo with resolution %d x %d.", Width, Height);
//#endif
//
//        uint8_t* pData = new uint8_t[Width * Height * 3]; // final image data
//        int32_t* pTimestamps = new int32_t[Width]; // timestamps for each pixel, "precise" to the millisecond :-P
//        memset(pData, 0, Width * Height * 3 * sizeof(uint8_t));
//        memset(pTimestamps, 0, Width * sizeof(int32_t));
//        uint32_t CurrentX = 0; // x cursor (width)
//
//        // now we copy the image data
//        for (uint32_t i = 0; i < FinalStrips.size(); ++i) {
//            Strip* pS = FinalStrips[i];
//            for (uint32_t y = 0; y < pS->Height; ++y) {
//                const int32_t RowIndex = y * Width * 3; // start of row (final image)
//                for (uint32_t x = 0; x < pS->Width; ++x) {
//                    const int32_t Index = RowIndex + (CurrentX + x) * 3; // start of pixel (final image)
//                    const int32_t StripIndex = (y * pS->Width + x) * 3; // current pixel (strip)
//                    pData[Index + 0] = pS->pData[StripIndex + 0];
//                    pData[Index + 1] = pS->pData[StripIndex + 1];
//                    pData[Index + 2] = pS->pData[StripIndex + 2];
//                }//for[rows]
//            }//for[all columns]
//
//            const float Step = pS->Width / (float)(pS->RelativeTimeInterval[1] - pS->RelativeTimeInterval[0]);
//
//            for (uint32_t x = 0; x < pS->Width; ++x) {
//                pTimestamps[CurrentX + x] = pS->RelativeTimeInterval[0] + (int32_t)round(Step * x);
//            }//for[all columns]
//
//            CurrentX += pS->Width;
//        }//for[all strips]
//
//        // now create the timings table
//#ifdef __ANDROID__
//        __android_log_print(ANDROID_LOG_INFO, "StripPhoto", "Strip photo generation finished.");
//#endif
//
//        m_StripPhoto.Height = Height;
//        m_StripPhoto.Width = Width;
//        m_StripPhoto.pColorData = pData;
//        m_StripPhoto.pTimestamps = pTimestamps;
//
//        // delete generated final strips
//        for (auto& i : FinalStrips) delete i;
//        FinalStrips.clear();
        
    }//buildStripPhoto

    void StripPhoto::buildInterpolatedStrip(Strip* pPast, Strip* pPresent, Strip* pInterpolated) {
  /*      int32_t TimeInterval = pPresent->Timestamp - pPast->Timestamp;

        // required pixels for each strip are 500px * Interval/1000.0 if we cover each second with 500px
        int32_t Width = (int32_t)(round(1.0 * TimeInterval)); // that is the number of columns we have to generate
        //__android_log_print(ANDROID_LOG_INFO, "StripPhoto", "Generating Strip with width %d @ %d ms", Width, TimeInterval);
        int32_t Height = pPast->Height;

        uint8_t* pColumnPast = new uint8_t[Height * 3];
        uint8_t* pColumnPresent = new uint8_t[Height * 3];
        uint8_t* pColumnInterpolated = new uint8_t[Height * 3];

        pInterpolated->init(nullptr, Width, Height, 0);
        pInterpolated->RelativeTimeInterval[0] = pPast->Timestamp - m_StartTime;
        pInterpolated->RelativeTimeInterval[1] = pPresent->Timestamp - m_StartTime;

        const int32_t PastMid = pPast->Width / 2 + 1;
        const int32_t PresentMid = pPresent->Width / 2 + 1;

        const int32_t SampleWidth = 32;
        const int32_t HalfSampleWidth = SampleWidth / 2;

        // always init first and last column
        pPast->getColumn(pColumnPast, PastMid);
        pPresent->getColumn(pColumnPresent, PresentMid);
        pInterpolated->setColumn(pColumnPast, 0);
        pInterpolated->setColumn(pColumnPresent, Width - 1);

        float Factor = SampleWidth / (float)Width; // steps factor
        for (int32_t k = 1; k < Width; ++k) {
            // we have 44 lines in total (22 from past and 22 from present)
            float RelativeIndexPast = (k - 1) * Factor;
            float RelativeIndexPresent = k * Factor;

            int32_t IndexPast = (int32_t)round(RelativeIndexPast);
            int32_t IndexPresent = (int32_t)round(RelativeIndexPresent);

            // retrieve the correct columns
            if (IndexPast < HalfSampleWidth) pPast->getColumn(pColumnPast, PastMid + IndexPast);
            else pPresent->getColumn(pColumnPast, PresentMid - HalfSampleWidth + (IndexPast - HalfSampleWidth));

            if (IndexPresent < HalfSampleWidth) pPast->getColumn(pColumnPresent, PastMid + IndexPresent);
            else pPresent->getColumn(pColumnPresent, PresentMid - HalfSampleWidth + (IndexPresent - HalfSampleWidth));

            // create interpolated column
            // 0.5 is not correct, compute correct Alpha from relative indices
            const float Alpha = 0.5f;
            for (int32_t t = 0; t < Height; ++t) {
                pColumnInterpolated[t * 3 + 0] = (uint8_t)round(Alpha * pColumnPast[t * 3 + 0] + (1.0 - Alpha) * pColumnPresent[t * 3 + 0]);
                pColumnInterpolated[t * 3 + 1] = (uint8_t)round(Alpha * pColumnPast[t * 3 + 1] + (1.0 - Alpha) * pColumnPresent[t * 3 + 1]);
                pColumnInterpolated[t * 3 + 2] = (uint8_t)round(Alpha * pColumnPast[t * 3 + 2] + (1.0 - Alpha) * pColumnPresent[t * 3 + 2]);
            }

            // and store
            pInterpolated->setColumn(pColumnInterpolated, k);
        }//for[width]

        delete[] pColumnPast;
        delete[] pColumnPresent;
        delete[] pColumnInterpolated;
        */
    }

    void StripPhoto::generateFinalStrips(std::vector<Strip*>* pFinalStrips) {
        /*
#ifdef __ANDROID__
        if (nullptr == pFinalStrips) __android_log_print(ANDROID_LOG_ERROR, "StripPhoto::collectFinalStrips", "Nulpointer exception (pFinalStrips)");
#endif
        if (m_Strips.size() == 0) return;

        // generate Strips from past strip timestamp to present strip timestamp
        for (uint32_t i = 1; i < m_Strips.size(); ++i) {
            Strip* pPast = m_Strips[i - 1];
            Strip* pPresent = m_Strips[i];
            // we skip if one of the two was discarded
            if (nullptr == pPast->pData || nullptr == pPresent->pData) continue;

            Strip* pInterpolated = new Strip();
            buildInterpolatedStrip(pPast, pPresent, pInterpolated);
            pFinalStrips->push_back(pInterpolated);

        }//for[all strips]
        */
    }//generateFinalStrips

    void StripPhoto::retrieveStripPhoto(T2DImage<uint8_t>* pImgData, int32_t** ppTimestamps) {
        if (nullptr == pImgData) throw NullpointerExcept("pImgData");
        buildStripPhoto();
        pImgData->init(m_StripPhoto.width(), m_StripPhoto.height(), m_StripPhoto.colorSpace(), m_StripPhoto.data());
    }//retrieveStripPhoto


}//names-space