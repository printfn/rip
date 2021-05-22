#include "Direction.h"
#include <iostream>

Direction::Direction(Value value) : value{value} {}

Direction::operator Value() const {
    return value;
}

std::ostream &operator<<(std::ostream &os, const Direction &dir) {
    switch (dir) {
        case Direction::XP: return os << "+x";
        case Direction::XN: return os << "-x";
        case Direction::YP: return os << "+y";
        case Direction::YN: return os << "-y";
        case Direction::ZP: return os << "+z";
        case Direction::ZN: return os << "-z";
    }
}

Direction Direction::opposite() const {
    switch (*this) {
        case Direction::XP: return Direction::XN;
        case Direction::XN: return Direction::XP;
        case Direction::YP: return Direction::YN;
        case Direction::YN: return Direction::YP;
        case Direction::ZP: return Direction::ZN;
        case Direction::ZN: return Direction::ZP;
    }
}
