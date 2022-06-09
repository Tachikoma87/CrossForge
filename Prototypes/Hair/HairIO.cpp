#include "HairIO.h"

#include "../../CForge/Core/SLogger.h"

#include <iostream>

namespace CForge {
	HairIO::HairIO(void): I3DHairIO("HairIO") {
	}//constructor

	HairIO::~HairIO(void) {
		clear();
	}//destructor

	void HairIO::init(void) {
	}//initialize

	void HairIO::clear(void) {
	}//clear

	void HairIO::release(void) {
		delete this;
	}

    bool HairIO::accepted(const std::string Filepath, Operation Op) {
		bool Rval = false;
		if (Op == OP_LOAD) {
            if (Filepath.find(".hair") != std::string::npos) Rval = true;
            else if (Filepath.find(".tfx") != std::string::npos) Rval = true;
        }
        else {
            Rval = false;
        }
		return Rval;
	}//accepted

	void HairIO::load(const std::string Filepath, T3DHair<float>* pHair){
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified");
        if (nullptr == pHair) throw NullpointerExcept("pHair");

        int FilepathLength = Filepath.length();
        if (Filepath.substr(FilepathLength - 5) == ".hair") { // cyHair file
            loadCyHairFile(Filepath.c_str(), pHair);
        }
        else if (Filepath.substr(FilepathLength - 4) == ".tfx") { // TressFX file
            loadTFXFile(Filepath.c_str(), pHair);
        }
        else {
            throw CForgeExcept("File format not supported.");
        }

        // output stats
        std::cout << "Hairfile '" + Filepath + "' loaded: " << std::endl;
        std::cout << "  strands = " << pHair->strandCount() << std::endl;
        std::cout << "  points  = " << pHair->vertexCount() << std::endl;
	}//load

	void HairIO::store(const std::string Filepath, const T3DHair<float>* pHair) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified!");
		if (nullptr == pHair) throw NullpointerExcept("pHair");

