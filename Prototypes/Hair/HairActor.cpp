#include <glad/glad.h>
#include "HairActor.h"

#include "../../CForge/Graphics/Shader/SShaderManager.h"

namespace CForge {

    HairActor::HairActor(void): IRenderableActor("HairActor", ATYPE_STATIC) {
        m_TypeID = ATYPE_STATIC;
        m_TypeName = "Hair Actor";
    } // constructor

    HairActor::~HairActor(void) {
        clear();
    } // destructor

    void HairActor::init(const T3DHair<float>* pHair) {
        if (nullptr == pHair) throw NullpointerExcept("pHair");
        if (pHair->vertexCount() == 0) throw CForgeExcept("Hair contains no vertex data");

        uint16_t VertexProperties = 0;
        if (pHair->vertexCount() > 0) {
            VertexProperties |= VertexUtility::VPROP_POSITION;
            VertexProperties |= VertexUtility::VPROP_TANGENT;
        }

        m_numStrands = pHair->strandCount();
        m_start = pHair->start();
        m_count = pHair->count();

        // build array buffer of vertex data
        void* pBuffer = nullptr;
        uint32_t BufferSize = 0;

        try {
            m_VertexUtility.init(VertexProperties);
            buildBuffer(&pBuffer, &BufferSize, pHair);
            m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, pBuffer, BufferSize);
            // free buffer data
            delete[] pBuffer;
            pBuffer = nullptr;
            BufferSize = 0;
        }
        catch (CrossForgeException& e) {
            SLogger::logException(e);
            return;
        }
        catch (...) {
            SLogger::log("Unknown exception occurred during vertex buffer creation!");
            return;
        }

        // init shaders
        try {
            initShaders(pHair);
        }
        catch (CrossForgeException& e) {
            SLogger::logException(e);
            return;
        }
        catch (...) {
            SLogger::log("Unknown exception occurred during building of index buffer!");
            return;
        }

