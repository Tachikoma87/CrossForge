#include <CForge/Graphics/GraphicsUtility.h>
#include "TileNode.h"

namespace Terrain {

    TileNode::TileActor::TileActor(Tile* tile, Tile::TileVariant variant)
        : IRenderableActor("MapActor", ATYPE_STATIC), mTile(tile), mVariant(variant) {}

    void TileNode::TileActor::release() {
        delete this;
    }

    void TileNode::TileActor::render(RenderDevice* renderDevice) {
        mTile->render(renderDevice, mVariant);
    }

    TileNode::TileNode(ISceneGraphNode* parent, Tile* tile, const TileData& data)
        : SGNGeometry(), mTileActor(tile, data.variant), mData(data) {
        SGNGeometry::init(parent, &mTileActor,
                          Vector3f(mData.pos.x(), 0, mData.pos.y()),
                          AngleAxisf(0, Vector3f::UnitX()) *
                                  AngleAxisf(GraphicsUtility::degToRad(static_cast<float>(mData.orientation) * 90.0f), Vector3f::UnitY()) *
                                  AngleAxisf(0, Vector3f::UnitZ()),
                          Vector3f::Ones() * mData.level * (mData.variant == Tile::Trim ? 2 : 1));
    }

    void TileNode::update(float cameraX , float cameraY) {
        // snap the position of the tile to the camera
        auto scale = 2.0f * static_cast<float>(mData.level);
        float x = floorf(cameraX / scale) * scale;
        float y = floorf(cameraY / scale) * scale;

        position(Vector3f(x + mData.pos.x(), 0, y + mData.pos.y()));

        if (mData.variant == Tile::Trim) {
            // compare the
            float nextScale = 2.0f * scale;
            bool sameX = abs(x - floorf(cameraX / nextScale) * nextScale) < scale;
            bool sameY = abs(y - floorf(cameraY / nextScale) * nextScale) < scale;
            float rot;

            if (sameX && sameY) {
                rot = 180.0f;
            } else if (sameX) {
                rot = 270.0f;
            } else if (!sameY) {
                rot = 0.0f;
            } else {
                rot = 90.0f;
            }

            rotation(AngleAxisf(0, Vector3f::UnitX()) *
                     AngleAxisf(GraphicsUtility::degToRad(rot), Vector3f::UnitY()) *
                     AngleAxisf(0, Vector3f::UnitZ()));
        }
    }
}
