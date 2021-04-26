#include <vector>
#include <cstdio>

enum class Direction { XP, XN, YP, YN, ZP, ZN };

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
    void print(bool detailed = false) const;
};

int get(const Voxels &v, Pos p) {
    return v.voxels[p.x * v.width * v.height + p.y * v.width + p.z];
}

void Voxels::print(bool detailed) const {
    printf("Dimensions: %ix%ix%i\n", maxX(), maxY(), maxZ());
    if (detailed) {
        for (int x = 0; x < maxX(); ++x) {
            for (int y = 0; y < maxX(); ++y) {
                for (int z = 0; z < maxX(); ++z) {
                    printf("%i", get(*this, Pos(x, y, z)));
                }
                printf("\n");
            }
            printf("\n\n");
        }
    }
}

void set(Voxels &v, Pos p, int value) {
    v.voxels[p.x * v.width * v.height + p.y * v.width + p.z] = value;
}

bool exists(const Voxels &v, Pos p) {
    if (p.x < 0 || p.y < 0 || p.z < 0) return false;
    if (p.x >= v.maxX() || p.y >= v.maxY() || p.z >= v.maxZ()) return false;
    int val = get(v, p);
    return val != 0;
}

int numExteriorFaces(const Voxels &v, Pos p) {
    int num = 6; // maximum
    if (exists(v, p.nextInDirection(Direction::XP))) --num;
    if (exists(v, p.nextInDirection(Direction::XN))) --num;
    if (exists(v, p.nextInDirection(Direction::YP))) --num;
    if (exists(v, p.nextInDirection(Direction::YN))) --num;
    if (exists(v, p.nextInDirection(Direction::ZP))) --num;
    if (exists(v, p.nextInDirection(Direction::ZN))) --num;
    return num;
}

bool hasFreePassage(const Voxels &v, Pos p, Direction dir) {
    for (int i = 0; i < 50; ++i) {
        p = p.nextInDirection(dir);
        if (exists(v, p)) return false;
    }
    return true;
}

std::vector<Pos> initialSeedCandidates(const Voxels &v, bool debug = false) {
    std::vector<Pos> results;
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
                results.push_back(p);
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

Voxels makeCube(int length) {
    Voxels result;
    result.width = length;
    result.height = length;
    for (int i = 0; i < length * length * length; ++i) {
        result.voxels.push_back(1);
    }
    return result;
}

int main(int argc, char *argv[]) {
    auto cube = makeCube(3);
    cube.print();
    auto seedCandidates = initialSeedCandidates(cube, true);
    return 0;
}
