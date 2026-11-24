#pragma once

#include "zoltraak_types.h"
struct SDL_Window;
namespace Zoltraak
{
    class zoltraak_engine
    {
    public:
        bool isInitialized{false};
        int frameNumber{0};
        bool stop_rendering{false};
        VkExtent2D windowExtent{1700,900};

        SDL_Window* window{nullptr};
        static zoltraak_engine& Get();
        
        VkInstance Instance;// Vulkan library handle
        VkDebugUtilsMessengerEXT Debug_messenger;// Vulkan debug output handle
        VkPhysicalDevice ChosenGPU;// GPU chosen as the default device
        VkDevice Device; // Vulkan device for commands
        VkSurfaceKHR Surface;// Vulkan window surface
        
        VkSwapchainKHR Swapchain;
        VkFormat SwapchainImageFormat;

        std::vector<VkImage> SwapchainImages;
        std::vector<VkImageView> SwapchainImageViews;
        VkExtent2D SwapchainExtent;
        
        //initializes everything in the engine
        void init();

        //shuts down the engine
        void cleanup();

        //draw loop
        void draw();

        //run main loop
        void run();
    private:
        void init_vulkan();
        void init_swapchain();
        void init_commands();
        void init_sync_structures();
        void create_swapchain(uint32_t width, uint32_t height);
        void destroy_swapchain();
    };
}
