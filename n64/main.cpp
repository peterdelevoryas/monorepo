#include <cstdio>
#include <vulkan/vulkan.h>

#define let const auto
#define var auto

int main(int argc, char** argv) {
  VkInstance instance;
  VkResult r;

  let instance_info = VkInstanceCreateInfo {};
  r = vkCreateInstance(&instance_info, nullptr, &instance);

  vkDestroyInstance(instance, nullptr);
}
