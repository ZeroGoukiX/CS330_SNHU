#include "ShapeGenerator.h"
#include "ShapeData.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "shader.h"
#include "camera.h"
#include "cylinder.h"

#include <iostream>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(1.0f, 6.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool perspective = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// offset variables for plane, sphere
const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 9;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

//// plane
//GLuint planeNumIndices;
//GLuint planeVertexArrayObjectID;
//GLuint planeIndexByteOffset;

// sphere
GLuint sphereNumIndices;
GLuint sphereVertexArrayObjectID;
GLuint sphereIndexByteOffset;


// projection matrix
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------

	Shader lightingShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs");
	Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");


	float vertices[] = {
		// positions        // texture coords
		
		//cube verts //first verts are also for plane
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,


		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,

//***************************************************
// 3d rectangle verts for psx
		//front
		-0.4f, -0.05f,  0.25f,  0.0f,  0.0f,
		 0.4f, -0.05f,  0.25f,  1.0f,  0.0f,
		 0.4f,  0.05f,  0.25f,  1.0f,  1.0f,
		 0.4f,  0.05f,  0.25f,  1.0f,  1.0f,
		-0.4f,  0.05f,  0.25f,  0.0f,  1.0f,
		-0.4f, -0.05f,  0.25f,  0.0f,  0.0f,

		//back 
		-0.4f, -0.05f, -0.25f,  0.0f,  0.0f,
		 0.4f, -0.05f, -0.25f,  1.0f,  0.0f,
		 0.4f,  0.05f, -0.25f,  1.0f,  1.0f,
		 0.4f,  0.05f, -0.25f,  1.0f,  1.0f,
		-0.4f,  0.05f, -0.25f,  0.0f,  1.0f,
		-0.4f, -0.05f, -0.25f,  0.0f,  0.0f,

		//left
		-0.4f,  0.05f,  0.25f,  1.0f,  0.0f,
		-0.4f,  0.05f, -0.25f,  1.0f,  1.0f,
		-0.4f, -0.05f, -0.25f,  0.0f,  1.0f,
		-0.4f, -0.05f, -0.25f,  0.0f,  1.0f,
		-0.4f, -0.05f,  0.25f,  0.0f,  0.0f,
		-0.4f,  0.05f,  0.25f,  1.0f,  0.0f,

		//right
		 0.4f,  0.05f,  0.25f,  1.0f,  0.0f,
		 0.4f,  0.05f, -0.25f,  1.0f,  1.0f,
		 0.4f, -0.05f, -0.25f,  0.0f,  1.0f,
		 0.4f, -0.05f, -0.25f,  0.0f,  1.0f,
		 0.4f, -0.05f,  0.25f,  0.0f,  0.0f,
		 0.4f,  0.05f,  0.25f,  1.0f,  0.0f,

		 //top
	    -0.4f,  0.05f, -0.25f,  0.0f,  1.0f,
		 0.4f,  0.05f, -0.25f,  1.0f,  1.0f,
		 0.4f,  0.05f,  0.25f,  1.0f,  0.0f,
		 0.4f,  0.05f,  0.25f,  1.0f,  0.0f,
	    -0.4f,  0.05f,  0.25f,  0.0f,  0.0f,
	    -0.4f,  0.05f, -0.25f,  0.0f,  1.0f,

	    //bottom
	    -0.4f,  -0.05f, -0.25f,  0.0f,  1.0f,
		 0.4f,  -0.05f, -0.25f,  1.0f,  1.0f,
		 0.4f,  -0.05f,  0.25f,  1.0f,  0.0f,
		 0.4f,  -0.05f,  0.25f,  1.0f,  0.0f,
	    -0.4f,  -0.05f,  0.25f,  0.0f,  0.0f,
	    -0.4f,  -0.05f, -0.25f,  0.0f,  1.0f

//************************************************
// 

		//controller handles (pyramid for test) 
		//back           // texture coordinates
		   -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.5f, 1.0f,

			//front
		   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.5f, 1.0f,

			//left
		   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		   -0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.5f, 1.0f,
		   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.5f, 1.0f,

			// // right
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.5f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.5f, 1.0f,


			//bottom
		   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		   -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		   -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,



	};

	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 0.5f, 1.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	// VAOs and VBOs

	unsigned int cubeVBO, cubeVAO; //for light cubes
	unsigned int VBOPlane, VAOPlane; // plane
	unsigned int VBOPSX, VAOPSX; // psx
	unsigned int VBOBody, VAOBody; // controller body
	unsigned int VBOLeft, VAOLeft; // controller left handle
	unsigned int VBORight, VAORight; // controller right handle
	unsigned int VBOCD, VAOCD; //CD (cylinder)


	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	// position attribute for cube
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attribute for cube
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute for cube
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// position attribute for cube
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attribute for cube
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute for cube
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// lightCube's VAO and VBO
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// plane
	glGenVertexArrays(1, &VAOPlane);
	glGenBuffers(1, &VBOPlane);
	glBindVertexArray(VAOPlane);
	glBindBuffer(GL_ARRAY_BUFFER, VBOPlane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute for cube
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	//CD (cylinder)
	glGenVertexArrays(1, &VAOCD);
	glGenBuffers(1, &VBOCD);
	glBindVertexArray(VAOCD);
	glBindBuffer(GL_ARRAY_BUFFER, VBOCD);

	// playstation (psx)
	glGenVertexArrays(1, &VAOPSX);
	glGenBuffers(1, &VBOPSX);
	glBindVertexArray(VAOPSX);
	glBindBuffer(GL_ARRAY_BUFFER, VBOPSX);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute for cube
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// controller body
	glGenVertexArrays(1, &VAOBody);
	glGenBuffers(1, &VBOBody);
	glBindVertexArray(VAOBody);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBody);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute for cube
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	

	// controller left handle
	glGenVertexArrays(1, &VAOLeft);
	glGenBuffers(1, &VBOLeft);
	glBindVertexArray(VAOLeft);
	glBindBuffer(GL_ARRAY_BUFFER, VBOLeft);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//normal attribute for cube
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// controller right handle
	glGenVertexArrays(1, &VAORight);
	glGenBuffers(1, &VBORight);
	glBindVertexArray(VAORight);
	glBindBuffer(GL_ARRAY_BUFFER, VBORight);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute for cube
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load and create textures 
	unsigned int texture1, texture2, texture3, texture4, texture5, texture6;

		//texture1 plane
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis
		unsigned char* data = stbi_load("woodtexture.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Texture failed to load " << std::endl;
		}
		stbi_image_free(data);

		//texture2 bowling ball
		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	    data = stbi_load("blueswirl.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Texture failed to load " << std::endl;
		}
		stbi_image_free(data);

		//texture3 playstation
		glGenTextures(1, &texture3);
		glBindTexture(GL_TEXTURE_2D, texture3);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		data = stbi_load("playstationpsx.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Texture failed to load " << std::endl;
		}
		stbi_image_free(data);


		//texture4 controller body
		glGenTextures(1, &texture4);
		glBindTexture(GL_TEXTURE_2D, texture4);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		data = stbi_load("controllerpsx.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Texture failed to load " << std::endl;
		}
		stbi_image_free(data);

		//texture5 controller left and right handle
		glGenTextures(1, &texture5);
		glBindTexture(GL_TEXTURE_2D, texture5);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		data = stbi_load("gray.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Texture failed to load " << std::endl;
		}
		stbi_image_free(data);

		//texture6 CD
		glGenTextures(1, &texture6);
		glBindTexture(GL_TEXTURE_2D, texture6);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		data = stbi_load("blackgloss.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Texture failed to load " << std::endl;
		}
		stbi_image_free(data);


	// creates sphere object for bowling ball
	ShapeData sphere = ShapeGenerator::makeSphere();
	GLsizeiptr currentOffset = 0;

	unsigned int sphereVBO{}, sphereVAO;
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sphere.vertexBufferSize() + sphere.indexBufferSize(), 0, GL_STATIC_DRAW);
	currentOffset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBufferSize(), sphere.vertices);
	currentOffset += sphere.vertexBufferSize();
	sphereIndexByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBufferSize(), sphere.indices);
	sphereNumIndices = sphere.numIndices;
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereVBO);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------

	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	lightingShader.setInt("texture1", 2);
	lightingShader.setInt("texture2", 3);
	lightingShader.setInt("texture3", 4);
	lightingShader.setInt("texture4", 5);
	lightingShader.setInt("texture5", 6);
	lightingShader.setInt("texture6", 7);
	lightingShader.setInt("texture7", 8);

	glm::mat4 model;
	float angle;

	static_meshes_3D::Cylinder CD(.6, 36, .010, true, true, true);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		// directional light
		lightingShader.setVec3("dirLight.direction", 2.5f, 0.0f, 0.0f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// key light
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// fill light
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.7f, 0.7f, 0.7f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.1f, 0.1f, 0.1f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 0.7f, 0.7f, 0.7f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// view/projection transformations
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		


		//draw plane
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAOPlane);
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, -5.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 36, 6);

		// draw bowling ball (sphere)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(sphereVAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.70f, 2.5f, -0.20f));
		angle = 0.0f;
		model = glm::scale(model, glm::vec3(1.5f)); // Make it a smaller sphere
		lightingShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereIndexByteOffset);

		// draw playstation
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glBindVertexArray(VAOPSX);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.25f, 1.25f, 1.0f));
		model = glm::scale(model, glm::vec3(4.75f));
		angle = -35.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 40.0f, 0.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 42, 36);

		// draw controller body
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture4);
		glBindVertexArray(VAOBody);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 1.25f, 2.0f));
		model = glm::scale(model, glm::vec3(1.5f));
		angle = 40.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(-4.0f, 25.0f, 0.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 42, 36);

		// draw controller handle left
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture5);
		glBindVertexArray(VAOLeft);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.75f, 1.25f, 2.50f));
		model = glm::scale(model, glm::vec3(.45f));
		angle = -40.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 75.0f, 0.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 78, 24);

		//draw controller handle right
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture5);
		glBindVertexArray(VAORight);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.15f, 1.25f, 2.0f));
		model = glm::scale(model, glm::vec3(.45f));
		angle = -40.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 80.0f, 0.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 78, 24);

		// draw CD
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture6);
		glBindVertexArray(VAOCD);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.50f, 1.45f, 2.15f));
		model = glm::scale(model, glm::vec3(1.20f));
		angle = 45.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(90.0f, 90.0f, 90.0f));
		lightingShader.setMat4("model", model);
		CD.render();




		// 
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAOPlane);
	glDeleteBuffers(1, &VBOPlane);

	glDeleteVertexArrays(1, &VAOPSX);
	glDeleteBuffers(1, &VBOPSX);

	glDeleteVertexArrays(1, &VAOBody);
	glDeleteBuffers(1, &VBOBody);

	glDeleteVertexArrays(1, &VAOLeft);
	glDeleteBuffers(1, &VBOLeft);

	glDeleteVertexArrays(1, &VAORight);
	glDeleteBuffers(1, &VBORight);

	glDeleteVertexArrays(1, &sphereVAO);
	glDeleteBuffers(1, &sphereVBO);

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	glDeleteVertexArrays(1, &VAOCD);
	glDeleteBuffers(1, &VBOCD);


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	// change view between perspective and orthographics
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (perspective) {
			projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
			perspective = false;
		}
		else {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			perspective = true;
		}
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
