#include <cstdio>
#include <unistd.h>
#include <vulkan/vulkan.h>
#include "metal.hh"

#define let const auto
#define var auto

int main(int argc, char** argv) {
    osx_create_window();

    VkInstance instance;
    let instance_info = VkInstanceCreateInfo{};
    let r = vkCreateInstance(&instance_info, nullptr, &instance);
    (void)r;
    vkDestroyInstance(instance, nullptr);

    for (int i = 0;; i++) {
        printf("%d ", i);
        fflush(stdout);

        osx_drain_events();
        usleep(33'000);
    }
}
