#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <algorithm>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\random.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

const float toRadians = 3.14159265f / 180.0f;
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
	meshList.push_back(cubo);
}

void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
	};

	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f,-0.25f,
		 0.0f, -0.5f,-0.5f,
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);
}

void CrearCilindro(int res, float R)
{
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
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

	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(-0.5f); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}

	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(0.5f); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}

	for (i = 0; i < (int)vertices.size(); i++) indices.push_back(i);

	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

void CrearCono(int res, float R)
{
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	vertices.push_back(0.0f);
	vertices.push_back(0.5f);
	vertices.push_back(0.0f);

	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0: vertices.push_back(x); break;
			case 1: vertices.push_back(y); break;
			case 2: vertices.push_back(z); break;
			}
		}
	}

	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5f);
	vertices.push_back(R * sin(0) * dt);

	for (i = 0; i < res + 2; i++) indices.push_back(i);

	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4
	};

	vector<GLfloat> piramidecuadrangular_vertices = {
		 0.5f,-0.5f, 0.5f,
		 0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f, 0.5f,
		 0.0f, 0.5f, 0.0f,
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// ==========================================================
// ARTICULACIONES
// ==========================================================

float pataSuperior[4] = { 10.0f, -10.0f, -8.0f, 8.0f };
float pataInferior[4] = { -20.0f, -20.0f, -20.0f, -20.0f };
float colaAng = 20.0f;
float colaAng2 = 10.0f;  // segunda articulación de cola

float limitar(float valor, float minV, float maxV)
{
	if (valor < minV) return minV;
	if (valor > maxV) return maxV;
	return valor;
}

void ActualizarArticulaciones(bool* keys, float dt)
{
	float vel = 80.0f * dt;

	// Delantera izquierda — KP7/KP8 superior, KP4/KP5 inferior
	if (keys[GLFW_KEY_KP_7]) pataSuperior[0] += vel;
	if (keys[GLFW_KEY_KP_8]) pataSuperior[0] -= vel;
	if (keys[GLFW_KEY_KP_4]) pataInferior[0] += vel;
	if (keys[GLFW_KEY_KP_5]) pataInferior[0] -= vel;

	// Delantera derecha — KP9/KP6 superior, KP3/KP. inferior
	if (keys[GLFW_KEY_KP_9]) pataSuperior[1] += vel;
	if (keys[GLFW_KEY_KP_6]) pataSuperior[1] -= vel;
	if (keys[GLFW_KEY_KP_3]) pataInferior[1] += vel;
	if (keys[GLFW_KEY_KP_DECIMAL]) pataInferior[1] -= vel;

	// Trasera izquierda — KP+/KP- superior, KP*/KP/ inferior
	if (keys[GLFW_KEY_KP_ADD])      pataSuperior[2] += vel;
	if (keys[GLFW_KEY_KP_SUBTRACT]) pataSuperior[2] -= vel;
	if (keys[GLFW_KEY_KP_MULTIPLY]) pataInferior[2] += vel;
	if (keys[GLFW_KEY_KP_DIVIDE])   pataInferior[2] -= vel;

	// Trasera derecha — KP_Enter/KP0 superior, KP1/KP2 inferior
	if (keys[GLFW_KEY_KP_ENTER]) pataSuperior[3] += vel;
	if (keys[GLFW_KEY_KP_0])     pataSuperior[3] -= vel;
	if (keys[GLFW_KEY_KP_1])     pataInferior[3] += vel;
	if (keys[GLFW_KEY_KP_2])     pataInferior[3] -= vel;

	// Cola articulación 1 — F/V
	if (keys[GLFW_KEY_F]) colaAng += vel;
	if (keys[GLFW_KEY_V]) colaAng -= vel;

	// Cola articulación 2 — R/C
	if (keys[GLFW_KEY_R]) colaAng2 += vel;
	if (keys[GLFW_KEY_C]) colaAng2 -= vel;

	for (int i = 0; i < 4; i++) {
		pataSuperior[i] = limitar(pataSuperior[i], -60.0f, 60.0f);
		pataInferior[i] = limitar(pataInferior[i], -90.0f, 35.0f);
	}
	colaAng = limitar(colaAng, -45.0f, 55.0f);
	colaAng2 = limitar(colaAng2, -60.0f, 60.0f);
}

void DibujarCubo(GLuint uniformModel, GLuint uniformColor, glm::mat4 model, glm::vec3 color)
{
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));
	meshList[0]->RenderMesh();
}

