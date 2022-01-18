#pragma once

#include <CForge/Graphics/RenderDevice.h>
#include <CForge/Graphics/SceneGraph/SGNTransformation.h>
#include "HeightMap.h"
#include "../ArrayTexture.h"
#include "TileNode.h"

using namespace CForge;
using namespace Eigen;

namespace Terrain {
    class TerrainMap {
    public:
        TerrainMap(SGNTransformation *rootTransform);
        ~TerrainMap();

        void update(float cameraX , float cameraY);
        void erode();

        void generateClipMap(ClipMap::ClipMapConfig clipMapConfig);
        void generateHeightMap(HeightMap::HeightMapConfig heightMapConfig);

        void heightMapFromTexture(GLTexture2D* texture, float mapHeight);

        Vector2f getMapSize();
        float getHeightAt(float x, float y);
        Vector3f getNormalAt(float x, float y);

        void render(RenderDevice* renderDevice, ClipMap::TileVariant variant);
        void bindTexture();
    private:
        void clear();
        void initShader();
        void generateClipMapTiles();

        ClipMap mClipMap;
        HeightMap mHeightMap;

        ArrayTexture mTextures;

        GLShader* mShader;

        SGNTransformation* mRootTransform;
        vector<TileNode*> mTileNodes;
    };
}
