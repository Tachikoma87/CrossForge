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
                          Vector3f::Zero(),
                          AngleAxisf(0, Vector3f::UnitX()) *
                                  AngleAxisf(GraphicsUtility::degToRad(static_cast<float>(mData.orientation) * 90.0f), Vector3f::UnitY()) *
                                  AngleAxisf(0, Vector3f::UnitZ()),
                          Vector3f::Ones() * mData.level * (data.variant == Tile::Trim ? 2 : 1));
    }

    void TileNode::update(float cameraX , float cameraY) {
        auto scale = 2.0f * static_cast<float>(mData.level);
        float x = floorf(cameraX / scale) * scale;
        float y = floorf(cameraY / scale) * scale;

        Vector3f pos =Vector3f(
            x + mData.pos.x(),
            0,
            y + mData.pos.y());

        if (mData.variant == Tile::Trim) {
            float nextScale = 2.0f * scale;
            float nextX = floorf(cameraX / nextScale) * nextScale;
            float nextY = floorf(cameraY / nextScale) * nextScale;
            bool rot = false;

            // Todo generate trim as 1 mesh
            if (x != nextX) {
                pos.x() = x + -mData.pos.x() + scale;
                rot = true;
            }
            if (y != nextY){
                pos.z() = y + -mData.pos.y() + scale;
                rot = true;
            }

            rotation(AngleAxisf(0, Vector3f::UnitX()) *
                     AngleAxisf(GraphicsUtility::degToRad(
                         static_cast<float>(mData.orientation + (rot ? 2 : 0)) * 90.0f), Vector3f::UnitY()) *
                     AngleAxisf(0, Vector3f::UnitZ()));
        }

        position(pos);
    }
}
