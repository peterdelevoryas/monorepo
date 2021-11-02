#include <cstdio>
#include <vulkan/vulkan.h>
#include "metal.hh"

#define let const auto
#define var auto

int main(int argc, char** argv) {
    metal_create_window();

    VkInstance instance;
    let instance_info = VkInstanceCreateInfo{};
    let r = vkCreateInstance(&instance_info, nullptr, &instance);
    (void)r;
    vkDestroyInstance(instance, nullptr);
}
