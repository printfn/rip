#ifndef HEADER_DIRECTION
#define HEADER_DIRECTION

#include <iosfwd>

class Direction {
public:
    enum Value {
        XP, XN, YP, YN, ZP, ZN
    };

    Direction(Value value);

    // Allow using Direction in switch statements
    operator Value() const;

    // Disallow using Direction in if statements (e.g. if (direction))
    explicit operator bool() = delete;

    friend std::ostream &operator<<(std::ostream &os, const Direction &dir);

    Direction opposite() const;

private:
    Value value;
};

static Direction ALL_DIRECTIONS[6] = {
    Direction::XP,
    Direction::XN,
    Direction::YP,
    Direction::YN,
    Direction::ZP,
    Direction::ZN,
};

#endif // HEADER_DIRECTION
