#include "Direction.h"
#include "Pos.h"
#include "Voxels.h"
#include "UI.h"
#include "utils.h"

#include <algorithm>
#include <vector>
#include <deque>
#include <unordered_set>
#include <cstdio>
#include <iostream>

struct OrientedPos {
    Pos pos;
    Direction removalDir;
    Direction normalDir; // normal direction in which no cube exists

    OrientedPos(Pos p, Direction removalDir, Direction normalDir)
        : pos{p}, removalDir{removalDir}, normalDir{normalDir} {}
};

std::vector<OrientedPos> initialSeedCandidates(const Voxels &v, bool debug = false) {
    std::vector<OrientedPos> results;
    int skippedDueToWrongFaceCount = 0;
    int skippedDueToNonFreePassage = 0;
    const Direction removalDir = Direction::YP;
    for (int x = 0; x < v.maxX(); ++x) {
        for (int y = 0; y < v.maxX(); ++y) {
            for (int z = 0; z < v.maxX(); ++z) {
                auto p = Pos(x, y, z);
                if (!v.existsAt(p)) {
                    continue;
                }
                if (v.numExteriorFaces(p) != 2) {
                    ++skippedDueToWrongFaceCount;
                    continue;
                }
                if (!v.hasFreePassage(p, removalDir, false)) {
                    ++skippedDueToNonFreePassage;
                    continue;
                }
                Direction normalDir = removalDir;
                if (!v.existsAt(p.nextInDirection(Direction::XP))) normalDir = Direction::XP;
                if (!v.existsAt(p.nextInDirection(Direction::XN))) normalDir = Direction::XN;
                if (!v.existsAt(p.nextInDirection(Direction::YN))) normalDir = Direction::YN;
                if (!v.existsAt(p.nextInDirection(Direction::ZP))) normalDir = Direction::ZP;
                if (!v.existsAt(p.nextInDirection(Direction::ZN))) normalDir = Direction::ZN;
                if (normalDir == removalDir) continue;
                results.push_back(OrientedPos(p, removalDir, normalDir));
            }
        }
    }
    if (debug) {
        std::cout << "Found " << results.size() << " initial seed candidates" <<
            " (rejected " << skippedDueToWrongFaceCount <<
            "/" << skippedDueToNonFreePassage << ")" << std::endl;
    }
    return results;
}

