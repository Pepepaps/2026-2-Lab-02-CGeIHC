#include <stdio.h> 
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";

Sphere sp = Sphere(1.0, 20, 20);

// =========================
// GIRO INDEPENDIENTE DE LLANTAS
// 0 = delantera izquierda
// 1 = delantera derecha
// 2 = trasera izquierda
// 3 = trasera derecha
// =========================
float giroLlantas[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

void ControlLlantas(bool* keys, float dt)
{
	float vel = 120.0f * dt;

	// Llanta 1
	if (keys[GLFW_KEY_1]) giroLlantas[0] += vel;
	if (keys[GLFW_KEY_Z]) giroLlantas[0] -= vel;

	// Llanta 2
	if (keys[GLFW_KEY_2]) giroLlantas[1] += vel;
	if (keys[GLFW_KEY_X]) giroLlantas[1] -= vel;

	// Llanta 3
	if (keys[GLFW_KEY_3]) giroLlantas[2] += vel;
	if (keys[GLFW_KEY_C]) giroLlantas[2] -= vel;

	// Llanta 4
	if (keys[GLFW_KEY_4]) giroLlantas[3] += vel;
	if (keys[GLFW_KEY_V]) giroLlantas[3] -= vel;
}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		7, 6, 5, 5, 4, 7,
		4, 0, 3, 3, 7, 4,
		4, 5, 1, 1, 0, 4,
		3, 2, 6, 6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo); // meshList[0]
}

void CrearCilindro(int res, float R)
{
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	for (n = 0; n <= res; n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		else {
			x = R * cos(0);
			z = R * sin(0);
		}

		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			case 3: vertices.push_back(x); break;
			case 4: vertices.push_back(0.5f); break;
			case 5: vertices.push_back(z); break;
			}
		}
	}

	for (n = 0; n <= res; n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		vertices.push_back(x);
		vertices.push_back(-0.5f);
		vertices.push_back(z);
	}

	for (n = 0; n <= res; n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		vertices.push_back(x);
		vertices.push_back(0.5f);
		vertices.push_back(z);
	}

	for (i = 0; i < (int)vertices.size(); i++) indices.push_back(i);

	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro); // meshList[1]
}

