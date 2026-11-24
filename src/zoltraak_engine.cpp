#include "zoltraak_engine.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <zoltraak_initializers.h>
#include <zoltraak_types.h>

#include "VkBootstrap.h"

#include <chrono>
#include <thread>

namespace Zoltraak 
{
    const bool bUseValidationLayer = false;

    zoltraak_engine* loadedEngine = nullptr;

    zoltraak_engine& zoltraak_engine::Get() { return *loadedEngine; }

    void zoltraak_engine::init()
    {
        assert(loadedEngine == nullptr);
        loadedEngine = this;
        
        SDL_Init(SDL_INIT_VIDEO);
        
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
        
        window = SDL_CreateWindow(
            "Zoltraak Engine", 
            windowExtent.width, 
            windowExtent.height, 
            window_flags);
        
        init_vulkan();
        
        init_swapchain();

        init_commands();

        init_sync_structures();
        
        isInitialized = true;
    }

    void zoltraak_engine::cleanup()
    {
        if (isInitialized) {

            destroy_swapchain();

            vkDestroySurfaceKHR(Instance, Surface, nullptr);
            vkDestroyDevice(Device, nullptr);
		
            vkb::destroy_debug_utils_messenger(Instance, Debug_messenger);
            vkDestroyInstance(Instance, nullptr);
            SDL_DestroyWindow(window);
        }
    }

    void zoltraak_engine::draw()
    {
        // nothing yet
    }

    void zoltraak_engine::run()
    {
        SDL_Event e;
        bool bQuit = false;

        // main loop
        while (!bQuit) {
            // Handle events on queue
            while (SDL_PollEvent(&e) != 0) {
                // close the window when user alt-f4s or clicks the X button
                if (e.type == SDL_EVENT_QUIT)
                    bQuit = true;

                if (e.type == SDL_EVENT_WINDOW_MINIMIZED) {
                    stop_rendering = true;
                }
                if (e.type == SDL_EVENT_WINDOW_RESTORED) {
                    stop_rendering = false;
                }
            }

            // do not draw if we are minimized
            if (stop_rendering) {
                // throttle the speed to avoid the endless spinning
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            draw();
        }
    }
    
    void zoltraak_engine::init_vulkan()
    {
        vkb::InstanceBuilder builder;
        
        auto inst_ret = builder.set_app_name("Zoltraak Engine")
        .request_validation_layers(bUseValidationLayer)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();
        
        vkb::Instance vkb_inst = inst_ret.value();
        
        Instance = vkb_inst.instance;
        Debug_messenger = vkb_inst.debug_messenger;
        
        SDL_Vulkan_CreateSurface(window, Instance,nullptr, &Surface);
        
        //vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        features.dynamicRendering = true;
        features.synchronization2 = true;
        
        //vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;
        
        //use vkbootstrap to select a gpu. 
        //We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct features
        vkb::PhysicalDeviceSelector selector{ vkb_inst };
        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 3)
            .set_required_features_13(features)
            .set_required_features_12(features12)
            .set_surface(Surface)
            .select()
            .value();
    
        //create the final vulkan device
        vkb::DeviceBuilder deviceBuilder{ physicalDevice };

        vkb::Device vkbDevice = deviceBuilder.build().value();

        // Get the VkDevice handle used in the rest of a vulkan application
        Device = vkbDevice.device;
        ChosenGPU = physicalDevice.physical_device;
    }
    
    void zoltraak_engine::init_swapchain()
    {
        create_swapchain(windowExtent.width, windowExtent.height);
    }
    
    void zoltraak_engine::init_commands()
    {
        
    }
    
    void zoltraak_engine::init_sync_structures()
    {
        
    }
    
    void zoltraak_engine::create_swapchain(uint32_t width, uint32_t height)
    {
        vkb::SwapchainBuilder swapchainBuilder{ ChosenGPU,Device,Surface };

        SwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            //.use_default_format_selection()
            .set_desired_format(VkSurfaceFormatKHR{ .format = SwapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
            //use vsync present mode
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

        SwapchainExtent = vkbSwapchain.extent;
        //store swapchain and its related images
        Swapchain = vkbSwapchain.swapchain;
        SwapchainImages = vkbSwapchain.get_images().value();
        SwapchainImageViews = vkbSwapchain.get_image_views().value();
    }
    
    void zoltraak_engine::destroy_swapchain()
    {
        vkDestroySwapchainKHR(Device, Swapchain, nullptr);

        // destroy swapchain resources
        for (int i = 0; i < SwapchainImageViews.size(); i++) {

            vkDestroyImageView(Device, SwapchainImageViews[i], nullptr);
        }
    }
}
