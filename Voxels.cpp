#include "Voxels.h"
#include "Direction.h"
#include "Pos.h"
#include "utils.h"
#include <iostream>

Voxels::Voxels(int width, int height, int depth) : width{width}, height{height} {
    for (int i = 0; i < width * height * depth; ++i) {
        voxels.push_back(0);
    }
}

int Voxels::maxX() const {
    return voxels.size() / width / height;
}

int Voxels::maxY() const {
    return height;
}

int Voxels::maxZ() const {
    return width;
}

bool Voxels::isInRange(Pos p) const {
    if (p.x < 0 || p.y < 0 || p.z < 0) return false;
    if (p.x >= maxX() || p.y >= maxY() || p.z >= maxZ()) return false;
    return true;
}

bool Voxels::existsAt(Pos p) const {
    if (p.x < 0 || p.y < 0 || p.z < 0) return false;
    if (p.x >= maxX() || p.y >= maxY() || p.z >= maxZ()) return false;
    int val = (*this)[p];
    return val != 0;
}

int Voxels::operator[](Pos p) const {
    if (!isInRange(p)) {
        std::cerr << "tried to get out of range position " << p << std::endl;
        fail();
    }
    return voxels[p.x * width * height + p.y * width + p.z];
}

int &Voxels::operator[](Pos p) {
    if (!isInRange(p)) {
        std::cerr << "tried to get out of range position " << p << std::endl;
        fail();
    }
    return voxels[p.x * width * height + p.y * width + p.z];
}

int Voxels::numNeighboursAt(Pos p) const {
    int num = 0;
    if (existsAt(p.nextInDirection(Direction::XP))) ++num;
    if (existsAt(p.nextInDirection(Direction::XN))) ++num;
    if (existsAt(p.nextInDirection(Direction::YP))) ++num;
    if (existsAt(p.nextInDirection(Direction::YN))) ++num;
    if (existsAt(p.nextInDirection(Direction::ZP))) ++num;
    if (existsAt(p.nextInDirection(Direction::ZN))) ++num;
    return num;
}

std::ostream &operator<<(std::ostream &os, const Voxels &v) {
    int mx = v.maxX();
    int my = v.maxY();
    int mz = v.maxZ();

    os << "Dimensions: " << mx << "x" << my << "x" << mz << std::endl;
    for (int x = 0; x < mx; ++x) {
        for (int y = 0; y < my; ++y) {
            for (int z = 0; z < mz; ++z) {
                os << v[Pos{x, y, z}];
            }
            if (y < my - 1) {
                os << " ";
            }
        }
        if (x < mx - 1) {
            os << std::endl;
        }
    }
    return os;
}