void CrearPiramideCuadrangular()
{
	vector<unsigned int> indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,3
	};

	vector<GLfloat> vertices = {
		 0.5f,-0.5f, 0.5f,
		 0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f, 0.5f,
		 0.0f, 0.5f, 0.0f,
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(vertices, indices, 15, 18);
	meshList.push_back(piramide); // meshList[2]
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();

	CrearCubo();                 // meshList[0]
	CrearCilindro(24, 1.0f);     // meshList[1]
	CrearPiramideCuadrangular(); // meshList[2]
	CreateShaders();

	camera = Camera(
		glm::vec3(0.0f, 6.0f, 6.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-90.0f, -15.0f,
		0.3f, 0.2f
	);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;

	glm::mat4 projection = glm::perspective(
		glm::radians(60.0f),
		(GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(),
		0.1f,
		100.0f
	);

	sp.init();
	sp.load();

	glm::mat4 model(1.0f);
	glm::mat4 modelaux(1.0f);
	glm::mat4 modelaux2(1.0f);
	glm::mat4 nodoPiramide(1.0f);
	glm::vec3 color(1.0f, 1.0f, 1.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Control independiente de las 4 llantas
		ControlLlantas(mainWindow.getsKeys(), deltaTime);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// =========================
		// NODO RAÍZ
		// =========================
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -10.0f));
		modelaux = model;

		// =========================
		// BASE - PIRÁMIDE CUADRANGULAR
		// =========================
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		nodoPiramide = model;

		model = glm::scale(model, glm::vec3(4.5f, 3.0f, 3.0f));
		color = glm::vec3(0.78f, 0.78f, 0.78f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		// =========================
		// LLANTAS EN LAS ESQUINAS
		// =========================
		float ruedaX[4] = { -2.2f,  2.2f, -2.2f,  2.2f };
		float ruedaZ[4] = { 1.5f,  1.5f, -1.5f, -1.5f };

		for (int i = 0; i < 4; i++) {
			model = nodoPiramide;
			model = glm::translate(model, glm::vec3(ruedaX[i], -2.0f, ruedaZ[i]));

			// Orientación del cilindro para formar la llanta
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			// Giro independiente de cada llanta
			model = glm::rotate(model, glm::radians(giroLlantas[i]), glm::vec3(1.0f, 0.0f, 0.0f));

			model = glm::scale(model, glm::vec3(0.85f, 0.40f, 0.85f));
			color = glm::vec3(0.08f, 0.08f, 0.08f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[1]->RenderMeshGeometry();
		}

		// =========================
		// CUERPO - PRISMA RECTANGULAR
		// =========================
		model = nodoPiramide;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(4.0f, 2.5f, 2.5f));
		color = glm::vec3(1.0f, 0.82f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// =========================
		// ARTICULACIÓN 1
		// =========================
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, 0.7f, 0.0f));
		model = glm::rotate(model, glm::radians(35.0f + mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		color = glm::vec3(1.0f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Brazo 1
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux2 = model;
		model = glm::scale(model, glm::vec3(5.0f, 0.6f, 0.6f));
		color = glm::vec3(1.0f, 0.2f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// =========================
		// ARTICULACIÓN 2
		// =========================
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-70.0f + mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		color = glm::vec3(1.0f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Brazo 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.2f, 0.0f, 0.0f));
		modelaux2 = model;
		model = glm::scale(model, glm::vec3(4.4f, 0.55f, 0.55f));
		color = glm::vec3(0.1f, 1.0f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// =========================
		// ARTICULACIÓN 3
		// =========================
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.2f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(35.0f + mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.30f, 0.30f, 0.30f));
		color = glm::vec3(1.0f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Brazo 3
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.8f, 0.0f, 0.0f));
		modelaux2 = model;
		model = glm::scale(model, glm::vec3(3.6f, 0.45f, 0.45f));
		color = glm::vec3(0.2f, 0.65f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// =========================
		// ARTICULACIÓN FINAL
		// =========================
		model = modelaux2;
		model = glm::translate(model, glm::vec3(1.8f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.30f, 0.30f, 0.30f));
		color = glm::vec3(1.0f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// =========================
		// CANASTA
		// =========================

		// Piso
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.9f, -0.35f, 0.0f));
		model = glm::scale(model, glm::vec3(1.8f, 0.2f, 1.2f));
		color = glm::vec3(0.92f, 0.92f, 0.92f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Poste frontal izquierdo
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.15f, 0.2f, 0.5f));
		model = glm::scale(model, glm::vec3(0.12f, 1.0f, 0.12f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Poste frontal derecho
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.65f, 0.2f, 0.5f));
		model = glm::scale(model, glm::vec3(0.12f, 1.0f, 0.12f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Poste trasero izquierdo
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.15f, 0.2f, -0.5f));
		model = glm::scale(model, glm::vec3(0.12f, 1.0f, 0.12f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Poste trasero derecho
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.65f, 0.2f, -0.5f));
		model = glm::scale(model, glm::vec3(0.12f, 1.0f, 0.12f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Barra superior frontal
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.9f, 0.65f, 0.5f));
		model = glm::scale(model, glm::vec3(1.8f, 0.10f, 0.10f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Barra superior trasera
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.9f, 0.65f, -0.5f));
		model = glm::scale(model, glm::vec3(1.8f, 0.10f, 0.10f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Barra lateral izquierda superior
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.15f, 0.65f, 0.0f));
		model = glm::scale(model, glm::vec3(0.10f, 0.10f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Barra lateral derecha superior
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.65f, 0.65f, 0.0f));
		model = glm::scale(model, glm::vec3(0.10f, 0.10f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}