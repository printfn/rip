#include "Direction.h"
#include "Pos.h"
#include "Voxels.h"

#include <vector>
#include <deque>
#include <unordered_set>
#include <cstdio>

void fail(const char *message) {
    fprintf(stderr, "%s\n", message);
    abort();
}

int numExteriorFaces(const Voxels &v, Pos p) {
    return 6 - v.numNeighboursAt(p);
}

bool hasFreePassage(const Voxels &v, Pos p, Direction dir) {
    for (int i = 0; i < 50; ++i) {
        p = p.nextInDirection(dir);
        if (v.existsAt(p)) return false;
    }
    return true;
}

struct OrientedPos {
    Pos pos;
    Direction dir; // direction in which no cube exists

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
                if (!v.existsAt(p)) {
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
                if (!v.existsAt(p.nextInDirection(Direction::XP))) dir = Direction::XP;
                if (!v.existsAt(p.nextInDirection(Direction::XN))) dir = Direction::XN;
                if (!v.existsAt(p.nextInDirection(Direction::YN))) dir = Direction::YN;
                if (!v.existsAt(p.nextInDirection(Direction::ZP))) dir = Direction::ZP;
                if (!v.existsAt(p.nextInDirection(Direction::ZN))) dir = Direction::ZN;
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
        fail("Could not find any initial seed candidates!");
    }
    return seeds[0];
}

Voxels makeCube(int length) {
    Voxels result{length, length, length};
    for (int x = 0; x < length; ++x) {
        for (int y = 0; y < length; ++y) {
            for (int z = 0; z < length; ++z) {
                result[Pos{x, y, z}] = 1;
            }
        }
    }
    return result;
}

double accessibilityHeuristic(const Voxels &v, Pos p, int j) {
    if (j < 0) {
        fail("j must not be less than zero");
    }
    const double WEIGHT_FACTOR = 0.1;
    if (j == 0) {
        return v.numNeighboursAt(p);
    } else {
        auto res = accessibilityHeuristic(v, p, j - 1);
        auto weight = pow(WEIGHT_FACTOR, (double)j);
        for (auto d : directions()) {
            auto posInD = p.nextInDirection(d);
            if (!v.existsAt(posInD)) continue;
            res += weight * accessibilityHeuristic(v, posInD, j - 1);
        }
        return res;
    }
}

struct OrientedPair {
    Pos blocking, blockee;
};

std::vector<OrientedPair> breadthFirstPairSearch(const Voxels &v, OrientedPos seed) {
    std::vector<OrientedPair> results;
    std::vector<Pos> done;
    std::deque<Pos> queue{seed.pos};
    while (!queue.empty() && results.size() < 50) {
        auto pos = queue.front();
        queue.pop_front();

        auto otherPosInPair = pos.nextInDirection(oppositeDirection(seed.dir));
        if (v.existsAt(pos) && v.existsAt(otherPosInPair)) {
            OrientedPair result{pos, otherPosInPair};
            results.push_back(result);
        }

        done.push_back(pos);
        for (Direction dir : directions()) {
            auto nextPos = pos.nextInDirection(dir);
            if (nextPos == seed.pos) continue;
            if (!v.existsAt(nextPos)) continue;
            if (std::find(done.begin(), done.end(), nextPos) != done.end()) continue;
            if (std::find(queue.begin(), queue.end(), nextPos) != queue.end()) continue;
            queue.push_back(nextPos);
        }
    }
    return results;
}

std::vector<OrientedPair> inaccessiblePairs(const Voxels &v, OrientedPos seed) {
    std::vector<OrientedPair> candidates = breadthFirstPairSearch(v, seed);
    std::sort(candidates.begin(), candidates.end(),
        [&v](const OrientedPair &p1, const OrientedPair &p2) {
            double a1 = accessibilityHeuristic(v, p1.blockee, 3);
            double a2 = accessibilityHeuristic(v, p2.blockee, 3);
            return a1 < a2;
        });
    while (candidates.size() > 10) {
        candidates.pop_back();
    }
    return candidates;
}

Voxels solvedThreeCube() {
    Voxels result{3, 3, 3};

    result[{0, 0, 0}] = 1;
    result[{0, 0, 1}] = 1;
    result[{0, 0, 2}] = 1;
    result[{0, 1, 0}] = 2;
    result[{0, 1, 1}] = 1;
    result[{0, 1, 2}] = 2;
    result[{0, 2, 0}] = 2;
    result[{0, 2, 1}] = 1;
    result[{0, 2, 2}] = 2;

    result[{1, 0, 0}] = 2;
    result[{1, 0, 1}] = 4;
    result[{1, 0, 2}] = 4;
    result[{1, 1, 0}] = 2;
    result[{1, 1, 1}] = 2;
    result[{1, 1, 2}] = 2;
    result[{1, 2, 0}] = 3;
    result[{1, 2, 1}] = 1;
    result[{1, 2, 2}] = 1;

    result[{2, 0, 0}] = 2;
    result[{2, 0, 1}] = 3;
    result[{2, 0, 2}] = 4;
    result[{2, 1, 0}] = 3;
    result[{2, 1, 1}] = 3;
    result[{2, 1, 2}] = 4;
    result[{2, 2, 0}] = 3;
    result[{2, 2, 1}] = 4;
    result[{2, 2, 2}] = 4;
    return result;
}

int main(int argc, char *argv[]) {
    auto cube = solvedThreeCube();
    cube.print(true);
    OrientedPos seed = findInitialSeed(cube, true);
    seed.pos.print("seed");
    printf("direction: %s\n", printDir(seed.dir));
    printf("accessibility: j = 0: %f\n", accessibilityHeuristic(cube, seed.pos, 0));
    printf("accessibility: j = 1: %f\n", accessibilityHeuristic(cube, seed.pos, 1));
    printf("accessibility: j = 2: %f\n", accessibilityHeuristic(cube, seed.pos, 2));
    printf("accessibility: j = 3: %f\n", accessibilityHeuristic(cube, seed.pos, 3));
    auto pairs = inaccessiblePairs(cube, seed);
    for (auto &pair : pairs) {
        pair.blocking.print("blocking");
        pair.blockee.print("blockee");
    }
    return 0;
}
