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

void Pos::print(const char *description) const {
    if (description) {
        printf("%s: ", description);
    }
    printf("(%i,%i,%i)\n", x, y, z);
}

bool Pos::operator==(const Pos &other) const {
    return x == other.x && y == other.y && z == other.z;
}

std::ostream &operator<<(std::ostream &os, const Pos &p) {
    return os << "Pos{" << p.x << ", " << p.y << ", " << p.z << "}" << std::endl;
}
