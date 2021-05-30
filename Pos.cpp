#include "Pos.h"
#include <iostream>

Pos::Pos(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Pos Pos::nextInDirection(Direction d) const {
    auto res = *this;
    switch (d) {
        case Direction::XP: ++res.x; break;
        case Direction::XN: --res.x; break;
        case Direction::YP: ++res.y; break;
        case Direction::YN: --res.y; break;
        case Direction::ZP: ++res.z; break;
        case Direction::ZN: --res.z; break;
    }
    return res;
}

bool Pos::operator==(const Pos &other) const {
    return x == other.x && y == other.y && z == other.z;
}

std::ostream &operator<<(std::ostream &os, const Pos &p) {
    return os << "Pos{" << p.x << ", " << p.y << ", " << p.z << "}";
}

bool Pos::isInLine(const Pos &other, Direction dir) const {
    if (*this == other) {
        return true;
    }

    switch (dir) {
        case Direction::XP: return this->x > other.x && this->y == other.y && this->z == other.z;
        case Direction::XN: return this->x < other.x && this->y == other.y && this->z == other.z;
        case Direction::YP: return this->x == other.x && this->y > other.y && this->z == other.z;
        case Direction::YN: return this->x == other.x && this->y < other.y && this->z == other.z;
        case Direction::ZP: return this->x == other.x && this->y == other.y && this->z > other.z;
        case Direction::ZN: return this->x == other.x && this->y == other.y && this->z < other.z;
    }

    return false;
}