		throw CForgeExcept("Currently no storing of hair files.");
	}//store

    // load file in cem yuksel hair format (.hair)
    void HairIO::loadCyHairFile(const std::string Filepath, T3DHair<float>* pHair)
    {
        FILE *fp = fopen(Filepath.c_str(), "rb");
        if (fp == nullptr) {
            throw CForgeExcept("Can't open hair file.");
        }

        // hair file header as specified on http://www.cemyuksel.com/research/hairmodels/
        struct Header {
            char signature[4]; // this should be "HAIR"
            unsigned int numStrands; // number of hair strands
            unsigned int numPoints; // total number of points of all strands
            unsigned int arrays; // bit array of data in the file

            unsigned int numSegmentsDefault; // default number of segments of each strand
            float thicknessDefault; // default thickness of hair strands
            float transparencyDefault; // default transparency of hair strands
            float colorDefault[3]; // default color of hair strands

            char info[88]; // information about the file
        };//Header

        // read the file header
        Header header;
        size_t headread = fread(&header, sizeof(Header), 1, fp);

        // check if it's a correct hair file with header signature
        if (strncmp(header.signature, "HAIR", 4) != 0) {
            throw CForgeExcept("Hair file has wrong signature.");
        }

        // get values from file header
        unsigned int numPoints = header.numPoints;
        unsigned int numFloats = numPoints * 3;
        unsigned int numStrands = header.numStrands;
        pHair->strandCount(numStrands);
        //pHair->thickness = header.thicknessDefault; [TODO]
        pHair->color(header.colorDefault[0], header.colorDefault[1], header.colorDefault[2], header.transparencyDefault);

        // read segments array
        bool segmentsArrayGiven;
        unsigned short* segments;
        unsigned int numStrandPointsDefault;
        if (header.arrays & 1) { // segments array given in hair file
            segmentsArrayGiven = true;
            segments = new unsigned short[numStrands];
            size_t numSegmentsRead = fread(segments, sizeof(unsigned short), numStrands, fp);
            if (numSegmentsRead < numStrands) {
                throw CForgeExcept("Can't read hair segments.");
            }
        }
        else { // no segments array, use default
            segmentsArrayGiven = false;
            numStrandPointsDefault = header.numSegmentsDefault + 1;
        }

        // read points array
        float* pointArray = new float[numFloats];
        size_t numPointsRead = fread(pointArray, sizeof(float), numFloats, fp);
        if (numPointsRead < numFloats) {
            throw CForgeExcept("Can't read hair points.");
        }

        // create tangents array
        int* start = new int[numStrands]; // draw call index array
        int* count = new int[numStrands]; // draw call index array
        unsigned int* pointStrand = new unsigned int[numPoints];
        float* tangentArray = new float[numFloats];
        int numStrandPoints = numStrandPointsDefault;
        int currentArrayIndex = 0; // this tracks the current index while iterating through the point and tangent arrays
        for (unsigned int strandIndex = 0; strandIndex < numStrands; strandIndex++) {
            if (segmentsArrayGiven) {
                numStrandPoints = segments[strandIndex] + 1;
            }

            // index arrays used for glMultiDrawArrays()
            start[strandIndex] = currentArrayIndex / 3;
            count[strandIndex] = numStrandPoints;

            // TODO [use T3DHair strand array]

            std::vector<Eigen::Matrix<float, 3, 1>> strandPoints;
            for (unsigned int pointIndex = 0; pointIndex < numStrandPoints; pointIndex++) {
                Eigen::Matrix<float, 3, 1> sp(
                    pointArray[currentArrayIndex],
                    pointArray[currentArrayIndex + 1],
                    pointArray[currentArrayIndex + 2]
                );

                pointStrand[currentArrayIndex / 3] = strandIndex;
                strandPoints.push_back(sp);
                currentArrayIndex += 3;
            }
            computeStrandTangents(tangentArray, strandPoints, currentArrayIndex, numStrandPoints); // calculate tangents
        }//for[all hair strands]

        // create vertex data from arrays
        std::vector<Eigen::Matrix<float, 3, 1>> vertices;
        std::vector<Eigen::Matrix<float, 3, 1>> tangents;
        for (int i = 0; i < numFloats; i+=3) {
            Eigen::Matrix<float, 3, 1> v(pointArray[i], pointArray[i+1], pointArray[i+2]);
            Eigen::Matrix<float, 3, 1> t(tangentArray[i], tangentArray[i+1], tangentArray[i+2]);
            vertices.push_back(v);
            tangents.push_back(t);
        } //for[vertex data array]

        pHair->vertices(&vertices);
        pHair->tangents(&tangents);

        pHair->start(start);
        pHair->count(count);

        fclose(fp);
    }//loadCyHairFile

    float* HairIO::computeStrandTangents(float* tangentArray, std::vector<Eigen::Matrix<float, 3, 1>> strandPoints, int currentArrayIndex, int numStrandPoints) {
        int strandStartIndex = currentArrayIndex - (numStrandPoints * 3); // currentArrayIndex is at the end of the current strand, so you have to subtract the number of points of the strand
        for (unsigned int p = 0; p < numStrandPoints; p++) {
            float strandTangentX, strandTangentY, strandTangentZ, strandTangentLength;
            if (p == 0) { // first point
                strandTangentX = strandPoints[1].x() - strandPoints[0].x();
                strandTangentY = strandPoints[1].y() - strandPoints[0].y();
                strandTangentZ = strandPoints[1].z() - strandPoints[0].z();
            }
            else if (p == numStrandPoints - 1) { // last point
                strandTangentX = strandPoints[numStrandPoints - 1].x() - strandPoints[numStrandPoints - 2].x();
                strandTangentY = strandPoints[numStrandPoints - 1].y() - strandPoints[numStrandPoints - 2].y();
                strandTangentZ = strandPoints[numStrandPoints - 1].z() - strandPoints[numStrandPoints - 2].z();
            }
            else { // point in between
                strandTangentX = (strandPoints[p].x() - strandPoints[p - 1].x()) + (strandPoints[p + 1].x() - strandPoints[p].x());
                strandTangentY = (strandPoints[p].y() - strandPoints[p - 1].y()) + (strandPoints[p + 1].y() - strandPoints[p].y());
                strandTangentZ = (strandPoints[p].z() - strandPoints[p - 1].z()) + (strandPoints[p + 1].z() - strandPoints[p].z());
            }
            // normalize
            strandTangentLength = sqrt(strandTangentX * strandTangentX + strandTangentY * strandTangentY + strandTangentZ * strandTangentZ);
            strandTangentX /= strandTangentLength;
            strandTangentY /= strandTangentLength;
            strandTangentZ /= strandTangentLength;

            tangentArray[strandStartIndex + 3 * p] = strandTangentX;
            tangentArray[strandStartIndex + 3 * p + 1] = strandTangentY;
            tangentArray[strandStartIndex + 3 * p + 2] = strandTangentZ;
            return tangentArray;
        }
    }//computeStrandTangents

    // load TressFX file (.tfx)
    // [UPDATE] implement .tfx support
    void HairIO::loadTFXFile(const std::string Filepath, T3DHair<float>* pHair) {
        throw CForgeExcept("Loading .tfx files not yet implemented.");
    }//loadTFXFile

}//name space