//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew.h>
#include <glfw3.h>

// glm
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// clases
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader> shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";

float angulo = 0.0f;

//================== retomando la idea de como realize el ejercicio de la PRACTICA 1 ==================
static void AddRectColor(std::vector<GLfloat>& v,
    float x1, float y1, float x2, float y2, float z,
    float r, float g, float b)
{
    // Triángulo 1
    v.insert(v.end(), { x1, y1, z, r, g, b });
    v.insert(v.end(), { x2, y1, z, r, g, b });
    v.insert(v.end(), { x2, y2, z, r, g, b });

    // Triángulo 2
    v.insert(v.end(), { x1, y1, z, r, g, b });
    v.insert(v.end(), { x2, y2, z, r, g, b });
    v.insert(v.end(), { x1, y2, z, r, g, b });
}

//================== Letras PUJA  ==================
void CrearLetrasPUJA()
{
    // Proporciones
    float w = 0.30f;  // ancho de letra
    float h = 0.45f;  // alto de letra
    float t = 0.07f;  // grosor del trazo
    float z = 0.0f;   // letras planas en XY; luego las movemos con model

    // Colores por letra (PUJA) 
    glm::vec3 colP(1.0f, 0.2f, 0.2f);   // P rojo
    glm::vec3 colU(0.2f, 1.0f, 0.2f);   // U verde
    glm::vec3 colJ(0.2f, 0.2f, 1.0f);   // J azul
    glm::vec3 colA(1.0f, 1.0f, 0.2f);   // A amarillo

    // ===== LETRA P ===== 
    {
        std::vector<GLfloat> v;
        float x = 0.0f, y = 0.0f;

        AddRectColor(v, x, y, x + t, y + h, z, colP.r, colP.g, colP.b);                           // barra izquierda
        AddRectColor(v, x, y + h - t, x + w, y + h, z, colP.r, colP.g, colP.b);                   // barra superior
        AddRectColor(v, x, y + h * 0.55f, x + w * 0.85f, y + h * 0.55f + t, z, colP.r, colP.g, colP.b); // barra media
        AddRectColor(v, x + w - t, y + h * 0.55f, x + w, y + h, z, colP.r, colP.g, colP.b);      // barra derecha arriba

        MeshColor* P = new MeshColor();
        P->CreateMeshColor(v.data(), (unsigned int)v.size());
        meshColorList.push_back(P);
    }

    // ===== LETRA U =====
    {
        std::vector<GLfloat> v;
        float x = 0.0f, y = 0.0f;

        AddRectColor(v, x, y, x + t, y + h, z, colU.r, colU.g, colU.b);                   // izquierda
        AddRectColor(v, x + w - t, y, x + w, y + h, z, colU.r, colU.g, colU.b);           // derecha
        AddRectColor(v, x, y, x + w, y + t, z, colU.r, colU.g, colU.b);                   // abajo

        MeshColor* U = new MeshColor();
        U->CreateMeshColor(v.data(), (unsigned int)v.size());
        meshColorList.push_back(U);
    }

    // ===== LETRA J ===== 
    {
        std::vector<GLfloat> v;
        float x = 0.0f, y = 0.0f;

        AddRectColor(v, x + w - t, y + t, x + w, y + h, z, colJ.r, colJ.g, colJ.b);                   // vertical derecha
        AddRectColor(v, x + w * 0.15f, y, x + w, y + t, z, colJ.r, colJ.g, colJ.b);                   // base inferior
        AddRectColor(v, x + w * 0.15f, y, x + w * 0.15f + t, y + h * 0.25f, z, colJ.r, colJ.g, colJ.b); // gancho

        MeshColor* J = new MeshColor();
        J->CreateMeshColor(v.data(), (unsigned int)v.size());
        meshColorList.push_back(J);
    }

    // ===== LETRA A =====
    {
        std::vector<GLfloat> v;
        float x = 0.0f, y = 0.0f;

        AddRectColor(v, x, y, x + t, y + h, z, colA.r, colA.g, colA.b);                        // izquierda
        AddRectColor(v, x + w - t, y, x + w, y + h, z, colA.r, colA.g, colA.b);                // derecha
        AddRectColor(v, x, y + h - t, x + w, y + h, z, colA.r, colA.g, colA.b);                // arriba
        AddRectColor(v, x, y + h * 0.5f, x + w, y + h * 0.5f + t, z, colA.r, colA.g, colA.b);  // medio

        MeshColor* A = new MeshColor();
        A->CreateMeshColor(v.data(), (unsigned int)v.size());
        meshColorList.push_back(A);
    }
}

//================== Shaders ==================
void CreateShaders()
{
    Shader* shader1 = new Shader(); // índices 
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);

    Shader* shader2 = new Shader(); // color por vértice (MeshColor) -> letras
    shader2->CreateFromFiles(vShaderColor, fShaderColor);
    shaderList.push_back(*shader2);
}

//================== MAIN ==================
int main()
{
    mainWindow = Window(800, 800);
    mainWindow.Initialise();

    glEnable(GL_DEPTH_TEST);

    CrearLetrasPUJA();  // PUJA en meshColorList[0..3]
    CreateShaders();

    GLuint uniformProjection = 0;
    GLuint uniformModel = 0;

    glm::mat4 projection = glm::perspective(
        glm::radians(60.0f),
        (float)mainWindow.getBufferWidth() / (float)mainWindow.getBufferHeight(),
        0.1f, 100.0f
    );

    glm::mat4 model(1.0f);

    // (diagonal de abajo hacia arriba)
    glm::vec3 pos[4] = {
        glm::vec3(-1.20f, -1.20f, -5.0f), // P
        glm::vec3(-0.40f, -0.40f, -5.0f), // U
        glm::vec3(0.40f,  0.40f, -5.0f), // J
        glm::vec3(1.20f,  1.20f, -5.0f)  // A
    };

    while (!mainWindow.getShouldClose())
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[1].useShader();
        uniformModel = shaderList[1].getModelLocation();
        uniformProjection = shaderList[1].getProjectLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        angulo += 0.0001f;  // rotación lenta

        float scaleL = 2.0f;

        for (int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pos[i]);
            model = glm::rotate(model, angulo, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(scaleL, scaleL, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            meshColorList[i]->RenderMeshColor();
        }

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}