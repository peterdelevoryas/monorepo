#include <cstdio>
#include <cassert>
#include <unistd.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#define let const auto
#define var auto
#define loop for (;;)

int main(int argc, char** argv) {
    glfwInit();
    let window = glfwCreateWindow(1000, 1000, "hello world", nullptr, nullptr);
    assert(window);

    VkInstance instance;
    let instance_info = VkInstanceCreateInfo{};
    let r = vkCreateInstance(&instance_info, nullptr, &instance);
    (void)r;
    vkDestroyInstance(instance, nullptr);

    loop {
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
            break;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
