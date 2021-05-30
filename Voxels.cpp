#include "Voxels.h"
#include "Direction.h"
#include "Pos.h"
#include "VoxelPiece.h"

#include <algorithm>
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
        return 0;
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

int Voxels::numExteriorFaces(Pos p) const {
    return 6 - numNeighboursAt(p);
}

bool Voxels::hasFreePassage(Pos p, Direction dir, bool checkLowerRank) const {
    int maxI = std::max(maxX(), std::max(maxY(), maxZ()));
    int pieceIndex = (*this)[p];
    for (int i = 0; i < maxI; ++i) {
        p = p.nextInDirection(dir);
        if (!existsAt(p)) continue;
        if (!checkLowerRank) return false;
        // pieces are removed starting with the higher rank, so if the
        // potentially blocking piece is higher than the current piece (`pieceIndex`),
        // we can ignore it
        if (pieceIndex >= (*this)[p]) continue;
        return false;
    }
    return true;
}

int Voxels::maxPieceIdx() const {
    int max = 0;
    for (int x = 0; x < maxX(); ++x) {
        for (int y = 0; y < maxY(); ++y) {
            for (int z = 0; z < maxZ(); ++z) {
                int piece = (*this)[{x, y, z}];
                if (piece > max) {
                    max = piece;
                }
            }
        }
    }
    return max;
}

int Voxels::totalVoxelCount() const {
    int count = 0;
    for (int x = 0; x < maxX(); ++x) {
        for (int y = 0; y < maxY(); ++y) {
            for (int z = 0; z < maxZ(); ++z) {
                if (existsAt({x, y, z})) {
                    ++count;
                }
            }
        }
    }
    return count;
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

Direction movableDirection(const Voxels &v, int piece) {
    if (piece == 0) {
        std::cerr << "Piece 0 is invalid" << std::endl;
        exit(1);
    }
    bool isXPBlocked = false;
    bool isXNBlocked = false;
    bool isYPBlocked = false;
    bool isYNBlocked = false;
    bool isZPBlocked = false;
    bool isZNBlocked = false;
    for (int x = 0; x < v.maxX(); ++x) {
        for (int y = 0; y < v.maxY(); ++y) {
            for (int z = 0; z < v.maxZ(); ++z) {
                Pos p = {x, y, z};
                if (v[p] != piece) continue;
                for (Direction d : ALL_DIRECTIONS) {
                    if (!v.hasFreePassage(p, d, true)) {
                        //std::cerr << "Piece at pos " << p << " can't move in direction " << d << std::endl;
                        switch (d) {
                            case Direction::XP: isXPBlocked = true; break;
                            case Direction::XN: isXNBlocked = true; break;
                            case Direction::YP: isYPBlocked = true; break;
                            case Direction::YN: isYNBlocked = true; break;
                            case Direction::ZP: isZPBlocked = true; break;
                            case Direction::ZN: isZNBlocked = true; break;
                        }
                    }
                }
            }
        }
    }
    if (!isXPBlocked) return Direction::XP;
    if (!isXNBlocked) return Direction::XN;
    if (!isYPBlocked) return Direction::YP;
    if (!isYNBlocked) return Direction::YN;
    if (!isZPBlocked) return Direction::ZP;
    if (!isZNBlocked) return Direction::ZN;
    std::cerr << "Piece " << piece << " is blocked in all directions!" << std::endl;
    return Direction::ZN;
}

VoxelPiece Voxels::propertiesForPiece(int piece) const {
    return VoxelPiece{piece, maxPieceIdx(), movableDirection(*this, piece)};
}
