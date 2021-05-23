#ifndef HEADER_VOXEL_PIECE
#define HEADER_VOXEL_PIECE

#include "Direction.h"

struct VoxelPiece {
    float r, g, b;
    float dx, dy, dz;
    float movementStart;

    VoxelPiece(int pieceIdx, Direction dir);
};

#endif // HEADER_VOXEL_PIECE
