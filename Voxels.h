#ifndef HEADER_VOXELS
#define HEADER_VOXELS

#include <vector>

struct Pos;

class Voxels {
    int width = 0;
    int height = 0;
    std::vector<int> voxels;

public:
    Voxels(int width, int height, int depth);

    int maxX() const;
    int maxY() const;
    int maxZ() const;

    bool existsAt(Pos p) const;

    int operator[](Pos p) const;
    int &operator[](Pos p);
    void print(bool detailed = false) const;

    int numNeighboursAt(Pos p) const;
};

#endif // HEADER_VOXELS
