/*****************************************************************************\
*                                                                           *
* File(s): IMUPackage.hpp                                            *
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
#ifndef __IMUPACKAGE_HPP__
#define __IMUPACKAGE_HPP__

#include <inttypes.h>
#include <cstring>

namespace IMUWIP {
    struct IMUPackage {

        enum Command : int8_t {
            CMD_UNKNOWN = -1,
            CMD_RAW_DATA,
            CMD_AVG_DATA,
            CMD_CALIBRATE,
            CMD_SEARCH,
        };

        enum DeviceType : int8_t {
            DEVICE_UNKNOWN = -1,
            DEVICE_DONGLE = 0,
            DEVICE_TRACKER_LEFT,
            DEVICE_TRACKER_RIGHT,
        };

        IMUPackage(void) {
            GyroX = 0.0f;
            GyroY = 0.0f;
            GyroZ = 0.0f;
            AccelX = 0.0f;
            AccelY = 0.0f;
            AccelZ = 0.0f;

            Cmd = CMD_UNKNOWN;
            pIP = nullptr;
            Port = 0;
            Type = DEVICE_UNKNOWN;
        }//Constructor

        ~IMUPackage(void) {
            if (nullptr != pIP) delete[] pIP;
            pIP = nullptr;
        }//Destructor

        static uint16_t setMagicTag(uint8_t* pBuffer) {
            pBuffer[0] = 'T';
            pBuffer[1] = '-';
            pBuffer[2] = 'I';
            pBuffer[3] = 'M';
            pBuffer[4] = 'U';
            return 5; // wrote 5 bytes
        }//setMagicTag

        static bool checkMagicTag(uint8_t* pBuffer) {
            return  (pBuffer[0] == 'T' &&
                pBuffer[1] == '-' &&
                pBuffer[2] == 'I' &&
                pBuffer[3] == 'M' &&
                pBuffer[4] == 'U');
        }//checkMagicTag

        uint16_t toStream(uint8_t* pBuffer, uint16_t BufferSize) {
            uint16_t Rval = 0;

            // set magic command
            Rval += setMagicTag(&pBuffer[Rval]);
            // set command
            memcpy(&pBuffer[Rval], &Cmd, sizeof(int8_t)); Rval += sizeof(int8_t);

            // set float values
            if (CMD_RAW_DATA == Cmd || CMD_AVG_DATA == Cmd) {
                memcpy(&pBuffer[Rval], &GyroX, sizeof(float)); Rval += sizeof(float);
                memcpy(&pBuffer[Rval], &GyroY, sizeof(float)); Rval += sizeof(float);
                memcpy(&pBuffer[Rval], &GyroZ, sizeof(float)); Rval += sizeof(float);
                memcpy(&pBuffer[Rval], &AccelX, sizeof(float)); Rval += sizeof(float);
                memcpy(&pBuffer[Rval], &AccelY, sizeof(float)); Rval += sizeof(float);
                memcpy(&pBuffer[Rval], &AccelZ, sizeof(float)); Rval += sizeof(float);
                memcpy(&pBuffer[Rval], &Type, sizeof(DeviceType)); Rval += sizeof(DeviceType);
            }
            else if (CMD_SEARCH == Cmd) {
                // set ip
                uint8_t StringLength = 0;
                while (nullptr != pIP && pIP[StringLength++] != '\0');
                memcpy(&pBuffer[Rval], &StringLength, sizeof(uint8_t)); Rval += sizeof(uint8_t);
                memcpy(&pBuffer[Rval], pIP, sizeof(char) * StringLength); Rval += sizeof(char) * StringLength;
                // copy port
                memcpy(&pBuffer[Rval], &Port, sizeof(uint16_t)); Rval += sizeof(uint16_t);
                // Device Type
                memcpy(&pBuffer[Rval], &Type, sizeof(DeviceType)); Rval += sizeof(DeviceType);

            }

            return Rval;
        }//toStream

        void fromStream(uint8_t* pBuffer, uint16_t BufferSize) {
            if (!checkMagicTag(pBuffer)) return;
            uint16_t Pointer = 5;

            memcpy(&Cmd, &pBuffer[Pointer], sizeof(int8_t)); Pointer += sizeof(int8_t);
            if (CMD_RAW_DATA == Cmd || CMD_AVG_DATA == Cmd) {
                memcpy(&GyroX, &pBuffer[Pointer], sizeof(float)); Pointer += sizeof(float);
                memcpy(&GyroY, &pBuffer[Pointer], sizeof(float)); Pointer += sizeof(float);
                memcpy(&GyroZ, &pBuffer[Pointer], sizeof(float)); Pointer += sizeof(float);
                memcpy(&AccelX, &pBuffer[Pointer], sizeof(float)); Pointer += sizeof(float);
                memcpy(&AccelY, &pBuffer[Pointer], sizeof(float)); Pointer += sizeof(float);
                memcpy(&AccelZ, &pBuffer[Pointer], sizeof(float)); Pointer += sizeof(float);
                memcpy(&Type, &pBuffer[Pointer], sizeof(DeviceType)); Pointer += sizeof(DeviceType);
            }
            else if (CMD_SEARCH == Cmd) {
                // ip string
                uint8_t StringLength = 0;
                memcpy(&StringLength, &pBuffer[Pointer], sizeof(uint8_t)); Pointer += sizeof(uint8_t);
                if (0 != StringLength) {
                    if (pIP != nullptr) {
                        delete[] pIP;
                        pIP = nullptr;
                    }
                    pIP = new char[StringLength];
                    memcpy(pIP, &pBuffer[Pointer], sizeof(char) * StringLength); Pointer += sizeof(char) * StringLength;
                }
                //port
                memcpy(&Port, &pBuffer[Pointer], sizeof(uint16_t)); Pointer += sizeof(uint16_t);
                // device type
                memcpy(&Type, &pBuffer[Pointer], sizeof(DeviceType)); Pointer += sizeof(DeviceType);

            }

        }//fromStream

        uint16_t streamSizeMax(void) {
            uint16_t Rval = 64;

            return Rval;
        }//streamSize

        void setIP(const char* pAddress) {
            if (nullptr == pAddress) return;

            if (nullptr != pIP) {
                delete[] pIP;
                pIP = nullptr;
            }

            uint8_t StringLength = 0;
            while (pAddress[StringLength++] != '\0');
            pIP = new char[StringLength];
            memcpy(pIP, pAddress, sizeof(char) * StringLength);

        }//setIP

        Command Cmd;
        float GyroX;
        float GyroY;
        float GyroZ;
        float AccelX;
        float AccelY;
        float AccelZ;

        uint16_t Port;
        char* pIP;
        DeviceType Type;
    };//IMUPackage

}//ArduForge

#endif 