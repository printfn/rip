#include "Direction.h"
#include "Pos.h"
#include "Voxels.h"
#include "utils.h"

#include <vector>
#include <deque>
#include <unordered_set>
#include <cstdio>
#include <iostream>

#include <GLFW/glfw3.h>

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
                Direction dir = Direction::YP;
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

        auto otherPosInPair = pos.nextInDirection(seed.dir.opposite());
        if (v.existsAt(pos) && v.existsAt(otherPosInPair)) {
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

std::vector<OrientedPair> inaccessiblePairs(const Voxels &v, OrientedPos seed) {
    std::vector<OrientedPair> candidates = breadthFirstPairSearch(v, seed);
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

int initGlfw() {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow *window = glfwCreateWindow(
        640, 480, "Recursive Interlocking Puzzles", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glClearColor(0.4f, 0.3f, 0.4f, 0.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 1;
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

int main(int argc, char *argv[]) {
    if (argc == 2) {
        std::cout << "Reading file " << argv[1] << "..." << std::endl;

    }
    auto voxels = initialiseVoxels(argc, argv);
    std::cout << voxels << std::endl;
    OrientedPos seed = findInitialSeed(voxels, true);
    std::cout << "seed: " << seed.pos << std::endl;
    std::cout << "direction: " << seed.dir << std::endl;
    printf("accessibility: j = 0: %f\n", voxels.accessibilityHeuristic(seed.pos, 0));
    printf("accessibility: j = 1: %f\n", voxels.accessibilityHeuristic(seed.pos, 1));
    printf("accessibility: j = 2: %f\n", voxels.accessibilityHeuristic(seed.pos, 2));
    printf("accessibility: j = 3: %f\n", voxels.accessibilityHeuristic(seed.pos, 3));
    auto pairs = inaccessiblePairs(voxels, seed);
    for (auto &pair : pairs) {
        std::cout << "blocking: " << pair.blocking << std::endl;
        std::cout << "blockee:  " << pair.blockee << std::endl;
    }
    return 0;
}
