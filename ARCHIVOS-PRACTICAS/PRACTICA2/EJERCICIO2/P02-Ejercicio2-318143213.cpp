// Práctica 2 - Ejercicio 2: Casa con cubos y pirámides + shaders por color
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
#include "Window.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

float angulo = 0.0f;

// ---------- Crear pirámide y cubo ----------
void CreaPiramide()
{
    unsigned int indices[] = {
        0,1,2,
        1,3,2,
        3,0,2,
        1,0,3
    };

    GLfloat vertices[] = {
        -0.5f, -0.5f,  0.0f,   //0
         0.5f, -0.5f,  0.0f,   //1
         0.0f,  0.5f, -0.25f,  //2
         0.0f, -0.5f, -0.5f    //3
    };

    Mesh* obj = new Mesh();
    obj->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj);
}

void CrearCubo()
{
    unsigned int idx[] = {
        // front
        0, 1, 2,  2, 3, 0,
        // right
        1, 5, 6,  6, 2, 1,
        // back
        7, 6, 5,  5, 4, 7,
        // left
        4, 0, 3,  3, 7, 4,
        // bottom
        4, 5, 1,  1, 0, 4,
        // top
        3, 2, 6,  6, 7, 3
    };

    GLfloat v[] = {
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
    cubo->CreateMesh(v, idx, 24, 36);
    meshList.push_back(cubo);
}

// ---------- Shaders por color  ----------
void CreateShaders()
{
    // 0 rojo
    Shader* rojo = new Shader();
    rojo->CreateFromFiles("shaders/shaderrojo.vert", "shaders/shadercolor.frag");
    shaderList.push_back(*rojo);

    // 1 verde
    Shader* verde = new Shader();
    verde->CreateFromFiles("shaders/shaderverde.vert", "shaders/shadercolor.frag");
    shaderList.push_back(*verde);

    // 2 azul
    Shader* azul = new Shader();
    azul->CreateFromFiles("shaders/shaderazul.vert", "shaders/shadercolor.frag");
    shaderList.push_back(*azul);

    // 3 cafe
    Shader* cafe = new Shader();
    cafe->CreateFromFiles("shaders/shadercafe.vert", "shaders/shadercolor.frag");
    shaderList.push_back(*cafe);

    // 4 verde oscuro
    Shader* verdeOsc = new Shader();
    verdeOsc->CreateFromFiles("shaders/shaderverdeoscuro.vert", "shaders/shadercolor.frag");
    shaderList.push_back(*verdeOsc);
}

static void DrawMeshColor(int shaderIndex, GLuint uniformModel, GLuint uniformProjection,
    const glm::mat4& model, const glm::mat4& projection, Mesh* mesh)
{
    shaderList[shaderIndex].useShader();
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
    mesh->RenderMesh();
}

int main()
{
    mainWindow = Window(800, 800);
    mainWindow.Initialise();

    glEnable(GL_DEPTH_TEST);

    CreaPiramide(); // meshList[0]
    CrearCubo();    // meshList[1]
    CreateShaders();

    glm::mat4 projection = glm::perspective(
        glm::radians(60.0f),
        (float)mainWindow.getBufferWidth() / (float)mainWindow.getBufferHeight(),
        0.1f, 100.0f
    );

    shaderList[0].useShader();
    GLuint uniformModel = shaderList[0].getModelLocation();
    GLuint uniformProjection = shaderList[0].getProjectLocation();

    while (!mainWindow.getShouldClose())
    {
        glfwPollEvents();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // fondo blanco 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rotación lenta para apreciar los cubos y piramides
        angulo += 0.0001f;
        glm::mat4 scene(1.0f);
        scene = glm::translate(scene, glm::vec3(0.0f, 0.0f, -8.0f));
        scene = glm::rotate(scene, angulo, glm::vec3(0.0f, 1.0f, 0.0f));

        // ---------------- CASA ----------------
        // Cuerpo (cubo rojo)
        {
            glm::mat4 model = scene;
            model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
            model = glm::scale(model, glm::vec3(3.0f, 2.8f, 1.2f));
            DrawMeshColor(0, uniformModel, uniformProjection, model, projection, meshList[1]);
        }

        // Techo (pirámide azul)
        {
            glm::mat4 model = scene;
            model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.65f));
            model = glm::scale(model, glm::vec3(3.4f, 1.6f, 2.51f));
            DrawMeshColor(2, uniformModel, uniformProjection, model, projection, meshList[0]);
        }

        // Ventana izquierda (cubo verde)
        {
            glm::mat4 model = scene;
            model = glm::translate(model, glm::vec3(-0.9f, 0.5f, 0.65f));
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.1f));
            DrawMeshColor(1, uniformModel, uniformProjection, model, projection, meshList[1]);
        }

        // Ventana derecha (cubo verde)
        {
            glm::mat4 model = scene;
            model = glm::translate(model, glm::vec3(0.9f, 0.5f, 0.65f));
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.1f));
            DrawMeshColor(1, uniformModel, uniformProjection, model, projection, meshList[1]);
        }

        // Puerta (cubo verde)
        {
            glm::mat4 model = scene;
            model = glm::translate(model, glm::vec3(0.0f, -1.1f, 0.65f));
            model = glm::scale(model, glm::vec3(0.9f, 1.2f, 0.1f));
            DrawMeshColor(1, uniformModel, uniformProjection, model, projection, meshList[1]);
        }

        // ---------------- ÁRBOL IZQUIERDO ----------------
        // Tronco (cubo café)
        {
            glm::mat4 model = scene;
            model = glm::translate(model, glm::vec3(-3.4f, -1.2f, 0.0f));
            model = glm::scale(model, glm::vec3(0.6f, 1.0f, 0.6f));
            DrawMeshColor(3, uniformModel, uniformProjection, model, projection, meshList[1]);
        }

        // Copa (pirámide verde oscuro)
        {
            glm::mat4 model = scene;
            model = glm::translate(model, glm::vec3(-3.4f, 0.1f, 0.3f));
            model = glm::scale(model, glm::vec3(1.6f, 1.6f, 1.2f));
            DrawMeshColor(4, uniformModel, uniformProjection, model, projection, meshList[0]);
        }

        // ---------------- ÁRBOL DERECHO ----------------
        // Tronco (cubo café)
        {
            glm::mat4 model = scene;
            model = glm::translate(model, glm::vec3(3.4f, -1.2f, 0.0f));
            model = glm::scale(model, glm::vec3(0.6f, 1.0f, 0.6f));
            DrawMeshColor(3, uniformModel, uniformProjection, model, projection, meshList[1]);
        }

        // Copa (pirámide verde oscuro)
        {
            glm::mat4 model = scene;
            model = glm::translate(model, glm::vec3(3.4f, 0.1f, 0.3f));
            model = glm::scale(model, glm::vec3(1.6f, 1.6f, 1.2f));
            DrawMeshColor(4, uniformModel, uniformProjection, model, projection, meshList[0]);
        }

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}