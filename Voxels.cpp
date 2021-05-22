#include "Voxels.h"
#include "Pos.h"

int Voxels::maxX() const {
    return voxels.size() / width / height;
}

int Voxels::maxY() const {
    return height;
}

int Voxels::maxZ() const {
    return width;
}

bool Voxels::existsAt(Pos p) const {
    if (p.x < 0 || p.y < 0 || p.z < 0) return false;
    if (p.x >= maxX() || p.y >= maxY() || p.z >= maxZ()) return false;
    int val = (*this)[p];
    return val != 0;
}

int Voxels::operator[](Pos p) const {
    return voxels[p.x * width * height + p.y * width + p.z];
}

int &Voxels::operator[](Pos p) {
    return voxels[p.x * width * height + p.y * width + p.z];
}

void Voxels::print(bool detailed) const {
    printf("Dimensions: %ix%ix%i\n", maxX(), maxY(), maxZ());
    if (detailed) {
        for (int x = 0; x < maxX(); ++x) {
            for (int y = 0; y < maxX(); ++y) {
                for (int z = 0; z < maxX(); ++z) {
                    printf("%i", (*this)[Pos(x, y, z)]);
                }
                printf("\n");
            }
            printf("\n\n");
        }
    }
}
