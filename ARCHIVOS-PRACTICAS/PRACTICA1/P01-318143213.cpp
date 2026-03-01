#include <stdio.h>
#include <string.h>
#include <time.h>      // <- para srand(time(NULL))
#include <stdlib.h>    // <- para rand(), srand()
#include <glew.h>
#include <glfw3.h>

//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;

GLuint VAO, VBO, shader;
int gVertexCount = 0; // <- cantidad de vertices a dibujar

//====================== SHADERS ======================

//Vertex Shader
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
	gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 		\n\
}";

//Fragment Shader (mismo color para todo)
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(1.0f,1.0f,1.0f,1.0f);	 			\n\
}";

//====================== GEOMETRIA (PUJA) ======================

static void AddRect(GLfloat* data, int& idx,
	float x1, float y1, float x2, float y2, float z = 0.0f)
{
	// Triangulo 1
	data[idx++] = x1; data[idx++] = y1; data[idx++] = z;
	data[idx++] = x2; data[idx++] = y1; data[idx++] = z;
	data[idx++] = x2; data[idx++] = y2; data[idx++] = z;

	// Triangulo 2
	data[idx++] = x1; data[idx++] = y1; data[idx++] = z;
	data[idx++] = x2; data[idx++] = y2; data[idx++] = z;
	data[idx++] = x1; data[idx++] = y2; data[idx++] = z;
}

void CrearTriangulo() // (mismo nombre para apegarse al estilo)
{
	// Rango NDC: x,y en [-1,1]
	float startX = -0.90f;
	float startY = -0.90f;
	float step = 0.45f;  // separacion diagonal
	float w = 0.30f;  // ancho de cada letra
	float h = 0.45f;  // alto de cada letra
	float t = 0.07f;  // grosor de trazo

	// Max aproximado: 14 rectangulos * 6 vertices * 3 floats = 252 floats
	// Por seguridad, se da más espacio
	GLfloat vertices[600];
	int idx = 0;

	// ---- LETRA P ----
	{
		float x = startX + step * 0;
		float y = startY + step * 0;

		// barra izquierda
		AddRect(vertices, idx, x, y, x + t, y + h);

		// barra superior
		AddRect(vertices, idx, x, y + h - t, x + w, y + h);

		// barra media
		AddRect(vertices, idx, x, y + h * 0.55f, x + w * 0.85f, y + h * 0.55f + t);

		// barra derecha (solo arriba, para formar el "hueco" de la P)
		AddRect(vertices, idx, x + w - t, y + h * 0.55f, x + w, y + h);
	}

	// ---- LETRA U ----
	{
		float x = startX + step * 1;
		float y = startY + step * 1;

		// barra izquierda
		AddRect(vertices, idx, x, y, x + t, y + h);

		// barra derecha
		AddRect(vertices, idx, x + w - t, y, x + w, y + h);

		// barra inferior
		AddRect(vertices, idx, x, y, x + w, y + t);
	}

	// ---- LETRA J ---- 
	{
		float x = startX + step * 2;
		float y = startY + step * 2;

		// barra vertical derecha (casi toda la altura)
		AddRect(vertices, idx, x + w - t, y + t, x + w, y + h);

		// base inferior (horizontal hacia la izquierda)
		AddRect(vertices, idx, x + w * 0.15f, y, x + w, y + t);

		// pequeño gancho vertical izquierdo
		AddRect(vertices, idx, x + w * 0.15f, y, x + w * 0.15f + t, y + h * 0.25f);
	}
	// ---- LETRA A ----
	{
		float x = startX + step * 3;
		float y = startY + step * 3;

		// barra izquierda
		AddRect(vertices, idx, x, y, x + t, y + h);

		// barra derecha
		AddRect(vertices, idx, x + w - t, y, x + w, y + h);

		// barra superior
		AddRect(vertices, idx, x, y + h - t, x + w, y + h);

		// barra media
		AddRect(vertices, idx, x, y + h * 0.5f, x + w, y + h * 0.5f + t);
	}

	// Guardamos cantidad de vertices
	gVertexCount = idx / 3; // 3 floats por vertice

	// VAO/VBO 
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, idx * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//====================== SHADER UTILS ======================

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	GLint codeLength[1];
	codeLength[0] = (GLint)strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	shader = glCreateProgram();
	if (!shader) { printf("Error creando el shader"); return; }

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}
}

//====================== MAIN ======================

int main()
{
	// Para que al ejecutar varias veces NO salga la misma secuencia de colores
	srand((unsigned int)time(NULL));

	if (!glfwInit()) { printf("Falló inicializar GLFW"); return 1; }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Practica FI: PUJA + Fondo Random", NULL, NULL);
	if (!mainWindow) { glfwTerminate(); return 1; }

	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);
	glfwMakeContextCurrent(mainWindow);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { glfwDestroyWindow(mainWindow); glfwTerminate(); return 1; }

	glViewport(0, 0, BufferWidth, BufferHeight);

	CrearTriangulo();
	CompileShaders();

	// Color de fondo y control de tiempo (cada 2 segundos)
	float bgR = 0.0f, bgG = 0.0f, bgB = 0.0f;
	double lastChange = -2.0; // para que cambie inmediatamente al iniciar

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		// 1) Fondo random RGB cada 2 segundos
		double tiempo = glfwGetTime();
		if (tiempo - lastChange >= 2.0)
		{
			lastChange = tiempo;

			bgR = (float)rand() / (float)RAND_MAX;
			bgG = (float)rand() / (float)RAND_MAX;
			bgB = (float)rand() / (float)RAND_MAX;
		}

		glClearColor(bgR, bgG, bgB, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 2) Dibujar PUJA con triángulos 
		glUseProgram(shader);
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, gVertexCount);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}