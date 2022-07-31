// Include GLFW

#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>

#include "ResourceManager.hpp"
#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const unsigned int WIDTH = 1320;
const unsigned int HEIGHT = 760;

glm::vec3 at(0.0f, 0.0f, 2.0f);
glm::vec3 to(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

const float cameraSpeed = 0.01;
void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

static void cursorPositionCallback(GLFWwindow *window, double xpos,
                                   double ypos) {
	float x = xpos / WIDTH - 0.5;
	float y = -ypos / HEIGHT + 0.5;
	to += glm::vec3(x, y, 0);
	glfwSetCursorPos(window, WIDTH / 2.0f, HEIGHT / 2.0f);
}
static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	at += glm::vec3(0, 0, -cameraSpeed * yoffset);
	to += glm::vec3(0, 0, -cameraSpeed * 5 * yoffset);
}

void handleInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_W)) {
		at += glm::vec3(0.0f, cameraSpeed, 0.0f);
		to += glm::vec3(0.0f, cameraSpeed, 0.0f);

	} else if (glfwGetKey(window, GLFW_KEY_S)) {

		at -= glm::vec3(0.0f, cameraSpeed, 0.0f);
		to -= glm::vec3(0.0f, cameraSpeed, 0.0f);
	} else if (glfwGetKey(window, GLFW_KEY_A)) {

		at -= glm::vec3(cameraSpeed, 0.0f, 0.0f);
		to -= glm::vec3(cameraSpeed, 0.0f, 0.0f);
	} else if (glfwGetKey(window, GLFW_KEY_D)) {

		at += glm::vec3(cameraSpeed, 0.0f, 0.0f);
		to += glm::vec3(cameraSpeed, 0.0f, 0.0f);
	}
}

GLFWwindow *InitWindow() {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return nullptr;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
	               GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwSwapInterval(1);

	// Open a window and create its OpenGL context
	GLFWwindow *window =
	    glfwCreateWindow(WIDTH, HEIGHT, "opengl-Red triangle", NULL, NULL);
	if (window == NULL) {
		fprintf(
		    stderr,
		    "Failed to open GLFW window. If you have an Intel GPU, they are "
		    "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Unable to laod GLAD" << std::endl;
	}

	std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetScrollCallback(window, scrollCallback);
	return window;
}

int main(void) {
	GLFWwindow *window = InitWindow();
	if (!window)
		return -1;

	// std::vector<float> vertices{
	//     0.5f,  0.5f,  0.0f, // top right
	//     0.5f,  -0.5f, 0.0f, // bottom right
	//     -0.5f, -0.5f, 0.0f, // bottom left
	//     -0.5f, 0.5f,  0.0f  // top left
	// };
	//
	// std::vector<unsigned int> indices{0, 1, 2, 2, 3, 0};

	// code for the sphere

	int sectorCount = 36;
	int stackCount = 18;
	float radius = 0.5;
	float PI = M_PI;
	std::vector<float> vertices;
	float x, y, z, xy;                           // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
	float s, t;                                  // vertex texCoord

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i) {
		stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);      // r * cos(u)
		z = radius * sinf(stackAngle);       // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but
		// different tex coords
		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep; // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// normalized vertex normal (nx, ny, nz)

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			vertices.push_back(s);
			vertices.push_back(t);
		}
	}
	std::vector<unsigned int> indices;
	int k1, k2;
	for (int i = 0; i < stackCount; ++i) {
		k1 = i * (sectorCount + 1); // beginning of current stack
		k2 = k1 + sectorCount + 1;  // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	// -------------------
	VertexArray va;
	VertexBuffer vb(&vertices[0], vertices.size() * sizeof(float));
	IndexBuffer ib(&indices[0], indices.size());

	indices.clear();
	vertices.clear();

	VertexBufferLayout layout;
	layout.AddFloat(3);
	layout.AddFloat(2);

	va.AddBuffer(vb, layout);

	Shader shader =
	    ResourceManager::LoadShader("res/shaders/basic-vertex.glsl",
	                                "res/shaders/basic-fragment.glsl", "basic");

	Texture2D sun =
	    ResourceManager::LoadTexture("res/images/sun.jpg", false, "sun");
	Texture2D earth =
	    ResourceManager::LoadTexture("res/images/earth.jpg", false, "earth");
	Texture2D venus =
	    ResourceManager::LoadTexture("res/images/venus.jpg", false, "venus");

	glActiveTexture(GL_TEXTURE0);
	sun.Bind();

	glActiveTexture(GL_TEXTURE1);
	earth.Bind();

	glActiveTexture(GL_TEXTURE2);
	venus.Bind();

	Renderer renderer;
	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	float delta_time;
	float current_time, prev_time = glfwGetTime();
	float delta_sum = 0;
	while (glfwWindowShouldClose(window) == false) {
		handleInput(window);
		current_time = glfwGetTime();
		delta_time = current_time - prev_time;
		prev_time = current_time;
		delta_sum += delta_time;
		if (delta_sum > 1) {
			std::cout << 1 / delta_time << " FPS" << std::endl;
			delta_sum = 0;
		}

		renderer.Clear();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Use();
		glm::mat4 view(1.0f);
		glm::mat4 projection(1.0f);

		view = glm::lookAt(at, to, up);
		projection = glm::perspective(glm::radians(45.0f),
		                              WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		glm::mat4 model(1.0f);
		model = glm::rotate(model,
		                    (float)(glfwGetTime() * 0.1) * glm::radians(45.0f),
		                    glm::vec3(0.1f, 0.1f, 0.1f));
		float scalingFactor = 0.6f;
		model = glm::scale(model, scalingFactor * glm::vec3(1));
		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("projection", projection);
		shader.SetInt("image", 0);
		renderer.Draw(va, ib, shader, GL_TRIANGLES);

		scalingFactor = 0.3;
		model = glm::mat4(1.0f);
		model = glm::rotate(model,
		                    (float)(glfwGetTime() * 0.4) * glm::radians(45.0f),
		                    glm::vec3(0.0f, 0.2f, 0.0f));
		model = glm::translate(model, glm::vec3(0.9f, 0.0f, 0.0f));
		model = glm::rotate(model,
		                    (float)(glfwGetTime() * 0.3) * glm::radians(45.0f),
		                    glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::scale(model, scalingFactor * glm::vec3(1));
		shader.SetMat4("model", model);
		shader.SetInt("image", 1);
		renderer.Draw(va, ib, shader, GL_TRIANGLES);

		scalingFactor = 0.4;
		model = glm::mat4(1.0f);
		model = glm::rotate(model,
		                    (float)(glfwGetTime() * 0.7) * glm::radians(45.0f),
		                    glm::vec3(0.0f, 0.5f, 0.0f));
		model = glm::translate(model, glm::vec3(0.6f, 0.0f, 0.0f));
		model = glm::rotate(model,
		                    (float)(glfwGetTime() * 0.4) * glm::radians(45.0f),
		                    glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::scale(model, scalingFactor * glm::vec3(1));
		shader.SetMat4("model", model);
		shader.SetInt("image", 2);
		renderer.Draw(va, ib, shader, GL_TRIANGLES);
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