OrientedPos findInitialSeed(const Voxels &v, bool debug = false) {
    auto seeds = initialSeedCandidates(v, debug);
    if (seeds.empty()) {
        std::cerr << "Could not find any initial seed candidates!" << std::endl;
        exit(1);
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

struct OrientedPair {
    Pos blocking, blockee;
};

std::vector<OrientedPair> breadthFirstPairSearch(const Voxels &v, OrientedPos seed, const std::vector<Pos> &anchors) {
    std::vector<OrientedPair> results;
    std::vector<Pos> done;
    std::deque<Pos> queue{seed.pos};
    while (!queue.empty() && results.size() < 50) {
        auto pos = queue.front();
        queue.pop_front();

        auto otherPosInPair = pos.nextInDirection(seed.normalDir.opposite());
        if (v.existsAt(pos) && v.existsAt(otherPosInPair) && !contains(anchors, otherPosInPair)) {
            OrientedPair result{pos, otherPosInPair};
            results.push_back(result);
        }

        done.push_back(pos);
        for (Direction dir : ALL_DIRECTIONS) {
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

std::vector<OrientedPair> inaccessiblePairs(const Voxels &v, OrientedPos seed, const std::vector<Pos> &anchors) {
    std::vector<OrientedPair> candidates = breadthFirstPairSearch(v, seed, anchors);
    std::sort(candidates.begin(), candidates.end(),
        [&v](const OrientedPair &p1, const OrientedPair &p2) {
            double a1 = v.accessibilityHeuristic(p1.blockee, 3);
            double a2 = v.accessibilityHeuristic(p2.blockee, 3);
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
    result[{0, 1, 0}] = 3;
    result[{0, 1, 1}] = 1;
    result[{0, 1, 2}] = 2;
    result[{0, 2, 0}] = 3;
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

Voxels initialiseVoxels(int argc, char *argv[]) {
    switch (argc) {
        case 1:
            std::cout << "Using default shape" << std::endl;
            return solvedThreeCube();
        case 2:
            std::cout << "Reading file " << argv[1] << "..." << std::endl;
            return Voxels::readFile(std::string{argv[1]});
        default:
            std::cout << "Usage: ./puzzles <shape file>" << std::endl;
            std::cout << "Using default shape" << std::endl;
            return solvedThreeCube();
    }
}

std::vector<std::vector<Pos>> findPaths(
    Pos from, Pos to, Pos disallowed, Direction disallowedDir, int maxLength,
    const std::vector<Pos> anchors, const Voxels &v
) {
    // return all shortest paths, not crossing 'disallowed' or any in disallowedDir
    if (from == to) return {{}};
    if (maxLength == 0) return {};
    std::vector<Pos> steps;
    for (Direction dir : ALL_DIRECTIONS) {
        Pos nextPos = from.nextInDirection(dir);
        if (v[nextPos] == 0) continue;
        if (nextPos.isInLine(disallowed, disallowedDir.opposite())) continue;
        if (contains(anchors, nextPos)) continue;
        if (nextPos == to) {
            return {{to}};
        }
        steps.push_back(nextPos);
    }
    std::vector<std::vector<Pos>> allPaths;
    for (Pos step : steps) {
        std::vector<std::vector<Pos>> paths = findPaths(
            step, to, disallowed, disallowedDir, maxLength - 1, anchors, v);
        for (const auto &path : paths) {
            std::vector<Pos> thisPath;
            thisPath.push_back(step);
            for (const auto &nextPos : path) {
                thisPath.push_back(nextPos);
            }
            allPaths.push_back(std::move(thisPath));
        }
    }
    return allPaths;
}

// Add any extra voxels in removalDir to path.
// Returns false if that isn't possible because we'd have to add an anchor voxel
bool addUpwardVoxels(
    std::vector<Pos> &path, Direction removalDir,
    const std::vector<Pos> anchors, const Voxels &v
) {
    std::vector<Pos> extraVoxels;
    for (const auto &p : path) {
        Pos next = p.nextInDirection(removalDir);
        while (v.isInRange(next)) {
            if (v.existsAt(next) && !contains(path, next) && !contains(extraVoxels, next)) {
                if (contains(anchors, next)) {
                    return false;
                }
                extraVoxels.push_back(next);
            }
            next = next.nextInDirection(removalDir);
        }
    }
    for (const auto &p : extraVoxels) {
        path.push_back(p);
    }
    return true;
}

std::vector<std::vector<Pos>> findShortestPaths(
    Pos from, const std::vector<OrientedPair> &blockingPairs, Direction disallowedDir,
    const std::vector<Pos> anchors, const Voxels &v
) {
    std::vector<std::vector<Pos>> shortestPaths;
    int shortestPathLength = 0;
    while (shortestPaths.size() == 0) {
        ++shortestPathLength;
        for (const OrientedPair &blockingPair : blockingPairs) {
            Pos to = blockingPair.blockee;
            Pos disallowed = blockingPair.blocking;
            std::vector<std::vector<Pos>> potShortestPaths = findPaths(from, to, disallowed, disallowedDir, shortestPathLength, anchors, v);
            for (auto &potentialPiece : potShortestPaths) {
                if (addUpwardVoxels(potentialPiece, disallowedDir, anchors, v)) {
                    // only accept this shortest path if it doesn't include anchors
                    shortestPaths.push_back(potentialPiece);
                }
            }
        }
    }
    std::cout << "Found " << shortestPaths.size() << " paths " <<
        "(length " << shortestPathLength << ")" << std::endl;
    return shortestPaths;
}

std::vector<Pos> findAnchors(const OrientedPos &seed, const Voxels &v) {
    std::vector<Pos> anchors;
    for (Direction dir : ALL_DIRECTIONS) {
        if (dir == seed.normalDir) continue;
        if (dir == seed.removalDir) continue;
        Pos next = seed.pos.nextInDirection(dir);
        Pos anchor = next;
        while (v.isInRange(next)) {
            if (v.existsAt(next)) {
                anchor = next;
            }
            next = next.nextInDirection(dir);
        }
        if (v.existsAt(anchor)) {
            anchors.push_back(anchor);
        }
    }
    return anchors;
}

void constructPiece(Voxels &voxels, int pieceNum) {
    std::cout << "Constructing piece " << pieceNum << std::endl;
    OrientedPos seed = findInitialSeed(voxels, true);
    std::vector<Pos> anchors = findAnchors(seed, voxels);
    std::cout << "seed: " << seed.pos <<
        ", removal direction: " << seed.removalDir <<
        ", normal direction: " << seed.normalDir <<
        ", accessibility: " << voxels.accessibilityHeuristic(seed.pos, 3) << std::endl;
    auto pairs = inaccessiblePairs(voxels, seed, anchors);
    std::cout << "Found " << pairs.size() << " blocking pairs" << std::endl;
    std::cout << "Minimum accessibility: " << voxels.accessibilityHeuristic(pairs.front().blockee, 3) << std::endl;
    std::cout << "Maximum accessibility: " << voxels.accessibilityHeuristic(pairs.back().blockee, 3) << std::endl;
    ++voxels[seed.pos];
    // each shortest path is a potential piece we might choose
    std::vector<std::vector<Pos>> shortestPaths = findShortestPaths(seed.pos, pairs, seed.removalDir, anchors, voxels);
    std::sort(shortestPaths.begin(), shortestPaths.end(),
        [](const auto &p1, const auto &p2) {
            return p1.size() < p2.size();
        });
    for (const auto &pos : shortestPaths[0]) {
        ++voxels[pos];
    }
}

int main(int argc, char *argv[]) {
    auto voxels = initialiseVoxels(argc, argv);
    std::cout << voxels << std::endl;
    constructPiece(voxels, 1);
    initGlfw(voxels);
    return 0;
}
