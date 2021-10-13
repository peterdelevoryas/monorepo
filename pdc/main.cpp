#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string_view>

#define panic(...) \
    do { \
        printf("%s: ", __func__); \
        printf(__VA_ARGS__); \
        if (errno) { \
            printf(": %s", strerror(errno)); \
        } \
        printf("\n"); \
        abort(); \
    } while (0)

static int file_size(int fd) {
    struct stat buf = {};
    fstat(fd, &buf);
    return buf.st_size;
}

struct MemoryMappedFile {
    uint8_t* data;
    int size;

    static MemoryMappedFile init(std::string_view path) {
        auto fd = open(path.data(), O_RDONLY | O_CLOEXEC);
        if (fd == -1) {
            panic("open '%s'", path.data());
        }
        auto size = file_size(fd);
        auto addr = mmap(nullptr, size, PROT_READ, MAP_FILE, fd, 0);
        if (!addr) {
            panic("mmap '%s' size=%d\n", path.data(), size);
        }
        auto data = static_cast<uint8_t*>(addr);
        close(fd);
        return {data, size};
    }

    ~MemoryMappedFile() {
        munmap(data, size);
    }
};

static void compile(std::string_view path) {
    auto file = MemoryMappedFile::init(path);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: %s file\n", argv[0]);
        return 1;
    }
    compile(argv[1]);
}
