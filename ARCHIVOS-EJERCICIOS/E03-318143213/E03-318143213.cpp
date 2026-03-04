// Práctica 3: Modelado Geométrico y Cámara Sintética.
// Escena Completa: Casa, Ventanas, Puerta y Jardín.
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

// --- Configuración Global ---
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0; // Consistencia con el profesor [cite: 3, 117]

Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader> shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";

Sphere sp = Sphere(1.0, 20, 20);

// --- Funciones de Geometría ---

void CrearCubo() {
    unsigned int cubo_indices[] = {
        0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1,
        7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4,
        4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3
    };
    GLfloat cubo_vertices[] = {
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f
    };
    Mesh* cubo = new Mesh();
    cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
    meshList.push_back(cubo); // meshList[0] 
}

void CrearCilindro(int res, float R) {
    GLfloat dt = 2 * PI / res;
    vector<GLfloat> vertices;
    vector<unsigned int> indices;

    for (int n = 0; n <= res; n++) {
        float x = R * cos(n * dt);
        float z = R * sin(n * dt);
        vertices.push_back(x); vertices.push_back(-0.5f); vertices.push_back(z); // Abajo
        vertices.push_back(x); vertices.push_back(0.5f);  vertices.push_back(z); // Arriba
    }
    unsigned int sideCount = (res + 1) * 2;
    unsigned int bottomStart = sideCount;            
    unsigned int topStart = bottomStart + (res + 1); 
    for (int n = 0; n <= res; n++) {
        float x = R * cos(n * dt);
        float z = R * sin(n * dt);
        vertices.push_back(x); vertices.push_back(-0.5f); vertices.push_back(z); 
    }
    for (int n = 0; n <= res; n++) {
        float x = R * cos(n * dt);
        float z = R * sin(n * dt);
        vertices.push_back(x); vertices.push_back(0.5f); vertices.push_back(z);  
    }

    unsigned int bottomCenter = (unsigned int)(vertices.size() / 3);
    vertices.push_back(0.0f); vertices.push_back(-0.5f); vertices.push_back(0.0f);
    unsigned int topCenter = (unsigned int)(vertices.size() / 3);
    vertices.push_back(0.0f); vertices.push_back(0.5f); vertices.push_back(0.0f);

    for (int n = 0; n < res; n++) {
        unsigned int b0 = 2 * n, t0 = 2 * n + 1, b1 = 2 * (n + 1), t1 = 2 * (n + 1) + 1;
        indices.push_back(b0); indices.push_back(t0); indices.push_back(t1);
        indices.push_back(b0); indices.push_back(t1); indices.push_back(b1);
        indices.push_back(bottomCenter);
        indices.push_back(bottomStart + n + 1);
        indices.push_back(bottomStart + n);

        indices.push_back(topCenter);
        indices.push_back(topStart + n);
        indices.push_back(topStart + n + 1);
    }

    Mesh* cilindro = new Mesh();
    cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
    meshList.push_back(cilindro);
}

void CrearCono(int res, float R) {
    GLfloat dt = 2 * PI / res;
    vector<GLfloat> vertices;
    vector<unsigned int> indices;

        vertices.push_back(0.0f); vertices.push_back(0.5f); vertices.push_back(0.0f);
        for (int n = 0; n <= res; n++) {
        vertices.push_back(R * cos(n * dt));
        vertices.push_back(-0.5f);
        vertices.push_back(R * sin(n * dt));
    }
    unsigned int center = (unsigned int)(vertices.size() / 3);
    vertices.push_back(0.0f); vertices.push_back(-0.5f); vertices.push_back(0.0f);
        for (int n = 0; n < res; n++) {
             indices.push_back(0);
        indices.push_back(1 + n);
        indices.push_back(2 + n);
        indices.push_back(center);
        indices.push_back(2 + n);
        indices.push_back(1 + n);
    }
        Mesh* cono = new Mesh();
    cono->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
    meshList.push_back(cono);
}

void CrearPiramideCuadrangular() {
    vector<unsigned int> indices = { 0,4,3, 3,4,2, 2,4,1, 1,4,0, 0,2,1, 0,3,2 };
    vector<GLfloat> vertices = { 0.5f,-0.5f,0.5f, 0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f, -0.5f,-0.5f,0.5f, 0.0f,0.5f,0.0f };
    Mesh* piramide = new Mesh();
    piramide->CreateMeshGeometry(vertices, indices, 15, 18);
    meshList.push_back(piramide); // meshList[3] [cite: 44]
}

int main() {
    mainWindow = Window(800, 800);
    mainWindow.Initialise();
    glEnable(GL_DEPTH_TEST);
    CrearCubo(); CrearCilindro(250, 1.0f); CrearCono(25, 2.0f); CrearPiramideCuadrangular();
    Shader* s1 = new Shader(); s1->CreateFromFiles(vShader, fShader); shaderList.push_back(*s1);
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.3f);
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
    sp.init(); sp.load();

    while (!mainWindow.getShouldClose()) {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS; 
        lastTime = now;
        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderList[0].useShader();
        GLuint uniModel = shaderList[0].getModelLocation();
        GLuint uniProj = shaderList[0].getProjectLocation();
        GLuint uniView = shaderList[0].getViewLocation();
        GLuint uniCol = shaderList[0].getColorLocation();
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        // --- RENDER ESCENA ---
        // PISO
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.8f, -6.0f));
        model = glm::scale(model, glm::vec3(12.0f, 0.1f, 8.0f));
        glUniform3fv(uniCol, 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[0]->RenderMesh();
        // CUERPO CASA 
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.2f, -6.0f));
        model = glm::scale(model, glm::vec3(2.5f, 4.0f, 2.5f));
        glUniform3fv(uniCol, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[0]->RenderMesh();
        // TECHO 
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, -6.0f));
        model = glm::scale(model, glm::vec3(3.5f, 2.5f, 3.5f));
        glUniform3fv(uniCol, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[3]->RenderMeshGeometry();
        // PUERTA
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.3f, -4.74f));
        model = glm::scale(model, glm::vec3(0.8f, 1.0f, 0.02f));
        glUniform3fv(uniCol, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[0]->RenderMesh();
        // VENTANAS FRONTALES
        glUniform3fv(uniCol, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f))); // Verde
        for (float x : {-0.65f, 0.65f}) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.9f, -4.74f));
            model = glm::scale(model, glm::vec3(0.7f, 0.8f, 0.02f));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            meshList[0]->RenderMesh();
        }
        // VENTANAS LATERALES 
        for (float x : {-1.26f, 1.26f}) {
            for (float z : {-6.5f, -5.5f}) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.9f, z));
                model = glm::scale(model, glm::vec3(0.02f, 0.8f, 0.8f));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                meshList[0]->RenderMesh();
            }
        }
        // VENTANA CIRCULAR TRASERA 
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -7.26f));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.05f));
        glUniform3fv(uniCol, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        sp.render();
        // ÁRBOLES 
        for (float x : {-3.2f, 3.2f}) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(x, -1.2f, -6.0f));
            model = glm::scale(model, glm::vec3(0.5f, 1.2f, 0.5f));
            glUniform3fv(uniCol, 1, glm::value_ptr(glm::vec3(0.35f, 0.25f, 0.1f)));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            meshList[1]->RenderMeshGeometry();
            model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.3f, -6.0f));
            model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.4f));
            glUniform3fv(uniCol, 1, glm::value_ptr(glm::vec3(0.0f, 0.6f, 0.0f)));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            meshList[2]->RenderMeshGeometry();
        }
        mainWindow.swapBuffers();
    }
    return 0;
}