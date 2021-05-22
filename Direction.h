#ifndef HEADER_DIRECTION
#define HEADER_DIRECTION

#include <vector>

enum class Direction { XP, XN, YP, YN, ZP, ZN };

const char *printDir(Direction d);
Direction oppositeDirection(Direction d);
std::vector<Direction> directions();

#endif // HEADER_DIRECTION
