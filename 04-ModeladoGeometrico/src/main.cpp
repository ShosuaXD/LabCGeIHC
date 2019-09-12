//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"//esta lineas fueron descomentadas
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers\FirstPersonCamera.h"//se agrego esta biblioteca

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int screenWidth;
int screenHeight;

GLFWwindow * window;

Shader shader;
std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());//se instancia una camara

Sphere sphere1(20, 20);//se descomentaron las lineas para instanciar la primitivas
Sphere sphere2(20, 20);
Sphere sphere3(20, 20);
Sphere ojos(20, 20);
Cylinder cylinder1(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Cylinder cylinder3(20, 20, 0.5, 0.5);
Cylinder manoPunio(20, 20, 0.5, 0.5);//se agrego esta primitiva para complementar el modelo, practica 5
Box box1;
Box pantalones;

bool exitApp = false;
int lastMousePosX, offsetX;//se agrego offsetX y offsetY
int lastMousePosY, offsetY;
//tambien puede ser
//int offsetX = 0;
//int offsetY = 0;

float rotacionHombroDerecho = 0.0f, rotacionHombroIzquierdo = 0.0f, rotacionCodoDerecho = 0.0f, rotacionCodoIzquierdo = 0.0f;//estas variables se agregaron para la actividad de la practica 5
float rotacionMuniecaDerecha = 0.0f;
float rot0 = 0.0f, dz = 0.0f;//se agrego esta variable para la practica 5

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
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
		destroy();
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
	glEnable(GL_CULL_FACE);

	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");

	//articulaciones (hombros y piernas)
	//inicializar los buffers VAO, VBO, EBO	
	sphere1.init();
	//METODO SETTER que coloca el apuntador al shader
	sphere1.setShader(&shader);
	//setter para poner el color de la geometria
	sphere1.setColor(glm::vec4(0.5f, 0.25f, 0.0f, 1.0f));

	//ojos
	//inicializar los buffers VAO, VBO, EBO	
	sphere2.init();
	//METODO SETTER que coloca el apuntador al shader
	sphere2.setShader(&shader);
	//setter para poner el color de la geometria
	sphere2.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphere3.init();
	sphere3.setShader(&shader);
	sphere3.setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	ojos.init();
	ojos.setShader(&shader);
	ojos.setColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	//piernas, pantorrillas, brazos y antebrazos
	cylinder1.init();
	cylinder1.setShader(&shader);
	cylinder1.setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	//nariz
	cylinder2.init();
	cylinder2.setShader(&shader);
	cylinder2.setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	//mano puño-practica 5
	manoPunio.init();
	manoPunio.setShader(&shader);
	manoPunio.setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	//zapatos
	cylinder3.init();
	cylinder3.setShader(&shader);
	cylinder3.setColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	//cuerpo
	box1.init();
	box1.setShader(&shader);
	box1.setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	pantalones.init();
	pantalones.setShader(&shader);
	pantalones.setColor(glm::vec4(0.5f, 0.25f, 0.0f, 1.0f));

	camera->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));//le asignamos una posicion a la camera
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	//Destruccion de los VAO, VBO, EBO
	sphere1.destroy();
	cylinder1.destroy();
	box1.destroy();
	sphere2.destroy();
	cylinder2.destroy();
	cylinder3.destroy();
	sphere3.destroy();
	ojos.destroy();
	pantalones.destroy();
	manoPunio.destroy();//destruir objeto-pratica 5

	shader.destroy();
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
	offsetX = xpos - lastMousePosX;//se esta linea
	offsetY = ypos - lastMousePosY;//se esta linea
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	/*if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))//esta condicion nos sirve para mover la camara cuando se presione la tecla izquierda del mouse
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);//se agrego esta linea para poder controlar la rotacion de la camara con el mouse*/
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

