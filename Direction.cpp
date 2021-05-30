#include "Direction.h"
#include <iostream>

Direction::Direction(Value value) : value{value} {}

Direction::operator Value() const {
    return value;
}

std::ostream &operator<<(std::ostream &os, const Direction &dir) {
    switch (dir) {
        case Direction::XP:
            os << "+x";
            break;
        case Direction::XN:
            os << "-x";
            break;
        case Direction::YP:
            os << "+y";
            break;
        case Direction::YN:
            os << "-y";
            break;
        case Direction::ZP:
            os << "+z";
            break;
        case Direction::ZN:
            os << "-z";
            break;
    }
    return os;
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
    return Direction::XP;
}

bool Direction::isPerpendicular(Direction other) const {
    switch (*this) {
        case Direction::XP:
        case Direction::XN:
            return other == Direction::YP || other == Direction::YN || other == Direction::ZP || other == Direction::ZN;
        case Direction::YP:
        case Direction::YN:
            return other == Direction::XP || other == Direction::XN || other == Direction::ZP || other == Direction::ZN;
        case Direction::ZP:
        case Direction::ZN:
            return other == Direction::XP || other == Direction::XN || other == Direction::YP || other == Direction::YN;
    }
    return false;
}
