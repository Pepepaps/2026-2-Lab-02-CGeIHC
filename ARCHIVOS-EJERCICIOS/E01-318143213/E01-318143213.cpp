#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>
//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 			\n\
}";

//Fragment Shader
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(1.0f,1.0f,1.0f,1.0f);	 			\n\
}";

void CrearTriangulo()
{
	GLfloat vertices[] = {
		// --- ROMBO (2 triángulos = 6 vértices) ---
		 0.0f,  0.8f, 0.0f,   // Superior punta
		-0.3f,  0.4f, 0.0f,   // Izquierda
		 0.3f,  0.4f, 0.0f,   // Derecha

		-0.3f,  0.4f, 0.0f,   // Izquierda
		 0.3f,  0.4f, 0.0f,   // Derecha
		 0.0f,  0.0f, 0.0f,   // Inferior punta

		 // --- TRAPECIO ISÓSCELES (4 triángulos = 12 vértices) ---
		 // 1. Triángulo Lateral Izquierdo (La pendiente izq)
		 -0.6f, -0.7f, 0.0f,   // Base externa izq
		 -0.3f, -0.7f, 0.0f,   // Base interna izq
		 -0.3f, -0.2f, 0.0f,   // Punta arriba izq

		 // 2. Triángulo Lateral Derecho (La pendiente der)
		  0.6f, -0.7f, 0.0f,   // Base externa der
		  0.3f, -0.7f, 0.0f,   // Base interna der
		  0.3f, -0.2f, 0.0f,   // Punta arriba der

		  // 3. Triángulo Central 1 (Parte del rectángulo interno)
		  -0.3f, -0.2f, 0.0f,   // Arriba izq
		   0.3f, -0.2f, 0.0f,   // Arriba der
		  -0.3f, -0.7f, 0.0f,   // Abajo izq

		  // 4. Triángulo Central 2 (Completa el rectángulo interno)
		   0.3f, -0.2f, 0.0f,   // Arriba der
		  -0.3f, -0.7f, 0.0f,   // Abajo izq
		   0.3f, -0.7f, 0.0f    // Abajo der
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

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

void CompileShaders() {
	shader = glCreateProgram();
	if (!shader) { printf("Error creando el shader"); return; }
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);
	glLinkProgram(shader);
	glValidateProgram(shader);
}

int main()
{
	if (!glfwInit()) { printf("Falló inicializar GLFW"); return 1; }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Practica FI: 4 Triangulos en Trapecio", NULL, NULL);

	if (!mainWindow) { glfwTerminate(); return 1; }
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);
	glfwMakeContextCurrent(mainWindow);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { glfwDestroyWindow(mainWindow); glfwTerminate(); return 1; }

	glViewport(0, 0, BufferWidth, BufferHeight);
	CrearTriangulo();
	CompileShaders();

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		// Lógica de fondo cíclico
		double tiempo = glfwGetTime();
		int fase = (int)(tiempo / 2.0) % 3;

		if (fase == 0) glClearColor(1.0f, 0.0f, 0.0f, 1.0f);      // Rojo
		else if (fase == 1) glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // Verde
		else glClearColor(0.0f, 0.0f, 1.0f, 1.0f);                // Azul

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(VAO);

		// Dibujamos 18 vértices (6 rombo + 12 del trapecio de 4 triángulos)
		glDrawArrays(GL_TRIANGLES, 0, 18);

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(mainWindow);
	}
	return 0;
}