#include "Voxels.h"
#include "Direction.h"
#include "Pos.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

Voxels::Voxels(int width, int height, int depth) : width{width}, height{height} {
    for (int i = 0; i < width * height * depth; ++i) {
        voxels.push_back(0);
    }
}

Voxels Voxels::readFile(const std::string &filename) {
    std::ifstream fin{filename};
    if (!fin) {
        std::cerr << "Failed to open file " << filename << ": " << strerror(errno) << std::endl;
        exit(1);
    }
    std::string line;
    if (!std::getline(fin, line)) {
        std::cerr << "Failed to read shape dimensions" << std::endl;
        exit(1);
    }
    std::stringstream strstream{line};
    int width = 0, height = 0, depth = 0;
    strstream >> width;
    strstream >> height;
    strstream >> depth;
    if (width <= 0 || height <= 0 || depth <= 0) {
        std::cerr << "Width, height and depth must all be greater than 0" << std::endl;
        exit(1);
    }
    Voxels result{width, height, depth};
    int voxelIdx = 0;
    while (std::getline(fin, line)) {
        strstream = std::stringstream{line};
        char ch = '\0';
        while (strstream >> ch) {
            if ((size_t)voxelIdx >= result.voxels.size()) {
                std::cerr << "Too many voxels: expected "
                    << (width * height * depth) << std::endl;
                exit(1);
            }
            switch (ch) {
                case '.':
                    result.voxels[voxelIdx++] = 0;
                    break;
                case 'x':
                    result.voxels[voxelIdx++] = 1;
                    break;
                default:
                    std::cerr << "Unexpected character at voxel index "
                        << voxelIdx << ": '" << ch << "'" << std::endl;
                    exit(1);
            }
        }
    }
    if (voxelIdx < width * height * depth) {
        std::cerr << "Expected " << (width * height * depth) << " voxels, "
            << "found " << voxelIdx << std::endl;
        exit(1);
    }
    return result;
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
        exit(1);
    }
    return voxels[p.x * width * height + p.y * width + p.z];
}

int &Voxels::operator[](Pos p) {
    if (!isInRange(p)) {
        std::cerr << "tried to get out of range position " << p << std::endl;
        exit(1);
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
                int value = v[Pos{x, y, z}];
                if (value == 0) {
                    os << '.';
                } else {
                    os << value;
                }
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

double Voxels::accessibilityHeuristic(Pos p, int j) const {
    if (j < 0) {
        std::cerr << "j must not be less than zero" << std::endl;
        exit(1);
    }
    const double WEIGHT_FACTOR = 0.1;
    if (j == 0) {
        double result = numNeighboursAt(p);
        return result;
    } else {
        auto result = accessibilityHeuristic(p, j - 1);
        auto weight = pow(WEIGHT_FACTOR, (double)j);
        for (auto d : ALL_DIRECTIONS) {
            auto posInD = p.nextInDirection(d);
            if (!existsAt(posInD)) continue;
            result += weight * accessibilityHeuristic(posInD, j - 1);
        }
        return result;
    }
}

void Voxels::invalidateAccessibilityHeuristic() const {
    accessibilityCache = {};
}
