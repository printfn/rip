#include <vector>
#include <cstdio>

struct Voxels {
    int width = 0;
    int height = 0;
    std::vector<int> voxels;
};

int get(const Voxels &v, int x, int y, int z) {
    return v.voxels[x * v.width * v.height + y * v.width + z];
}

void set(Voxels &v, int x, int y, int z, int value) {
    v.voxels[x * v.width * v.height + y * v.width + z] = value;
}

void print(const Voxels &v) {
    printf("Dimensions: %lux%ix%i\n", v.voxels.size() / v.width / v.height, v.height, v.width);
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
    auto cube = makeCube(5);
    print(cube);
    return 0;
}