void DibujarCilindro(GLuint uniformModel, GLuint uniformColor, glm::mat4 model, glm::vec3 color)
{
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));
	meshList[2]->RenderMeshGeometry();
}

void DibujarCono(GLuint uniformModel, GLuint uniformColor, glm::mat4 model, glm::vec3 color)
{
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));
	meshList[3]->RenderMeshGeometry();
}

void DibujarPiramide(GLuint uniformModel, GLuint uniformColor, glm::mat4 model, glm::vec3 color)
{
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));
	meshList[4]->RenderMeshGeometry();
}

void DibujarEsfera(GLuint uniformModel, GLuint uniformColor, glm::mat4 model, glm::vec3 color)
{
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));
	sp.render();
}

void DibujarPata(
	glm::mat4 cuerpoBase,
	glm::vec3 offset,
	float angSup,
	float angInf,
	GLuint uniformModel,
	GLuint uniformColor)
{
	glm::mat4 model = cuerpoBase;
	model = glm::translate(model, offset);
	model = glm::rotate(model, glm::radians(angSup), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 nodoSuperior = model;

	// Articulación superior
	glm::mat4 aux = nodoSuperior;
	aux = glm::scale(aux, glm::vec3(0.26f, 0.26f, 0.26f));
	DibujarEsfera(uniformModel, uniformColor, aux, glm::vec3(0.78f, 0.72f, 0.95f));

	// Segmento superior más ancho
	aux = nodoSuperior;
	aux = glm::translate(aux, glm::vec3(0.0f, -0.62f, 0.0f));
	glm::mat4 finSegmentoSup = aux;
	aux = glm::scale(aux, glm::vec3(0.42f, 1.25f, 0.42f));
	DibujarCilindro(uniformModel, uniformColor, aux, glm::vec3(0.25f, 0.52f, 0.95f));

	// Articulación media
	aux = finSegmentoSup;
	aux = glm::translate(aux, glm::vec3(0.0f, -0.62f, 0.0f));
	aux = glm::rotate(aux, glm::radians(angInf), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 nodoInferior = aux;

	glm::mat4 art2 = nodoInferior;
	art2 = glm::scale(art2, glm::vec3(0.24f, 0.24f, 0.24f));
	DibujarEsfera(uniformModel, uniformColor, art2, glm::vec3(0.78f, 0.72f, 0.95f));

	// Segmento inferior más ancho
	aux = nodoInferior;
	aux = glm::translate(aux, glm::vec3(0.0f, -0.70f, 0.0f));
	glm::mat4 finSegmentoInf = aux;
	aux = glm::scale(aux, glm::vec3(0.35f, 1.40f, 0.35f));
	DibujarCilindro(uniformModel, uniformColor, aux, glm::vec3(0.25f, 0.52f, 0.95f));

	// Pie
	aux = finSegmentoInf;
	aux = glm::translate(aux, glm::vec3(0.0f, -0.82f, 0.18f));
	aux = glm::rotate(aux, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	aux = glm::scale(aux, glm::vec3(0.44f, 0.35f, 0.60f));
	DibujarPiramide(uniformModel, uniformColor, aux, glm::vec3(0.68f, 0.65f, 0.82f));
}

// ==========================================================
// MAIN
// ==========================================================

int main()
{
	mainWindow = Window(1000, 700);
	mainWindow.Initialise();

	CrearCubo();                  // meshList[0]
	CrearPiramideTriangular();    // meshList[1]
	CrearCilindro(20, 0.5f);      // meshList[2]
	CrearCono(20, 0.5f);          // meshList[3]
	CrearPiramideCuadrangular();  // meshList[4]
	CreateShaders();

	camera = Camera(
		glm::vec3(0.0f, 2.0f, 12.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-90.0f,
		-10.0f,
		0.25f,
		0.20f
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

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		ActualizarArticulaciones(mainWindow.getsKeys(), deltaTime);

		glClearColor(0.05f, 0.06f, 0.10f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();

		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// ======================================================
		// NODO RAÍZ
		// ======================================================
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -8.0f));
		model = glm::rotate(model, glm::radians(-12.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 nodoCuerpo = model;

		// ======================================================
		// CUERPO
		// ======================================================
		glm::mat4 aux = nodoCuerpo;
		aux = glm::scale(aux, glm::vec3(4.2f, 2.3f, 2.1f));
		DibujarCubo(uniformModel, uniformColor, aux, glm::vec3(0.25f, 0.70f, 0.92f));

		// Parte superior trasera
		aux = nodoCuerpo;
		aux = glm::translate(aux, glm::vec3(-0.35f, 0.35f, 0.0f));
		aux = glm::scale(aux, glm::vec3(2.0f, 1.0f, 2.0f));
		DibujarCubo(uniformModel, uniformColor, aux, glm::vec3(0.23f, 0.64f, 0.88f));

		// ======================================================
		// CABEZA
		// ======================================================
		glm::mat4 nodoCabeza = nodoCuerpo;
		nodoCabeza = glm::translate(nodoCabeza, glm::vec3(2.55f, 0.55f, 0.0f));

		aux = nodoCabeza;
		aux = glm::scale(aux, glm::vec3(2.1f, 1.6f, 1.55f));
		DibujarCubo(uniformModel, uniformColor, aux, glm::vec3(0.23f, 0.64f, 0.88f));

		// Hocico
		glm::mat4 nodoHocico = nodoCabeza;
		nodoHocico = glm::translate(nodoHocico, glm::vec3(1.45f, -0.08f, 0.0f));

		aux = nodoHocico;
		aux = glm::scale(aux, glm::vec3(1.5f, 0.75f, 0.95f));
		DibujarCubo(uniformModel, uniformColor, aux, glm::vec3(0.85f, 0.88f, 0.94f));

		// Nariz
		aux = nodoHocico;
		aux = glm::translate(aux, glm::vec3(0.92f, -0.02f, 0.0f));
		aux = glm::scale(aux, glm::vec3(0.18f, 0.18f, 0.18f));
		DibujarEsfera(uniformModel, uniformColor, aux, glm::vec3(0.02f, 0.02f, 0.02f));

		// ======================================================
		// OJOS 
		// ======================================================

		// Ojo izquierdo (esclerótica)
		aux = nodoCabeza;
		aux = glm::translate(aux, glm::vec3(0.55f, 0.12f, 0.82f));
		aux = glm::scale(aux, glm::vec3(0.16f, 0.16f, 0.16f));
		DibujarEsfera(uniformModel, uniformColor, aux, glm::vec3(0.95f, 0.95f, 0.98f));
		// Pupila izquierda
		aux = nodoCabeza;
		aux = glm::translate(aux, glm::vec3(0.68f, 0.12f, 0.85f));
		aux = glm::scale(aux, glm::vec3(0.08f, 0.08f, 0.08f));
		DibujarEsfera(uniformModel, uniformColor, aux, glm::vec3(0.02f, 0.02f, 0.05f));

		// Ojo derecho (esclerótica)
		aux = nodoCabeza;
		aux = glm::translate(aux, glm::vec3(0.55f, 0.12f, -0.82f));
		aux = glm::scale(aux, glm::vec3(0.16f, 0.16f, 0.16f));
		DibujarEsfera(uniformModel, uniformColor, aux, glm::vec3(0.95f, 0.95f, 0.98f));
		// Pupila derecha
		aux = nodoCabeza;
		aux = glm::translate(aux, glm::vec3(0.68f, 0.12f, -0.85f));
		aux = glm::scale(aux, glm::vec3(0.08f, 0.08f, 0.08f));
		DibujarEsfera(uniformModel, uniformColor, aux, glm::vec3(0.02f, 0.02f, 0.05f));

		// ======================================================
		// OREJAS
		// ======================================================
		aux = nodoCabeza;
		aux = glm::translate(aux, glm::vec3(-0.10f, 1.05f, 0.55f));
		aux = glm::rotate(aux, glm::radians(-8.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		aux = glm::scale(aux, glm::vec3(0.48f, 0.95f, 0.48f));
		DibujarCono(uniformModel, uniformColor, aux, glm::vec3(0.92f, 0.92f, 0.96f));

		aux = nodoCabeza;
		aux = glm::translate(aux, glm::vec3(-0.10f, 1.05f, -0.55f));
		aux = glm::rotate(aux, glm::radians(-8.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		aux = glm::scale(aux, glm::vec3(0.48f, 0.95f, 0.48f));
		DibujarCono(uniformModel, uniformColor, aux, glm::vec3(0.92f, 0.92f, 0.96f));

		// ======================================================
		// COLA — 2 articulaciones
		// ======================================================
		glm::mat4 nodoCola1 = nodoCuerpo;
		nodoCola1 = glm::translate(nodoCola1, glm::vec3(-2.15f, 0.95f, 0.0f));
		nodoCola1 = glm::rotate(nodoCola1, glm::radians(colaAng), glm::vec3(0.0f, 0.0f, 1.0f));

		// Segmento 1
		aux = nodoCola1;
		aux = glm::translate(aux, glm::vec3(0.0f, 0.75f, 0.0f));
		aux = glm::scale(aux, glm::vec3(0.26f, 1.5f, 0.26f));
		DibujarCilindro(uniformModel, uniformColor, aux, glm::vec3(0.78f, 0.72f, 0.95f));

		// Nodo segunda articulación
		glm::mat4 nodoCola2 = nodoCola1;
		nodoCola2 = glm::translate(nodoCola2, glm::vec3(0.0f, 1.50f, 0.0f));
		nodoCola2 = glm::rotate(nodoCola2, glm::radians(colaAng2), glm::vec3(0.0f, 0.0f, 1.0f));

		// Esfera de unión
		aux = nodoCola2;
		aux = glm::scale(aux, glm::vec3(0.20f, 0.20f, 0.20f));
		DibujarEsfera(uniformModel, uniformColor, aux, glm::vec3(0.90f, 0.88f, 0.98f));

		// Segmento 2
		aux = nodoCola2;
		aux = glm::translate(aux, glm::vec3(0.0f, 0.55f, 0.0f));
		aux = glm::scale(aux, glm::vec3(0.18f, 1.1f, 0.18f));
		DibujarCilindro(uniformModel, uniformColor, aux, glm::vec3(0.88f, 0.84f, 0.98f));

		// Punta de la cola
		aux = nodoCola2;
		aux = glm::translate(aux, glm::vec3(0.0f, 1.15f, 0.0f));
		aux = glm::scale(aux, glm::vec3(0.16f, 0.16f, 0.16f));
		DibujarEsfera(uniformModel, uniformColor, aux, glm::vec3(0.97f, 0.96f, 1.0f));

		// ======================================================
		// PATAS
		// ======================================================
		DibujarPata(nodoCuerpo, glm::vec3(1.35f, -0.90f, 0.78f), pataSuperior[0], pataInferior[0], uniformModel, uniformColor);
		DibujarPata(nodoCuerpo, glm::vec3(1.35f, -0.90f, -0.78f), pataSuperior[1], pataInferior[1], uniformModel, uniformColor);
		DibujarPata(nodoCuerpo, glm::vec3(-1.15f, -0.90f, 0.78f), pataSuperior[2], pataInferior[2], uniformModel, uniformColor);
		DibujarPata(nodoCuerpo, glm::vec3(-1.15f, -0.90f, -0.78f), pataSuperior[3], pataInferior[3], uniformModel, uniformColor);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}