        m_VertexArray.init();
        initBufferData();
    } // initialize

    void HairActor::clear(void) {
        m_VertexBuffer.clear();
        m_VertexArray.clear();
        m_VertexUtility.clear();
    } // clear

    void HairActor::release(void) {
        delete this;
    } // release

    RenderMaterial* HairActor::material() {
        return &(m_material);
    } // material

    void HairActor::render(RenderDevice* pRDev) {
        if (nullptr == pRDev) throw NullpointerExcept("pRDev");
        if (m_pShader == nullptr) throw NullpointerExcept("Hair has no shader!");

        m_VertexArray.bind();

        switch (pRDev->activePass()) {
        case RenderDevice::RENDERPASS_SHADOW: {
            //if (nullptr == pShaderShadowPass) continue;
            pRDev->activeShader(pRDev->shadowPassShader());
        }break;
        case RenderDevice::RENDERPASS_GEOMETRY: {
            //if (nullptr == pShaderGeometryPass) continue;
            //pRDev->activeShader(m_pShader);
            //pRDev->activeMaterial(&m_material);
        }break;
        case RenderDevice::RENDERPASS_FORWARD: {
            //if (nullptr == pShaderForwardPass) continue;
            pRDev->activeShader(m_pShader);
            pRDev->activeMaterial(&m_material);
        }break;
        }

        glMultiDrawArrays(GL_LINE_STRIP, m_start, m_count, m_numStrands); // [TODO] hair density
    } // render

    void HairActor::initBufferData(void) {
        m_VertexArray.bind();
        m_VertexBuffer.bind();

        uint16_t vertexSize = m_VertexUtility.vertexSize();
        // position array
        if (m_VertexUtility.hasProperties(VertexUtility::VPROP_POSITION)) {
            uint32_t positionAttributeIndex = GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION);
            glEnableVertexAttribArray(positionAttributeIndex);
            glVertexAttribPointer(positionAttributeIndex, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)m_VertexUtility.offset(VertexUtility::VPROP_POSITION));
        }
        // tangent array
        if (m_VertexUtility.hasProperties(VertexUtility::VPROP_TANGENT)) {
            uint32_t tangentAttributeIndex = GLShader::attribArrayIndex(GLShader::ATTRIB_TANGENT);
            glEnableVertexAttribArray(tangentAttributeIndex);
            glVertexAttribPointer(tangentAttributeIndex, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)m_VertexUtility.offset(VertexUtility::VPROP_TANGENT));
        }

        m_VertexArray.unbind();
    }

    void HairActor::buildBuffer(void** ppBuffer, uint32_t* pBufferSize, const T3DHair<float>* pHair) {
        if (nullptr == ppBuffer) throw NullpointerExcept("ppBuffer");
        if (nullptr == pBufferSize) throw NullpointerExcept("pBufferSize");

        uint32_t vertexCount = pHair->vertexCount();
        if (0 == vertexCount) throw CForgeExcept("Invalid vertex count for buffer building specified!");

        uint16_t positionOffset = m_VertexUtility.offset(VertexUtility::VPROP_POSITION);
        uint16_t tangentOffset = m_VertexUtility.offset(VertexUtility::VPROP_TANGENT);
        uint16_t vertexSize = m_VertexUtility.vertexSize();

        if (0 == vertexSize) throw CForgeExcept("Vertex utility was not initialized. Vertex size is still 0!");

        // compute size and allocate memory
        uint32_t BufferSize = vertexSize * vertexCount;
        uint8_t* pBuf = new uint8_t[BufferSize];
        if (nullptr == pBuf) throw OutOfMemoryExcept("pBuf");
        // initialize memory with zeros
        memset(pBuf, 0, BufferSize);

        // assign return values
        (*ppBuffer) = (void*)pBuf;
        (*pBufferSize) = BufferSize;

        if (nullptr == pHair || 0 == pHair->vertexCount()) return; // thats it

        // initialize buffer with mesh values (if available)
        for (uint32_t i = 0; i < vertexCount; ++i) {

            // positions
            uint32_t BufferOffset = i * vertexSize + positionOffset;
            float* pV = (float*)&pBuf[BufferOffset];
            pV[0] = pHair->vertex(i).x();
            pV[1] = pHair->vertex(i).y();
            pV[2] = pHair->vertex(i).z();

            // tangents
            BufferOffset = i * vertexSize + tangentOffset;
            pV = (float*)&pBuf[BufferOffset];
            pV[0] = pHair->tangent(i).x();
            pV[1] = pHair->tangent(i).y();
            pV[2] = pHair->tangent(i).z();

        } // for[number of vertices]
    } // buildBuffer

    void HairActor::initShaders(const T3DHair<float>* pHair) {
        if (nullptr == pHair) throw NullpointerExcept("pHair");

        // fill buffer and build render groups
        SShaderManager* pSMan = SShaderManager::instance();

        int32_t UsedMaterial = 0;

        // initialize shader
        if (UsedMaterial != -1) {

            std::vector<ShaderCode*> VSSources;
            std::vector<ShaderCode*> FSSources;
            std::string ErrorLog;

            const T3DHair<float>::Material* pMat = pHair->getMaterial();

            try {
                for (auto k : pMat->VertexShaderSources) {
                    uint8_t ConfigOptions = 0;

                    ShaderCode* pC = pSMan->createShaderCode(k, "330 core", ConfigOptions, "highp");
                    VSSources.push_back(pC);
                }

                for (auto k : pMat->FragmentShaderSources) {
                    uint8_t ConfigOptions = 0;

                    ShaderCode* pC = pSMan->createShaderCode(k, "330 core", ConfigOptions, "highp");
                    FSSources.push_back(pC);
                }

                m_pShader = pSMan->buildShader(&VSSources, &FSSources, &ErrorLog);

                if (!ErrorLog.empty()) SLogger::log("Building shader failed:\n" + ErrorLog);
            }
            catch (const CrossForgeException& e) {

                SLogger::log("Building shader failed!\n");
            }

        }

        // initialize material
        if (UsedMaterial != -1) {
            try {
                m_material.clear();
            }
            catch (CrossForgeException& e) {
                SLogger::logException(e);
                m_material.clear();
            }
        }//if[initialize material]

        pSMan->release();
        pSMan = nullptr;

    }//initShaders

}//name space