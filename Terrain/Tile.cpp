#include "Tile.h"

namespace Terrain {
    Tile::Tile(uint sideLength) : mSideLength(sideLength), mVertexCount(sideLength + 1) {
        for (int y = 0; y < mVertexCount; y++) {
            for (int x = 0; x < mVertexCount; x++) {
                float percentX = (float) x / (float) sideLength;
                float percentY = (float) y / (float) sideLength;
                mVertices.push_back((percentX - 0.5f)); // * (float) sideLength);
                mVertices.push_back((percentY - 0.5f)); // * (float) sideLength);
                mVertices.push_back(0);
            }
        }
    }

    Tile::~Tile() = default;

    void Tile::calculateIndices(TileVariant variant) {
        for (int y = 0; y < mSideLength; y += 2) {
            for (int x = 0; x < mSideLength; x += 2) {
                auto a = y * mVertexCount + x;
                auto b = a + 1;
                auto c = a + 2;
                auto d = a + mVertexCount;
                auto e = a + mVertexCount + 1;
                auto f = a + mVertexCount + 2;
                auto g = a + 2 * mVertexCount;
                auto h = a + 2 * mVertexCount + 1;
                auto i = a + 2 * mVertexCount + 2;

                if (x == 0 && variant != TileVariant::Normal) {
                    addTriangle(e, g, a);
                } else {
                    addTriangle(e, g, d);
                    addTriangle(e, d, a);
                }

                if (y == 0 && variant == TileVariant::Corner) {
                    addTriangle(e, a, c);
                } else {
                    addTriangle(e, a, b);
                    addTriangle(e, b, c);
                }

                addTriangle(e, c, f);
                addTriangle(e, f, i);
                addTriangle(e, i, h);
                addTriangle(e, h, g);
            }
        }
    }

    GLuint* Tile::getIndices() {
        mIndices.data();
    }

    GLfloat* Tile::getVertices() {
        mVertices.data();
    }

    uint Tile::getIndexBufferSize() {
        return mIndices.size() * sizeof(GLuint);
    }

    uint Tile::getVertexBufferSize() {
        return mVertices.size() * sizeof(GLfloat);
    }

    void Tile::addTriangle(uint a, uint b, uint c) {
        mIndices.push_back(a);
        mIndices.push_back(b);
        mIndices.push_back(c);
    }
}