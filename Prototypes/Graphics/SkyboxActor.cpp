#include "../../CForge/Graphics/RenderDevice.h"
#include "../../CForge/Core/SLogger.h"
#include "SkyboxActor.h"

#include <glad/glad.h>

#include "../../CForge/AssetIO/SAssetIO.h"

using namespace Eigen;

namespace CForge {

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    int32_t SkyboxIndices[] = {
        0,1,2,
        3,4,5,

        6,7,8,
        9,10,11,

        12,13,14,
        15,16,17,

        18,19,20,
        21,22,23,

        24,25,26,
        27,28,29,

        30,31,32,
        33,34,35
    };


	SkyboxActor::SkyboxActor(void): IRenderableActor("SkyboxActor", 12) {


	}//Constructor

	SkyboxActor::~SkyboxActor(void) {

	}//Destructor

	void SkyboxActor::init(std::string Right, std::string Left, std::string Top, std::string Bottom, std::string Back, std::string Front) {

        // create skybox data (unit cube)
        T3DMesh<float> M;
        T3DMesh<float>::Submesh Sub;
        std::vector<Vector3f> Vertices;
        // create vertices
        for (uint32_t i = 0; i < 108/3 ; i++) {
            Vertices.push_back(Vector3f(skyboxVertices[i*3 + 0], skyboxVertices[i*3 + 1], skyboxVertices[i*3 + 2]));
        }


        // create faces
        for (uint32_t i = 0; i < 12; ++i) {
            T3DMesh<float>::Face F;
            F.Vertices[0] = SkyboxIndices[i*3 + 0];
            F.Vertices[1] = SkyboxIndices[i*3 + 1];
            F.Vertices[2] = SkyboxIndices[i*3 + 2];
            F.Material = 0;
            Sub.Faces.push_back(F);
        }
        
        M.vertices(&Vertices);
        M.addSubmesh(&Sub, true);

        T3DMesh<float>::Material Mat;
        Mat.ID = 0;
        Mat.VertexShaderSources.push_back("Shader/Skybox.vert");
        Mat.FragmentShaderSources.push_back("Shader/Skybox.frag");
        M.addMaterial(&Mat, true);

        uint16_t VertexProperties = VertexUtility::VPROP_POSITION;

        uint32_t BufferSize;
        void* pBuffer = nullptr;
        try {
            m_VertexUtility.init(VertexProperties);
            m_VertexUtility.buildBuffer(M.vertexCount(), &pBuffer, &BufferSize, &M);
            m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);

            delete[] pBuffer;
            pBuffer = nullptr;
            BufferSize = 0;
        }
        catch (const CrossForgeException& e) {
            SLogger::logException(e);
            return;
        }

        // build render groups and element array
        try {
            m_RenderGroupUtility.init(&M, &pBuffer, &BufferSize);
            m_ElementBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
            delete[] pBuffer;
            pBuffer = nullptr;
            BufferSize = 0;
        }
        catch (const CrossForgeException& e) {
            SLogger::logException(e);
            return;
        }

        m_VertexArray.init();
        m_VertexArray.bind();
        setBufferData();
        m_VertexArray.unbind();

        // initialize cubemap
        m_Cubemap.clear();

        T2DImage<uint8_t> CubeImages[6];
        try {
            AssetIO::load(Right, &CubeImages[0]);
            AssetIO::load(Left, &CubeImages[1]);
            AssetIO::load(Top, &CubeImages[2]);
            AssetIO::load(Bottom, &CubeImages[3]);
            AssetIO::load(Back, &CubeImages[4]);
            AssetIO::load(Front, &CubeImages[5]);
        }
        catch (const CrossForgeException& e) {
            Logger::logException(e);
            return;
        }

        m_Cubemap.init(&CubeImages[0], &CubeImages[1], &CubeImages[2], &CubeImages[3], &CubeImages[4], &CubeImages[5]);
       

	}//initialize

	void SkyboxActor::clear(void) {
        m_VertexBuffer.clear();
        m_ElementBuffer.clear();
        m_VertexArray.clear();

        m_VertexUtility.clear();
        m_RenderGroupUtility.clear();
	}//clear

	void SkyboxActor::release(void) {
		delete this;
	}

	void SkyboxActor::render(RenderDevice* pRDev) {

        m_VertexArray.bind();

        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);

        for (auto i : m_RenderGroupUtility.renderGroups()) {
            if (i->pShader == nullptr) continue;

            if (pRDev->activePass() == RenderDevice::RENDERPASS_FORWARD) {
                pRDev->activeShader(i->pShader);
                pRDev->activeMaterial(&i->Material);

                m_Cubemap.bind();

                glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBuffer.size() / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
                //glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }//for[all render groups]

        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
	}//render

}//name space