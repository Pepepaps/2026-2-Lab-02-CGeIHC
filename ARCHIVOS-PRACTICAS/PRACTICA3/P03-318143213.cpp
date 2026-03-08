// práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>

// glm
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\random.hpp>

// clases para dar orden y limpieza al código
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

// tecla E: Rotar sobre el eje X
// tecla R: Rotar sobre el eje Y
// tecla T: Rotar sobre el eje Z

using std::vector;

// Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0f;
const float PI = 3.14159265f;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;

// Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";

Sphere sp = Sphere(1.0, 20, 20);

// =========================
// Colores del Pyraminx
// =========================
const glm::vec3 COLOR_BLACK(0.0f, 0.0f, 0.0f);
const glm::vec3 COLOR_BLUE(0.10f, 0.40f, 0.95f);
const glm::vec3 COLOR_PINK(0.95f, 0.30f, 0.65f);
const glm::vec3 COLOR_GREEN(0.15f, 0.80f, 0.35f);
const glm::vec3 COLOR_YELLOW(0.98f, 0.85f, 0.15f);

// =========================
// Geometría original
// =========================
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
	};

	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,  0.0f,   // 0
		 0.5f, -0.5f,  0.0f,   // 1
		 0.0f,  0.5f, -0.25f,  // 2
		 0.0f, -0.5f, -0.5f    // 3
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);
}

/*
Crear cilindro, cono y esferas con arreglos dinámicos vector
creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R)
{
	int n, i;
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	// paredes
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

	// circunferencia inferior
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

	// circunferencia superior
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
		 0.0f, 0.5f, 0.0f
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

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}

// =========================
// Helpers para el Pyraminx
// =========================

// Punto dentro de la cara triangular ABC usando coordenadas baricéntricas discretas
static glm::vec3 gridPoint(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C,
	int i, int j, int N)
{
	float a = (float)i / (float)N;
	float b = (float)j / (float)N;
	float c = 1.0f - a - b;

	return c * A + a * B + b * C;
}

// Inserta un triángulo un poco más adentro para dejar ver borde negro
static void pushTriInset(vector<GLfloat>& V, vector<unsigned int>& I,
	const glm::vec3& a0, const glm::vec3& b0, const glm::vec3& c0,
	float inset)
{
	glm::vec3 centroid = (a0 + b0 + c0) / 3.0f;

	glm::vec3 a = glm::mix(a0, centroid, inset);
	glm::vec3 b = glm::mix(b0, centroid, inset);
	glm::vec3 c = glm::mix(c0, centroid, inset);

	unsigned int base = static_cast<unsigned int>(V.size() / 3);

	V.push_back(a.x); V.push_back(a.y); V.push_back(a.z);
	V.push_back(b.x); V.push_back(b.y); V.push_back(b.z);
	V.push_back(c.x); V.push_back(c.y); V.push_back(c.z);

	I.push_back(base);
	I.push_back(base + 1);
	I.push_back(base + 2);
}

// Crea una cara triangular subdividida en orden N
static Mesh* buildSubdividedFace(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C,
	int N, float inset)
{
	vector<GLfloat> V;
	vector<unsigned int> I;

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N - i; ++j)
		{
			// triángulo upright
			glm::vec3 p0 = gridPoint(A, B, C, i, j, N);
			glm::vec3 p1 = gridPoint(A, B, C, i + 1, j, N);
			glm::vec3 p2 = gridPoint(A, B, C, i, j + 1, N);
			pushTriInset(V, I, p0, p1, p2, inset);

			// triángulo invertido
			if (i + j < N - 1)
			{
				glm::vec3 p3 = gridPoint(A, B, C, i + 1, j, N);
				glm::vec3 p4 = gridPoint(A, B, C, i + 1, j + 1, N);
				glm::vec3 p5 = gridPoint(A, B, C, i, j + 1, N);
				pushTriInset(V, I, p3, p4, p5, inset);
			}
		}
	}

	Mesh* m = new Mesh();
	m->CreateMesh(V.data(), I.data(),
		static_cast<unsigned int>(V.size()),
		static_cast<unsigned int>(I.size()));
	return m;
}

static Mesh* buildBaseTetra(const glm::vec3 v[4])
{
	vector<GLfloat> V = {
		v[0].x, v[0].y, v[0].z,
		v[1].x, v[1].y, v[1].z,
		v[2].x, v[2].y, v[2].z,
		v[3].x, v[3].y, v[3].z
	};

	vector<unsigned int> I = {
		0,1,2,
		0,3,1,
		0,2,3,
		1,3,2
	};

	Mesh* m = new Mesh();
	m->CreateMesh(V.data(), I.data(),
		static_cast<unsigned int>(V.size()),
		static_cast<unsigned int>(I.size()));
	return m;
}

// Crea: [0] base negra + [1..4] caras subdivididas
static void CreatePyraminxMeshes(int N, float inset)
{
	meshList.clear();

	// tetraedro regular centrado
	glm::vec3 v[4] = {
		glm::normalize(glm::vec3(1.0f,  1.0f,  1.0f)),
		glm::normalize(glm::vec3(-1.0f,  1.0f, -1.0f)),
		glm::normalize(glm::vec3(-1.0f, -1.0f,  1.0f)),
		glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f))
	};

	// Base negra
	meshList.push_back(buildBaseTetra(v));

	// 4 caras triangulares subdivididas
	meshList.push_back(buildSubdividedFace(v[0], v[1], v[2], N, inset)); // cara 1
	meshList.push_back(buildSubdividedFace(v[0], v[3], v[1], N, inset)); // cara 2
	meshList.push_back(buildSubdividedFace(v[0], v[2], v[3], N, inset)); // cara 3
	meshList.push_back(buildSubdividedFace(v[1], v[3], v[2], N, inset)); // cara 4
}

int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	glEnable(GL_DEPTH_TEST);

	CreateShaders();

	// Cámara
	camera = Camera(
		glm::vec3(0.0f, 0.0f, 6.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-90.0f,
		0.0f,
		0.3f,
		0.3f
	);

	// Pyraminx: orden 3 = 9 triángulos por cara
	const int ORDER = 3;
	const float INSET = 0.2f;
	CreatePyraminxMeshes(ORDER, INSET);

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;

	glm::mat4 projection = glm::perspective(
		glm::radians(60.0f),
		(float)mainWindow.getBufferWidth() / (float)mainWindow.getBufferHeight(),
		0.1f,
		100.0f
	);

	glm::mat4 model(1.0f);
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

		glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		glm::mat4 baseModel(1.0f);
		baseModel = glm::translate(baseModel, glm::vec3(0.0f, 0.0f, -2.0f));

		// Rotación manual con teclas
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		baseModel = glm::rotate(baseModel, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

		// Rotación automática suave
		baseModel = glm::rotate(baseModel, now * 0.6f, glm::vec3(0.0f, 1.0f, 0.0f));

		baseModel = glm::scale(baseModel, glm::vec3(1.8f));

		// [0] base negra ligeramente más pequeña para que sobresalgan las caras
		model = glm::scale(baseModel, glm::vec3(0.985f));
		color = COLOR_BLACK;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// [1..4] caras de color
		const glm::vec3 faceColors[4] = {
			COLOR_BLUE,
			COLOR_PINK,
			COLOR_GREEN,
			COLOR_YELLOW
		};

		for (int f = 1; f <= 4; ++f)
		{
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(baseModel));
			glUniform3fv(uniformColor, 1, glm::value_ptr(faceColors[f - 1]));
			meshList[f]->RenderMesh();
		}

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}