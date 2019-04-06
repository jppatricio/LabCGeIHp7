//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
//Texture includes
//Descomentar // ---- CAMBIO
#include "Headers/Texture.h"

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere(20, 20);
Sphere sphere2(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5, 2);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Box box;

Shader shader;
//Descomentar
Shader shaderTexture;

GLuint textureID1, textureID2; // Se agrega el id 2

GLuint textureIDBody, textureIDTop, textureIDBottom; // para body, top y bottom de la lata qwe
GLuint textureIDCube;

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	shader.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	//Descomentar
	// DECOMENTAR linea 38
	shaderTexture.initialize("../../Shaders/texturizado.vs", "../../Shaders/texturizado.fs");

	sphere.init();
	sphere.setShader(&shader);
	sphere.setColor(glm::vec3(0.3, 0.3, 1.0));

	sphere2.init();
	//Cambiar el objetos shader a shaderTexture
	sphere2.setShader(&shaderTexture);
	sphere2.setColor(glm::vec3(0.3, 0.3, 1.0));
	sphere2.scaleUVS(glm::vec2(3.0f, 1.0f));

	cylinder.init();
	cylinder.setShader(&shaderTexture);
	cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shader);
	cylinder2.setColor(glm::vec3(0.2, 0.7, 0.3));

	box.init();
	box.setShader(&shaderTexture);//
	box.setColor(glm::vec3(0.2, 0.8, 0.4));

	camera->setPosition(glm::vec3(0.0f, 0.0f, 0.4f));

	// Descomentar --- CHANGE
	
	int imageWidth, imageHeight;
	//Texture texture1("../../Textures/goku.png");
	Texture texture1("../../Textures/texturaLadrillos.jpg");
	FIBITMAP* bitmap = texture1.loadImage();
	unsigned char * data = texture1.convertToData(bitmap, imageWidth, imageHeight);
	// Se genera la textura y se le asigna su apuntador
	glGenTextures(1, &textureID1);
	// Se enlaza el tipo de textura al ID: textureID1 (2D)
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data){
		glTexImage2D(
			GL_TEXTURE_2D, //Tipo de textura 
			0,// MIPMAPS -> no creamos los niveles (openGl los creará con glGenerateMipmap)
			GL_RGBA,// Formato interno del Buffer
			imageWidth,
			imageHeight,
			0,// Borde de la imágen
			GL_BGRA,// Formato de la librería
			GL_UNSIGNED_BYTE,// tipo de dato
			data//Datos de la imágen
		);
		glGenerateMipmap(GL_TEXTURE_2D); // OPENGL generará los MIPMAPS
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture1.freeImage(bitmap);

	//COPY PASTE DE LO DE ARRIBA PARA LA SEGUNDA TEXTURA

	Texture texture2("../../Textures/goku.png");
	FIBITMAP* bitmap2 = texture2.loadImage(false);
	unsigned char * data2 = texture2.convertToData(bitmap2, imageWidth, imageHeight);
	// Se genera la textura y se le asigna su apuntador
	glGenTextures(1, &textureID2);
	// Se enlaza el tipo de textura al ID: textureID1 (2D)
	glBindTexture(GL_TEXTURE_2D, textureID2);
	// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data2) {
		glTexImage2D(
			GL_TEXTURE_2D, //Tipo de textura 
			0,// MIPMAPS -> no creamos los niveles (openGl los creará con glGenerateMipmap)
			GL_RGBA,// Formato interno del Buffer
			imageWidth,
			imageHeight,
			0,// Borde de la imágen
			GL_BGRA,// Formato de la librería
			GL_UNSIGNED_BYTE,// tipo de dato
			data2//Datos de la imágen
		);
		glGenerateMipmap(GL_TEXTURE_2D); // OPENGL generará los MIPMAPS
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture2.freeImage(bitmap2);

	////------------------------------------------------
	// PARA LA TEXTURA BODY
	Texture texture3("../../Textures/body.png");
	FIBITMAP* bitmap3 = texture3.loadImage(false);
	unsigned char * data3 = texture3.convertToData(bitmap3, imageWidth, imageHeight);
	// Se genera la textura y se le asigna su apuntador
	glGenTextures(1, &textureIDBody);
	// Se enlaza el tipo de textura al ID
	glBindTexture(GL_TEXTURE_2D, textureIDBody);
	// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data3) {
		glTexImage2D(
			GL_TEXTURE_2D, //Tipo de textura 
			0,// MIPMAPS -> no creamos los niveles (openGl los creará con glGenerateMipmap)
			GL_RGBA,// Formato interno del Buffer
			imageWidth,
			imageHeight,
			0,// Borde de la imágen
			GL_BGRA,// Formato de la librería
			GL_UNSIGNED_BYTE,// tipo de dato
			data3//Datos de la imágen
		);
		glGenerateMipmap(GL_TEXTURE_2D); // OPENGL generará los MIPMAPS
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture3.freeImage(bitmap3);

	////------------------------------------------------
	// PARA LA TEXTURA TOP CAP
	Texture texture4("../../Textures/topCap.png");
	FIBITMAP* bitmap4 = texture4.loadImage(false);
	unsigned char * data4 = texture4.convertToData(bitmap4, imageWidth, imageHeight);
	// Se genera la textura y se le asigna su apuntador
	glGenTextures(1, &textureIDTop);
	// Se enlaza el tipo de textura al ID
	glBindTexture(GL_TEXTURE_2D, textureIDTop);
	// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data4) {
		glTexImage2D(
			GL_TEXTURE_2D, //Tipo de textura 
			0,// MIPMAPS -> no creamos los niveles (openGl los creará con glGenerateMipmap)
			GL_RGBA,// Formato interno del Buffer
			imageWidth,
			imageHeight,
			0,// Borde de la imágen
			GL_BGRA,// Formato de la librería
			GL_UNSIGNED_BYTE,// tipo de dato
			data4//Datos de la imágen
		);
		glGenerateMipmap(GL_TEXTURE_2D); // OPENGL generará los MIPMAPS
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture4.freeImage(bitmap4);

	//------------------------------------------------
	// PARA LA TEXTURA BOTTOM CAP
	Texture texture5("../../Textures/bottomCap.png");
	FIBITMAP* bitmap5 = texture5.loadImage(false);
	unsigned char * data5 = texture5.convertToData(bitmap5, imageWidth, imageHeight);
	// Se genera la textura y se le asigna su apuntador
	glGenTextures(1, &textureIDBottom);
	// Se enlaza el tipo de textura al ID
	glBindTexture(GL_TEXTURE_2D, textureIDBottom);
	// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data5) {
		glTexImage2D(
			GL_TEXTURE_2D, //Tipo de textura 
			0,// MIPMAPS -> no creamos los niveles (openGl los creará con glGenerateMipmap)
			GL_RGBA,// Formato interno del Buffer
			imageWidth,
			imageHeight,
			0,// Borde de la imágen
			GL_BGRA,// Formato de la librería
			GL_UNSIGNED_BYTE,// tipo de dato
			data5//Datos de la imágen
		);
		glGenerateMipmap(GL_TEXTURE_2D); // OPENGL generará los MIPMAPS
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture5.freeImage(bitmap5);

	//------------------------------------------------
	//CUBE

	Texture texture6("../../Textures/cube.png");
	FIBITMAP* bitmap6 = texture6.loadImage(false);
	unsigned char * data6 = texture6.convertToData(bitmap6, imageWidth, imageHeight);
	// Se genera la textura y se le asigna su apuntador
	glGenTextures(1, &textureIDCube);
	// Se enlaza el tipo de textura al ID: textureID1 (2D)
	glBindTexture(GL_TEXTURE_2D, textureIDCube);
	// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data6) {
		glTexImage2D(
			GL_TEXTURE_2D, //Tipo de textura 
			0,// MIPMAPS -> no creamos los niveles (openGl los creará con glGenerateMipmap)
			GL_RGBA,// Formato interno del Buffer
			imageWidth,
			imageHeight,
			0,// Borde de la imágen
			GL_BGRA,// Formato de la librería
			GL_UNSIGNED_BYTE,// tipo de dato
			data6//Datos de la imágen
		);
		glGenerateMipmap(GL_TEXTURE_2D); // OPENGL generará los MIPMAPS
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture6.freeImage(bitmap6);

	////------------------------------------------------
}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shader.destroy();
	sphere.destroy();
	cylinder.destroy();
	cylinder2.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		//Descomentar
		//Enlazamiento de la textura a utilizar
		glBindTexture(GL_TEXTURE_2D, textureID1);


		sphere2.setProjectionMatrix(projection);
		sphere2.setViewMatrix(view);
		//sphere2.enableWireMode();
		sphere2.setPosition(glm::vec3(0.0, 0.0, -4.0));
		sphere2.render();

		//PARA EL CUBO
		//Descomentar
		//Enlazamiento de la textura a utilizar
		glBindTexture(GL_TEXTURE_2D, textureIDCube);


		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setPosition(glm::vec3(2.0, 0.0, -3.0));
		box.render();
		//-----------

		//Cilindro

		glBindTexture(GL_TEXTURE_2D, textureIDBody);// para el cuerpo de la lata
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.setPosition(glm::vec3(-2.0, 0.0, -3.0));
		cylinder.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3); // (SIN TAPAS) -- 2 por que son 2 alturas y cada indice son 3 veces
		glBindTexture(GL_TEXTURE_2D, textureIDTop);// top
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3); // Tapa TOP
		glBindTexture(GL_TEXTURE_2D, textureIDBottom); //bottom
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3); // Tapa BOTTOM
		//--

		//Descomentar
		//Enlazamiento de ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}