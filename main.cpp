#include <vector>
#include <cstdio>

enum class Direction { XP, XN, YP, YN, ZP, ZN };

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

struct Pos {
    int x, y, z;

    Pos(int x, int y, int z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Pos nextInDirection(Direction d) const {
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
    void print(const char *description = nullptr) const {
        if (description) {
            printf("%s: ", description);
        }
        printf("(%i,%i,%i)\n", x, y, z);
    }
};

struct Voxels {
    int width = 0;
    int height = 0;
    std::vector<int> voxels;

    int maxX() const {
        return voxels.size() / width / height;
    }
    int maxY() const {
        return height;
    }
    int maxZ() const {
        return width;
    }
    int operator[](Pos p) const {
        return voxels[p.x * width * height + p.y * width + p.z];
    }
    int &operator[](Pos p) {
        return voxels[p.x * width * height + p.y * width + p.z];
    }
    void print(bool detailed = false) const;
};

void Voxels::print(bool detailed) const {
    printf("Dimensions: %ix%ix%i\n", maxX(), maxY(), maxZ());
    if (detailed) {
        for (int x = 0; x < maxX(); ++x) {
            for (int y = 0; y < maxX(); ++y) {
                for (int z = 0; z < maxX(); ++z) {
                    printf("%i", (*this)[Pos(x, y, z)]);
                }
                printf("\n");
            }
            printf("\n\n");
        }
    }
}

bool exists(const Voxels &v, Pos p) {
    if (p.x < 0 || p.y < 0 || p.z < 0) return false;
    if (p.x >= v.maxX() || p.y >= v.maxY() || p.z >= v.maxZ()) return false;
    int val = v[p];
    return val != 0;
}

int numNeighbours(const Voxels &v, Pos p) {
    int num = 0;
    if (exists(v, p.nextInDirection(Direction::XP))) ++num;
    if (exists(v, p.nextInDirection(Direction::XN))) ++num;
    if (exists(v, p.nextInDirection(Direction::YP))) ++num;
    if (exists(v, p.nextInDirection(Direction::YN))) ++num;
    if (exists(v, p.nextInDirection(Direction::ZP))) ++num;
    if (exists(v, p.nextInDirection(Direction::ZN))) ++num;
    return num;
}

int numExteriorFaces(const Voxels &v, Pos p) {
    return 6 - numNeighbours(v, p);
}

bool hasFreePassage(const Voxels &v, Pos p, Direction dir) {
    for (int i = 0; i < 50; ++i) {
        p = p.nextInDirection(dir);
        if (exists(v, p)) return false;
    }
    return true;
}

struct OrientedPos {
    Pos pos;
    Direction dir;

    OrientedPos(Pos p, Direction d) : pos{p}, dir{d} {}
};

std::vector<OrientedPos> initialSeedCandidates(const Voxels &v, bool debug = false) {
    std::vector<OrientedPos> results;
    int skippedDueToWrongFaceCount = 0;
    int skippedDueToNonFreePassage = 0;
    for (int x = 0; x < v.maxX(); ++x) {
        for (int y = 0; y < v.maxX(); ++y) {
            for (int z = 0; z < v.maxX(); ++z) {
                auto p = Pos(x, y, z);
                if (!exists(v, p)) {
                    continue;
                }
                if (numExteriorFaces(v, p) != 2) {
                    ++skippedDueToWrongFaceCount;
                    continue;
                }
                if (!hasFreePassage(v, p, Direction::YP)) {
                    ++skippedDueToNonFreePassage;
                    continue;
                }
                Direction dir;
                if (!exists(v, p.nextInDirection(Direction::XP))) dir = Direction::XP;
                if (!exists(v, p.nextInDirection(Direction::XN))) dir = Direction::XN;
                if (!exists(v, p.nextInDirection(Direction::YN))) dir = Direction::YN;
                if (!exists(v, p.nextInDirection(Direction::ZP))) dir = Direction::ZP;
                if (!exists(v, p.nextInDirection(Direction::ZN))) dir = Direction::ZN;
                results.push_back(OrientedPos(p, dir));
            }
        }
    }
    if (debug) {
        printf("Found %lu initial seed candidates (rejected %i/%i)\n",
            results.size(), skippedDueToWrongFaceCount,
            skippedDueToNonFreePassage);
    }
    return results;
}

OrientedPos findInitialSeed(const Voxels &v, bool debug = false) {
    auto seeds = initialSeedCandidates(v, debug);
    if (seeds.empty()) {
        printf("Could not find any initial seed candidates!\n");
        abort();
    }
    return seeds[0];
}

Voxels makeCube(int length) {
    Voxels result;
    result.width = length;
    result.height = length;
    for (int i = 0; i < length * length * length; ++i) {
        result.voxels.push_back(1);
    }
    return result;
}

double accessibilityHeuristic(const Voxels &v, Pos p, int j) {
    const double WEIGHT_FACTOR = 0.1;
    if (j == 0) {
        return numNeighbours(v, p);
    } else {
        auto res = accessibilityHeuristic(v, p, j - 1);
        auto weight = pow(WEIGHT_FACTOR, (double)j);
        for (auto d : directions()) {
            auto posInD = p.nextInDirection(d);
            if (!exists(v, posInD)) continue;
            res += weight * accessibilityHeuristic(v, posInD, j - 1);
        }
        return res;
    }
}

int main(int argc, char *argv[]) {
    auto cube = makeCube(3);
    cube.print();
    OrientedPos seed = findInitialSeed(cube, true);
    seed.pos.print("seed");
    printf("direction: %s\n", printDir(seed.dir));
    printf("accessibility: j = 0: %f\n", accessibilityHeuristic(cube, seed.pos, 0));
    printf("accessibility: j = 1: %f\n", accessibilityHeuristic(cube, seed.pos, 1));
    printf("accessibility: j = 2: %f\n", accessibilityHeuristic(cube, seed.pos, 2));
    printf("accessibility: j = 3: %f\n", accessibilityHeuristic(cube, seed.pos, 3));
    return 0;
}
