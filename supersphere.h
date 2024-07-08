#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <fstream>

#include <cstdint>
#include <limits>
#include <algorithm>

#include "struct.h"
#include "debug.h"

class SuperSphere {
public:
	void run();
	uint64_t frameCount = 0;

private:
	GLFWwindow* window;
	VkInstance instance;

	VkDebugUtilsMessengerEXT debugMessenger;

	// Devices
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	// Queues
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	// Window + presentation
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	bool framebufferResized = false;

	// Render setup
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	// Command pools and scheduling
	VkCommandPool commandPool;
	VkCommandPool transferCommandPool;

	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	// Unified vertex-and-index buffer
	size_t detail = 180;
	
	float radius = 2.0f;

	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	VkBuffer unifiedBuffer;
	VkDeviceMemory unifiedBufferMemory;

	// Camera
	Camera camera{};

	// Uniform buffers
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	// Misc
	uint32_t currentFrame = 0;

	// Setup + basic functions
	void initWindow();
	void createVertices();
	void createIndices();
	void createInstance();
	void initVulkan();
	void mainLoop();
	void cleanup();

	uint16_t IX(int i, int j);

	// Window + presentation
	void createSurface();
	void createSwapChain();
	void createImageViews();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	// Devices and queues
	void pickPhysicalDevice();
	void createLogicalDevice();

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	// Validation + debug
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			std::cerr << "Validation layer: " << pCallbackData->pMessage << "\n" << std::endl;
		}

		return VK_FALSE; // Normally used to test validation layers themselves otherwise
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();

	std::vector<const char*> getRequiredExtensions();

	bool checkValidationLayerSupport();

	// Render setup
	void createGraphicsPipeline();
	void createRenderPass();
	void createFramebuffers();
	void cleanupSwapChain();
	void recreateSwapChain();

	VkShaderModule createShaderModule(const std::vector<char>& code);

	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary); // ate: read from end, binary: read as binary (no text transformations)

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	// Command pools and scheduling
	void createCommandPools();
	void createCommandBuffers();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void createSyncObjects();

	// Unified vertex-and-index buffer
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size);
	void createVertexBuffer(VkDeviceMemory& stagingBufferMemory); // Rename?
	void createIndexBuffer(VkDeviceMemory& stagingBufferMemory);
	void createUnifiedBuffer();

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	// Camera
	void createCamera();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto app = reinterpret_cast<SuperSphere*>(glfwGetWindowUserPointer(window));
		Camera* camera = &app->camera;

		bool keyAction = action == GLFW_PRESS || action == GLFW_REPEAT;

		// Key controls
		switch (key) {
		case GLFW_KEY_W:
			camera->controls.forwards = action;
			break;

		case GLFW_KEY_S:
			camera->controls.backwards = action;
			break;

		case GLFW_KEY_A:
			camera->controls.left = action;
			break;

		case GLFW_KEY_D:
			camera->controls.right = action;
			break;

		case GLFW_KEY_LEFT_SHIFT:
			camera->controls.down = action;
			break;

		case GLFW_KEY_SPACE:
			camera->controls.up = action;
			break;
		}

		// Making closure easier
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	static void cursorPosCallback(GLFWwindow* window, double x, double y) {
		auto app = reinterpret_cast<SuperSphere*>(glfwGetWindowUserPointer(window));

		app->camera.theta -= (float)x * app->camera.sensitivity;
		app->camera.phi += (float)y * app->camera.sensitivity;

		glfwSetCursorPos(window, 0, 0);
	};

	// Uniform buffers and descriptors
	void createDescriptorSetLayout();
	void createUniformBuffers();
	void updateUniformBuffer(uint32_t currentImage);
	void createDescriptorPool();
	void createDescriptorSets();

	// Main rendering and event handling
	void drawFrame();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<SuperSphere*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
};
