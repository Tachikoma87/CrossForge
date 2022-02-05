//#include "../../CForge/AssetIO/SAssetIO.h"
//#include "../../CForge/Graphics/RenderDevice.h"
//#include "../../CForge/Core/SLogger.h"

#include "../../AssetIO/SAssetIO.h"
#include "../RenderDevice.h"
#include "../../Core/SLogger.h"

#include "SkyboxActor.h"
#include <glad/glad.h>



using namespace Eigen;

namespace CForge {

    float SkyboxVertices[] = {
        // bottom vertices (CCW)
       -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,

       // to vertices (CCW)
       -1.0f, 1.0f, -1.0f,
       1.0f, 1.0f, -1.0f,
       1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
    };//skyboxVertices

    int32_t SkyboxIndices[] = {
       // right
        1,2,5,
        6,5,2,

        // left
        3,0,4,
        3,4,7,

        // top
        7,4,5,
        7,5,6,

        // bottom
        0,3,1,
        1,3,2,

        // back
        2,3,7,
        2,7,6,

        // front
        0,1,4,
        5,4,1,
    };//skyboxIndices

	SkyboxActor::SkyboxActor(void): IRenderableActor("SkyboxActor", 12) {


	}//Constructor

	SkyboxActor::~SkyboxActor(void) {
        clear();
	}//Destructor

	void SkyboxActor::init(std::string Right, std::string Left, std::string Top, std::string Bottom, std::string Back, std::string Front) {
        clear();

        // create skybox data (unit cube)
        T3DMesh<float> M;
        T3DMesh<float>::Submesh Sub;
        std::vector<Vector3f> Vertices;
        // create vertices
        for (uint32_t i = 0; i < 8 ; i++) {
            Vertices.push_back(Vector3f(SkyboxVertices[i*3 + 0], SkyboxVertices[i*3 + 1], SkyboxVertices[i*3 + 2]));
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

            CubeImages[0].rotate180();
            CubeImages[1].rotate180();
            CubeImages[4].rotate180();
            CubeImages[5].rotate180();
        }
        catch (const CrossForgeException& e) {
            Logger::logException(e);
            return;
        }

        m_Cubemap.init(&CubeImages[0], &CubeImages[1], &CubeImages[2], &CubeImages[3], &CubeImages[4], &CubeImages[5]);  
        m_ColorAdjustUBO.init();
        contrast(1.0f);
        brightness(1.0f);
        saturation(1.0f);
	}//initialize

	void SkyboxActor::clear(void) {
        m_VertexBuffer.clear();
        m_ElementBuffer.clear();
        m_VertexArray.clear();

        m_VertexUtility.clear();
        m_RenderGroupUtility.clear();
        m_ColorAdjustUBO.clear();
	}//clear

	void SkyboxActor::release(void) {
		delete this;
	}

	void SkyboxActor::render(RenderDevice* pRDev) {
        if (pRDev->activePass() != RenderDevice::RENDERPASS_FORWARD) return;

        m_VertexArray.bind();

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        for (auto i : m_RenderGroupUtility.renderGroups()) {
            if (i->pShader == nullptr) continue;

            pRDev->activeShader(i->pShader);
            pRDev->activeMaterial(&i->Material);
            m_Cubemap.bind();

            uint32_t BindingPoint = i->pShader->uboBindingPoint(GLShader::DEFAULTUBO_COLORADJUSTMENT); // i->pShader->uboBindingPoint("ColorAdjustmentData");
            if (BindingPoint != GL_INVALID_INDEX) m_ColorAdjustUBO.bind(BindingPoint);        

            glDrawRangeElements(GL_TRIANGLES, 0, m_ElementBuffer.size() / sizeof(unsigned int), i->Range.y() - i->Range.x(), GL_UNSIGNED_INT, (const void*)(i->Range.x() * sizeof(unsigned int)));
            
        }//for[all render groups]
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

	}//render

    void SkyboxActor::saturation(float Saturation) {
        m_Saturation = Saturation;
        m_ColorAdjustUBO.saturation(m_Saturation);
    }//saturation

    void SkyboxActor::brightness(float Brightness) {
        m_Brightness = Brightness;
        m_ColorAdjustUBO.brigthness(m_Brightness);
    }//brightness

    void SkyboxActor::contrast(float Contrast) {
        m_Contrast = Contrast;
        m_ColorAdjustUBO.contrast(m_Contrast);
    }//contrast

    float SkyboxActor::saturation(void)const {
        return m_Saturation;
    }//saturation

    float SkyboxActor::brightness(void)const {
        return m_Brightness;
    }//brightness

    float SkyboxActor::contrast(void)const {
        return m_Contrast;
    }//contrast

}//name space