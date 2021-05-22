#ifndef HEADER_POS
#define HEADER_POS

#include "Direction.h"
#include <iosfwd>

struct Pos {
    int x, y, z;

    Pos(int x, int y, int z);

    Pos nextInDirection(Direction d) const;
    void print(const char *description = nullptr) const;
    bool operator==(const Pos &other) const;

    friend std::ostream &operator<<(std::ostream &os, const Pos &p);
};

#endif // HEADER_POS
