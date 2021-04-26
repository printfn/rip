#include <vector>
#include <cstdio>

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
};

int get(const Voxels &v, int x, int y, int z) {
    return v.voxels[x * v.width * v.height + y * v.width + z];
}

void set(Voxels &v, int x, int y, int z, int value) {
    v.voxels[x * v.width * v.height + y * v.width + z] = value;
}

void print(const Voxels &v, bool detailed = false) {
    printf("Dimensions: %ix%ix%i\n", v.maxX(), v.maxY(), v.maxZ());
    if (detailed) {
        for (int x = 0; x < v.maxX(); ++x) {
            for (int y = 0; y < v.maxX(); ++y) {
                for (int z = 0; z < v.maxX(); ++z) {
                    printf("%i", get(v, x, y, z));
                }
                printf("\n");
            }
            printf("\n\n");
        }
    }
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
