#ifndef HEADER_VOXELS
#define HEADER_VOXELS

#include "VoxelPiece.h"

#include <vector>
#include <string>

struct Pos;

class Voxels {
    int width = 0;
    int height = 0;
    std::vector<int> voxels;
    mutable std::vector<std::vector<double>> accessibilityCache;
    mutable std::vector<VoxelPiece> voxelPieceProperties;

public:
    Voxels(int width, int height, int depth);

    static Voxels readFile(const std::string &filename);

    int maxX() const;
    int maxY() const;
    int maxZ() const;

    bool isInRange(Pos p) const;
    bool existsAt(Pos p) const;

    int operator[](Pos p) const;
    int &operator[](Pos p);
    void print(bool detailed = false) const;

    int numNeighboursAt(Pos p) const;
    int numExteriorFaces(Pos p) const;
    bool hasFreePassage(Pos p, Direction dir, bool checkLowerRank) const;
    int maxPieceIdx() const;
    int totalVoxelCount() const;

    double accessibilityHeuristic(Pos p, int j) const;
    void invalidateAccessibilityHeuristic() const;

    VoxelPiece &propertiesForPiece(int piece);
    const VoxelPiece &propertiesForPiece(int piece) const;

    friend std::ostream &operator<<(std::ostream &os, const Voxels &v);
};

#endif // HEADER_VOXELS
