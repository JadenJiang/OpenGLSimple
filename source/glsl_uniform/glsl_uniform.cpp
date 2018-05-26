#include "my_shaders.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));



int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  -1.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// build and compile our shader zprogram
	Shader shaderRed(vertexShaderSource, fragmentShaderSource_red);
	Shader shaderGreen(vertexShaderSource, fragmentShaderSource_green);
	Shader shaderBlue(vertexShaderSource, fragmentShaderSource_blue);
	Shader shaderYellow(vertexShaderSource, fragmentShaderSource_yellow);

	GLuint cubeVAO, VBO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);


	GLuint uniformBlockIndexRed = glGetUniformBlockIndex(shaderRed.ID, "Matrices");
	GLuint uniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen.ID, "Matrices");
	GLuint uniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue.ID, "Matrices");
	GLuint uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow.ID, "Matrices");

	glUniformBlockBinding(shaderRed.ID, uniformBlockIndexRed, 0);
	glUniformBlockBinding(shaderGreen.ID, uniformBlockIndexGreen, 0);
	glUniformBlockBinding(shaderBlue.ID, uniformBlockIndexBlue, 0);
	glUniformBlockBinding(shaderYellow.ID, uniformBlockIndexYellow, 0);

	GLuint uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));



	GLuint myUniformBlockIndexRed = glGetUniformBlockIndex(shaderRed.ID, "MyUniform");
	GLuint myUniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen.ID, "MyUniform");
	GLuint myUniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue.ID, "MyUniform");
	GLuint myUniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow.ID, "MyUniform");

	glUniformBlockBinding(shaderRed.ID, myUniformBlockIndexRed, 1);
	glUniformBlockBinding(shaderGreen.ID, myUniformBlockIndexGreen, 1);
	glUniformBlockBinding(shaderBlue.ID, myUniformBlockIndexBlue, 1);
	glUniformBlockBinding(shaderYellow.ID, myUniformBlockIndexYellow, 1);

	GLuint myUniformBuffer;
	glGenBuffers(1, &myUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, myUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, myUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// store the projection matrix (we only do this once now) (note: we're not using zoom anymore by changing the FoV)
	glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{


		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set the view and projection matrix in the uniform block - we only have to do this once per loop iteration.
		glm::mat4 view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// draw 4 cubes 

		glBindVertexArray(cubeVAO);
		// RED
		shaderRed.use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f)); // move top-left
		//shaderRed.setMat4("model", model);
		glBindBuffer(GL_UNIFORM_BUFFER, myUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// GREEN
		shaderGreen.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f)); // move top-right
		//shaderGreen.setMat4("model", model);
		glBindBuffer(GL_UNIFORM_BUFFER, myUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// YELLOW
		shaderYellow.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f)); // move bottom-left
		//shaderYellow.setMat4("model", model);
		glBindBuffer(GL_UNIFORM_BUFFER, myUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// BLUE
		shaderBlue.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f)); // move bottom-right
		//shaderBlue.setMat4("model", model);
		glBindBuffer(GL_UNIFORM_BUFFER, myUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

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
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}