bool processInput(bool continueApplication){
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)//se agregan estas dos lineas para mover la camara, hacia enfrente
		camera->moveFrontCamera(true, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)//se agregan estas dos lineas para mover la camara, hacia atras
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)//se agregan estas dos lineas para mover la camara, hacia la derecha
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)//se agregan estas dos lineas para mover la camara, hacia la izquierda
		camera->moveRightCamera(true, deltaTime);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))//esta condicion nos sirve para mover la camara cuando se presione la tecla izquierda del mouse
		//camera->mouseMoveCamera(offsetX, offsetY, deltaTime);//se agrego esta linea para poder controlar la rotacion de la camara con el mouse
		camera->mouseMoveCamera(offsetX, offsetY, 0.01);//modificando la linea anterior
	offsetX = 0;
	offsetY = 0;

	//se agregaron esta lineas para la actividad de la pratica 5
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rot0 = 0.0001f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rot0 = -0.0001f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		dz = 0.0001f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		dz = -0.0001f;

	//condicion para modificar el valor de la rotacion del hombro derecho
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		rotacionHombroDerecho += 0.01f;
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		rotacionHombroDerecho -= 0.01f;

	//condicion para modificar el valor de la rotacion del hombro izquierdo
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		rotacionHombroIzquierdo += 0.01f;
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		rotacionHombroIzquierdo -= 0.01f;

	//condicion para modificar el valor de la rotacion del codo derecho
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		rotacionCodoDerecho += 0.01f;
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		rotacionCodoDerecho -= 0.01f;

	//condicion para modificar el valor de la rotacion del codo derecho
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		rotacionCodoIzquierdo += 0.01f;
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		rotacionCodoIzquierdo -= 0.01f;

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	glm::mat4 model = glm::mat4(1.0f);//esta linea se mueve para la actividad de la practica 5
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
		//glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
		glm::mat4 view = camera->getViewMatrix();//modificando la linea anterior

		shader.turnOn();

		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		//glm::mat4 model = glm::mat4(1.0f);//esta linea se movio para la actividad 5

		//vizualizar con lineas la geometria
		//sphere1.enableWireMode();//se descomentan y se intercambia de lugar,//comentando esta linea para descubrir que realiza esa funcion
		//dibujado de la geometria y recibe la matriz de transfomacion 
		//sphere1.render(model);

		//cylinder1.render(model);
		//cylinder1.enableWireMode();

		//se aplican transformaciones a model
		model = glm::translate(model, glm::vec3(0, 0, dz));
		model = glm::rotate(model, rot0, glm::vec3(0.0, 1.0, 0.0));

		box1.enableWireMode();
		box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));

		glm::mat4 pant = glm::translate(model, glm::vec3(0.0f, -0.375, 0.0f));
		//pantalones.enableWireMode();
		pantalones.render(glm::scale(pant, glm::vec3(1.0f,0.25f,0.1f)));

		//articulacion 1 (hombro derecho)
		glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));//se agrego esta linea para empezar a modelar a la esponja
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j1, glm::vec3(0.1,0.1,0.1)));
		j1 = glm::rotate(j1, rotacionHombroDerecho, glm::vec3(0, 0, 1));//agregamos la rotacion segun sea el valor de rotacionHombroDerecho

		//hueso 1 (Brazo derecho)
		glm::mat4 l1 = glm::translate(j1, glm::vec3(0.25f, 0.0f, 0.0f));
		l1 = glm::rotate(l1, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l1,glm::vec3(0.1,0.5,0.1)));

		//articulacion 2 (codo derecho)
		glm::mat4 j2 = glm::translate(j1, glm::vec3(0.5f, 0.0f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));
		j2 = glm::rotate(j2, rotacionCodoDerecho, glm::vec3(0, 0, 1));//se agrego esta linea para la rotacion del codo derecho

		//hueso 2(antebrazo derecho)
		glm::mat4 l2 = glm::translate(j2, glm::vec3(0.25, 0.0, 0.0));
		l2 = glm::rotate(l2, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l2, glm::vec3(0.1, 0.5, 0.1)));

		//Muñeca derecha
		glm::mat4 j5 = glm::translate(j2, glm::vec3(0.5f, 0.0f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j5, glm::vec3(0.1, 0.1, 0.1)));
		//j5 = glm::rotate(j5, rotacionCodoDerecho, glm::vec3(0, 0, 1));

		//puño del dolor :V derecho
		glm::mat4 manoDerecha = glm::translate(j5, glm::vec3(0.06f, 0.0f, 0.0f));
		//manoDerecha = glm::rotate(manoDerecha, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
		manoPunio.enableWireMode();
		manoPunio.render(glm::scale(manoDerecha, glm::vec3(0.1, 0.1, 0.1)));

		//pulgar del dolor BV derecho
		glm::mat4 pulgarDerecho = glm::translate(manoDerecha, glm::vec3(0.0f, 0.08f, 0.0f));
		//manoDerecha = glm::rotate(manoDerecha, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
		box1.enableWireMode();
		box1.render(glm::scale(pulgarDerecho, glm::vec3(0.03, 0.03, 0.03)));

		//Articulacion 3 (Hombro izquierdo)
		glm::mat4 j3 = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));//se agrego esta linea para empezar a modelar a la esponja
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j3, glm::vec3(0.1, 0.1, 0.1)));
		j3 = glm::rotate(j3, rotacionHombroIzquierdo, glm::vec3(0, 0, 1));//agregamos la rotacion segun sea el valor de rotacionHombroIzquierdo

		//Hueso 3 (Brazo izquierdo)
		glm::mat4 l3 = glm::translate(j3, glm::vec3(-0.25f, 0.0f, 0.0f));
		l3 = glm::rotate(l3, glm::radians(-90.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l3, glm::vec3(0.1, 0.5, 0.1)));

		//Articulacion 4 (Codo izquierda)
		glm::mat4 j4 = glm::translate(j3, glm::vec3(-0.5f, 0.0f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(j4, glm::vec3(0.1, 0.1, 0.1)));
		j4 = glm::rotate(j4, rotacionCodoIzquierdo, glm::vec3(0, 0, 1));//se agrego esta linea para la rotacion del codo izquierdo

		//Hueso 4 (Antebrazo izquierdo)
		glm::mat4 l4 = glm::translate(j4, glm::vec3(-0.25, 0.0, 0.0));
		l4 = glm::rotate(l4, glm::radians(-90.0f), glm::vec3(0, 0, 1.0));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l4, glm::vec3(0.1, 0.5, 0.1)));

		//ojo 1
		glm::mat4 o1 = glm::translate(model, glm::vec3(0.25f, 0.25f, 0.05f));
		sphere2.enableWireMode();
		sphere2.render(glm::scale(o1,glm::vec3(0.20f,0.20f,0.10f)));

		//pupila 1
		glm::mat4 pu1 = glm::translate(o1, glm::vec3(0.0f, 0.0f, 0.04f));
		ojos.enableWireMode();
		ojos.render(glm::scale(pu1, glm::vec3(0.10f,0.10f,0.05f)));

		//ojo 2
		glm::mat4 o2 = glm::translate(model, glm::vec3(-0.25f, 0.25f, 0.05f));
		sphere2.enableWireMode();
		sphere2.render(glm::scale(o2, glm::vec3(0.20f, 0.20f, 0.10f)));

		//pupila 2
		glm::mat4 pu2 = glm::translate(o2, glm::vec3(0.0f, 0.0f, 0.04f));
		ojos.enableWireMode();
		ojos.render(glm::scale(pu2, glm::vec3(0.10f, 0.10f, 0.05f)));

		//Articulacion 5 (pierna derecha)
		glm::mat4 a1 = glm::translate(model, glm::vec3(0.15f, -0.5f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(a1, glm::vec3(0.1f, 0.1f, 0.1f)));

		//Hueso 5 (pierna derecha)
		glm::mat4 p1 = glm::translate(a1, glm::vec3(0.0f, -0.25, 0.0f));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(p1, glm::vec3(0.1f, 0.5f, 0.1)));

		//Articulacion 6 (rodilla derecha)
		glm::mat4 a2 = glm::translate(a1, glm::vec3(0.0f,-0.5f,0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(a2, glm::vec3(0.1f, 0.1f, 0.1f)));

		//Hueso 6 (pantorrilla derecha)
		glm::mat4 p2 = glm::translate(a2, glm::vec3(0.0f, -0.25f, 0.0f));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(p2, glm::vec3(0.1f, 0.5f, 0.1f)));

		//Articulacion 7 (tobillo derecha)
		glm::mat4 a5 = glm::translate(a2, glm::vec3(0.0f, -0.5f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(a5, glm::vec3(0.1f, 0.1f, 0.1f)));

		//Hueso 7 (pie derecho)
		glm::mat4 p5 = glm::translate(a5, glm::vec3(0.15f, 0.0f, 0.0f));
		p5 = glm::rotate(p5, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		cylinder3.enableWireMode();
		cylinder3.render(glm::scale(p5, glm::vec3(0.1f, 0.25f, 0.1f)));

		//Articulacion 8 (pierna izquierda)
		glm::mat4 a3 = glm::translate(model, glm::vec3(-0.15f, -0.5f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(a3, glm::vec3(0.1f, 0.1f, 0.1f)));

		//Hueso 8 (pierna derecha)
		glm::mat4 p3 = glm::translate(a3, glm::vec3(0.0f, -0.25, 0.0f));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(p3, glm::vec3(0.1f, 0.5f, 0.1)));

		//Articulacion 9 (rodilla izquierda)
		glm::mat4 a4 = glm::translate(a3, glm::vec3(0.0f, -0.5f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(a4, glm::vec3(0.1f, 0.1f, 0.1f)));

		//Hueso 9 (pantorrilla izquierda)
		glm::mat4 p4 = glm::translate(a4, glm::vec3(0.0f, -0.25f, 0.0f));
		cylinder1.enableWireMode();
		cylinder1.render(glm::scale(p4, glm::vec3(0.1f, 0.5f, 0.1f)));

		//Articulacion 10 (tobillo izquierdo)
		glm::mat4 a6 = glm::translate(a4, glm::vec3(0.0f, -0.5f, 0.0f));
		sphere1.enableWireMode();
		sphere1.render(glm::scale(a6, glm::vec3(0.1f, 0.1f, 0.1f)));

		//Hueso 10 (pie izquierdo)
		glm::mat4 p6 = glm::translate(a6, glm::vec3(0.0f, 0.0f, 0.15f));
		p6 = glm::rotate(p6, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		cylinder3.enableWireMode();
		cylinder3.render(glm::scale(p6, glm::vec3(0.1f, 0.25f, 0.1f)));

		//Nariz
		glm::mat4 nariz = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.15f));
		nariz = glm::rotate(nariz, glm::radians(90.0f), glm::vec3(0.1f, 0.0f, 0.0f));
		cylinder2.enableWireMode();
		cylinder2.render(glm::scale(nariz, glm::vec3(0.1f, 0.25f, 0.1f)));

		//nariz punta
		glm::mat4 nPunta = glm::translate(nariz, glm::vec3(0.0f, 0.125f, 0.0f));
		sphere3.enableWireMode();
		sphere3.render(glm::scale(nPunta, glm::vec3(0.1f, 0.1f, 0.1f)));

		shader.turnOff();

		//se reinician las variables
		dz = 0.0f;
		rot0 = 0.0f;

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
