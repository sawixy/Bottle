#include "render.hpp"
#include <stdexcept>
#include <iostream>

void Render::pickPhysicalDevice(){
    std::cout << "Picking Physical device" << std::endl;

    // Enumerate all Physical Devices
    uint32_t devicesCount = 0;
    if (vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr) != VK_SUCCESS){
        throw std::runtime_error("Failed to enumerate phsyical devices");
    }
    std::vector<VkPhysicalDevice> physicalDevices{devicesCount};
    if (vkEnumeratePhysicalDevices(instance, &devicesCount, physicalDevices.data()) != VK_SUCCESS){
        throw std::runtime_error("Failed to enumerate phsyical devices");
    }

    // Selecting Physical Device
    VkPhysicalDeviceFeatures   features;
    VkPhysicalDeviceProperties properties;

    std::cout << "\tDevices: " << std::endl;
    for (VkPhysicalDevice currentPhysicalDevice : physicalDevices){
        vkGetPhysicalDeviceProperties(currentPhysicalDevice, &properties);
        vkGetPhysicalDeviceFeatures(currentPhysicalDevice, &features);

        std::cout << "\t\t" << "Device Name: " << properties.deviceName << std::endl;
        std::cout << "\t\t" << "Api Version: " << properties.apiVersion << std::endl;
        std::cout << "\t\t" << "Driver Version: " << properties.driverVersion << std::endl;

        if (!physicalDevice && features.geometryShader == VK_TRUE){
            physicalDevice = currentPhysicalDevice;
            std::cout << "\tSelected " << properties.deviceName << std::endl;
        }
    }

    // Check surface support
    VkBool32 supported = VK_FALSE;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueueFamilyIndex, surface, &supported) || supported == VK_FALSE){
        throw std::runtime_error("Physical does not support surface");
    }

    std::cout << "\tSurface supported by physical device" << std::endl;

    std::cout << "Physical Device selected" << std::endl << std::endl;
}

void Render::createLogicalDevice(){
    std::cout << "Creating logical device" << std::endl;

    // Get all queue families properties
    uint32_t queueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties{queueFamilyPropertiesCount};
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

    // Get Present and Graphics Queue Family Index
    for (int i = 0; i < queueFamilyPropertiesCount; i++){
        VkQueueFamilyProperties CurrentQueueFamilyProperties = queueFamilyProperties[i];
        if (CurrentQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            graphicsQueueFamilyIndex = i;
        }
        if (CurrentQueueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT){
            presentQueueFamilyIndex = i;
        }
    }

    // Device Extensions
    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Creating Device Queue

    // Graphics Queue
    float graphicsQueuePriority = 1.0;
    VkDeviceQueueCreateInfo deviceGraphicsQueueCreateInfo{};
    deviceGraphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceGraphicsQueueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
    deviceGraphicsQueueCreateInfo.queueCount = 1;
    deviceGraphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    
    // Present Queue
    float presentQueuePriority = 0.5;
    VkDeviceQueueCreateInfo devicePresentQueueCreateInfo{};
    devicePresentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    devicePresentQueueCreateInfo.pQueuePriorities = &presentQueuePriority;
    devicePresentQueueCreateInfo.queueCount = 1;
    devicePresentQueueCreateInfo.queueFamilyIndex = presentQueueFamilyIndex;

    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos{
        deviceGraphicsQueueCreateInfo,
        devicePresentQueueCreateInfo
    };

    // Creating Logical Device
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = deviceQueueCreateInfos.size();
    deviceCreateInfo.enabledLayerCount = validationLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS){
        throw std::runtime_error("Failed to create logical device");
    }

    std::cout << "\tQueues: " << queueFamilyProperties.size() << std::endl;

    std::cout << "\tPresent Queue Family Index: " << presentQueueFamilyIndex << std::endl;
    std::cout << "\tGraphics Queue Family Index: " << graphicsQueueFamilyIndex << std::endl;

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);

    if (graphicsQueue == VK_NULL_HANDLE || presentQueue == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to get device queues");
    }

    std::cout << "Logical Device created successfully" << std::endl << std::endl;
}