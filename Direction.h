#ifndef HEADER_DIRECTION
#define HEADER_DIRECTION

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

private:
    Value value;
};

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
