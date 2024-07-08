#pragma once

#include <iostream>
#include <optional>
#include <vector>
#include <array>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glfw/glfw3.h>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete();
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 colour;

	// Describes how to read vertices from VBO
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	};

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		// Two attributes (position and colour) --> two descriptions
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // vec2
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, colour);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	float time;
};

struct KeyControls {
	bool forwards = false;
	bool backwards = false;
	bool left = false;
	bool right = false;
	bool down = false;
	bool up = false;
};

struct Camera {
	glm::vec3 eye;
	glm::vec3 centre;
	glm::vec3 direction;
	glm::vec3 up;

	KeyControls controls{};

	float theta;
	float phi;

	float sensitivity = 0.001;
	float speed = 0.001;

	void updateEye() {
		glm::vec3 forwards = direction;
		forwards.z = 0;
		forwards = glm::normalize(forwards);

		glm::vec3 right = glm::cross(forwards, up);

		if (controls.forwards) {
			eye += forwards * speed;
		}

		if (controls.backwards) {
			eye -= forwards * speed;
		}

		if (controls.right) {
			eye += right * speed;
		}

		if (controls.left) {
			eye -= right * speed;
		}

		if (controls.down) {
			eye.z -= speed;
		}

		if (controls.up) {
			eye.z += speed;
		}
	}

	void updateCentre() {
		if (phi < 0.01) {
			phi = 0.01;
		}
		else if (phi > glm::pi<float>() - 0.01) {
			phi = glm::pi<float>() - 0.01;
		}

		// Calculate view direction
		direction.x = glm::sin(phi) * glm::cos(theta);
		direction.y = glm::sin(phi) * glm::sin(theta);
		direction.z = glm::cos(phi);

		centre = eye + direction;
	}

	// CHECK... this seems a bit sketchy
	void initControls(GLFWwindow* window, void (*keyCallback)(GLFWwindow*, int, int, int, int), void (*cursorPosCallback)(GLFWwindow*, double, double)) {
		// Keys
		glfwSetKeyCallback(window, keyCallback);

		// Lock cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		glfwSetCursorPos(window, 0, 0);
		glfwSetCursorPosCallback(window, cursorPosCallback);
	}
};
