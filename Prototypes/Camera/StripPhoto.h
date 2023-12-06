/*****************************************************************************\
*                                                                           *
* File(s): StripPhoto.h and StripPhoto.cpp                                  *
*                                                                           *
* Content:    *
*                                                   *
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

#ifndef __CFORGE_STRIPPHOTO_H__
#define __CFORGE_STRIPPHOTO_H__

#include <Eigen/Eigen>
#include <inttypes.h>
#include <vector>
#include <crossforge/AssetIO/T2DImage.hpp>

namespace CForge {

    class StripPhoto {
    public:
        StripPhoto(void);
        ~StripPhoto();

        void init(float Sensitivity, int32_t DiscardThreshold);
        void startRecording(int32_t StartTime);
        void clear(void);

        void addStrip(T2DImage<uint8_t> *pImg, uint32_t Timestamp, bool CopyImage = true);

        void calibrate(bool DeleteCurrentStrips);
        void buildStripPhoto(void);

        void retrieveStripPhoto(T2DImage<uint8_t>* pImgData, int32_t** ppTimestamps);

    private:

        struct Strip {
            T2DImage<uint8_t>* pImgData;
            uint32_t Timestamp; ////< Timestamp at CENTER of the image
            bool LightBarrierTriggered;
            bool State; ///< False if state is unknown.
            int32_t RelativeTimeInterval[2]; ///< Relative time interval the interpolated strip covers

            Strip(void) {
                Timestamp = 0;
                LightBarrierTriggered = false;
                State = false;
                pImgData = nullptr;
                RelativeTimeInterval[0] = 0;
                RelativeTimeInterval[1] = 0;
            }

            ~Strip(void) {
                clear();
            }

            void clear(void) {
                Timestamp = 0;
                LightBarrierTriggered = false;
                State = false;
                delete pImgData;
                pImgData = nullptr;
            }//clear

        };//Strip


        float computeBarrierErrorValue(int32_t StripIDLeft, int32_t StripIDRight);
        float computeBarrierErrorValue(const Strip* pLeft, const Strip* pRight);

        void generateFinalStrips(std::vector<Strip*>* pFinalStrips);
        void buildInterpolatedStrip(Strip* pPast, Strip* pPresent, Strip* pInterpolated);

        uint32_t m_StartTime; ///< Start of the heat. Required to compute relative time
        int32_t m_DiscardThreshold; ///< Discarding data threshold in milliseconds
        int32_t m_LastBarrierTrigger;
        std::vector<Strip*> m_Strips;
        float m_NoiseValue;
        float m_Sensitivity;

        T2DImage<uint8_t> m_StripPhoto;
    };//StripPhoto

}//name-space

#endif //TF_CHAMPION_STRIPPHOTO_H
