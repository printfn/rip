#include "Direction.h"

const char *printDir(Direction d) {
    switch (d) {
        case Direction::XP: return "+x";
        case Direction::XN: return "-x";
        case Direction::YP: return "+y";
        case Direction::YN: return "-y";
        case Direction::ZP: return "+z";
        case Direction::ZN: return "-z";
    }
}

Direction oppositeDirection(Direction d) {
    switch (d) {
        case Direction::XP: return Direction::XN;
        case Direction::XN: return Direction::XP;
        case Direction::YP: return Direction::YN;
        case Direction::YN: return Direction::YP;
        case Direction::ZP: return Direction::ZN;
        case Direction::ZN: return Direction::ZP;
    }
}

std::vector<Direction> directions() {
    return {
        Direction::XP,
        Direction::XN,
        Direction::YP,
        Direction::YN,
        Direction::ZP,
        Direction::ZN,
    };
}
