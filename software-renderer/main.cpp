#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <utility>
#include <string>
#include <string_view>

template<typename T>
struct Vec {
    T *ptr;
    uint32_t len, cap;

    void push(T &&v) {
        if (len >= cap) {
            cap = cap ? cap * 3 / 2 : 8;
            ptr = realloc(ptr, sizeof(T) * cap);
            assert(ptr);
        }
        ptr[len++] = std::move(v);
    }

    const T &operator[](uint32_t i) const {
        assert(i < len);
        return ptr[i];
    }

    T &operator[](uint32_t i) {
        assert(i < len);
        return ptr[i];
    }

    const T *begin() const {
        return ptr;
    }

    const T *end() const {
        return ptr + len;
    }

    T *begin() {
        return ptr;
    }

    T *end() {
        return ptr + len;
    }
};

struct Obj {
    struct Vertex {
        float x, y, z;
    };
    struct Face {
        uint16_t vertex_index[3];
        uint16_t texcoord_index[3];
        uint16_t normal_index[3];
    };
    struct Group {
        std::string_view name;
        Vec<Face> faces;
    };
    std::string string_pool;
    Vec<Group> groups;

    struct Parser {
        FILE *file;
        Obj &obj;
        uint32_t line_no;

        void parse_file() {

        }
    };

    static Obj read_file(const char *path) {
        auto file = fopen(path, "r");
        if (!file) {
            perror("Unable to open obj file");
            return {};
        }
        auto obj = Obj{};
        auto parser = Parser{file, obj};
        parser.parse_file();
        fclose(file);
        return obj;
    }
};

struct Image {
    struct Pixel {
        uint8_t b, g, r;
    };
    Pixel *data;
    int width, height;

    static Image init(int width, int height) {
        auto data = static_cast<Pixel*>(calloc(sizeof(Pixel), width * height));
        return {data, width, height};
    }

    void free() {
        ::free(data);
    }

    void write_tga_file(const char *path) {
        uint8_t header[18] = {};
        // Uncompressed true color image.
        header[2] = 2;
        // Width, little-endian byte-order.
        header[12] = width & 0xFF;
        header[13] = (width & 0xFF00) >> 8;
        // Height, little-endian byte-order.
        header[14] = height & 0xFF;
        header[15] = (height & 0xFF00) >> 8;
        // Bits-per-pixel.
        header[16] = sizeof(Pixel) * 8;

        auto file = fopen(path, "w");
        fwrite(header, sizeof(header), 1, file);
        fwrite(data, sizeof(Pixel), width * height, file);
        fclose(file);
    }

    void draw_line(int x0, int y0, int x1, int y1) {
        bool steep = false;
        if (std::abs(y1 - y0) > std::abs(x1 - x0)) {
            std::swap(x0, y0);
            std::swap(x1, y1);
            steep = true;
        }
        if (x1 < x0) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        int dx = x1 - x0;
        int dy = y1 - y0;
        int de = std::abs(dy);
        int e = 0;
        int y = y0;
        int y_step = y0 < y1 ? 1 : -1;
        for (int x = x0; x <= x1; x++) {
            // printf("(%d, %d) e=%d\n", x, y, e);
            if (steep) {
                data[x * width + y] = {0, 0, 0xFF};
            } else {
                data[y * width + x] = {0, 0, 0xFF};
            }
            e += de;
            if (2 * e >= dx) {
                y += y_step;
                e -= dx;
            }
        }
    }
};

int main() {
    auto image = Image::init(800, 600);

    for (int x = 0; x < 800; x += 100) {
        image.draw_line(x, 0, x, 599);
    }
    for (int y = 0; y < 600; y += 100) {
        image.draw_line(0, y, 799, y);
    }
    auto head = Obj::read_file("head.obj");
    (void)head;

    image.write_tga_file("out.tga");
    image.free();
}
