#include "VoxelPiece.h"

void setColorForIndex(VoxelPiece &piece, int i) {
    switch (i) {
        case 0:
            piece.r = 1;
            piece.g = 0;
            piece.b = 0;
            break;
        case 1:
            piece.r = 0;
            piece.g = 1;
            piece.b = 0;
            break;
        case 2:
            piece.r = 0;
            piece.g = 0;
            piece.b = 1;
            break;
        case 3:
            piece.r = 1;
            piece.g = 1;
            piece.b = 0;
            break;
        case 4:
            piece.r = 1;
            piece.g = 0;
            piece.b = 1;
            break;
        default:
            piece.r = 0;
            piece.g = 1;
            piece.b = 1;
            break;
    }
}

VoxelPiece::VoxelPiece(int pieceIdx, int numPieces, Direction dir) {
    setColorForIndex(*this, pieceIdx);
    switch (dir) {
        case Direction::XP: dx = 1; dy = 0; dz = 0; break;
        case Direction::XN: dx = -1; dy = 0; dz = 0; break;
        case Direction::YP: dx = 0; dy = 1; dz = 0; break;
        case Direction::YN: dx = 0; dy = -1; dz = 0; break;
        case Direction::ZP: dx = 0; dy = 0; dz = 1; break;
        case Direction::ZN: dx = 0; dy = 0; dz = -1; break;
    }
    if (pieceIdx == 1) {
        movementStart = (numPieces - 1) * 15;
    } else {
        movementStart = (pieceIdx - 2) * 15;
    }
}
