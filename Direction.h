#ifndef HEADER_DIRECTION
#define HEADER_DIRECTION

enum class Direction { XP, XN, YP, YN, ZP, ZN };

const char *printDir(Direction d);
Direction oppositeDirection(Direction d);

static Direction ALL_DIRECTIONS[6] = {
    Direction::XP,
    Direction::XN,
    Direction::YP,
    Direction::YN,
    Direction::ZP,
    Direction::ZN,
};

#endif // HEADER_DIRECTION
