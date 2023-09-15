// Sandbox_2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "SanboxFramework.h" 
#include "glew_glfw.h"

#include <iostream>
#include <stdio.h>
#include <thread>
#include <chrono>
#include "PlayerInput.h"
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Camera.h"
#include "Light.h"
#include "Vertex.h"
#include "Cubemap.h"
#include "Mesh.h"

#include <random>

using namespace std::literals;
using namespace glm;

float WIDTH = 1000.f, HEIGHT = 700.f;

DirectionalLight dl;
mat4 cubeModel, view, projection;

VBO* cubeVBO;
VAO* cubeVAO;
Material cubeMat;
LightShader* lShader;

Cubemap skyBox;
Shader* skyBoxShader;

VBO* planeVBO;
VAO* planeVAO;
EBO* planeEBO;
Shader* planeShader; // water shader

// wind direction
vec2 wind = vec2(10.f, 10.f);

FrameBuffer* InitSpecBuffer, * SpecBuffer, * PingBuffer, * PongBuffer, * DisplacementBuffer, * NormalBuffer, *PingTBuffer, *PongTBuffer;
Shader* PhaseShader, * ButterflyShader, * InitSpecShader, * SpecShader, * NormalShader;
Texture* InitSpecTex, * SpecTex, * NormalTex, *PingTex, *PongTex, *DisplacementTex;
Texture *PingTransformTex, *PongTransformTex;
struct waterVertex
{
    vec3 location;
    vec2 cords;
};

VBO* quadVBO;
VAO* quadVAO;

// comment out
//#define USE_STRIPS


// Grid Dim (Geometry resolution)
#define GRID_DIM 1024.f
// RESOLUTION of textures
#define TEX_RES 1024
int SIZE = 400; // can modify
#define GEOM_SIZE 1500.f

bool isPing;

int vertex_count = GRID_DIM + 1;
std::vector<waterVertex> waterVertices(GRID_DIM * GRID_DIM);
std::vector<uint32_t> waterIndices(6 * (GRID_DIM - 1) * (GRID_DIM - 1));
// generate plane mesh, vertices are related to Grid Dimension and Geometry size
void makeWaterPlane()
{
    float f = 1.f;
    int ind = 0;
    vec2 origin = vec2(-1000, -1000);
    for (int zIndex = 0; zIndex < GRID_DIM; zIndex += 1) {
        for (int xIndex = 0; xIndex < GRID_DIM; xIndex += 1) {
            waterVertices[ind].location.x = ((xIndex * GEOM_SIZE) / (GRID_DIM - 1) + origin[0]) * f;
            waterVertices[ind].location.y = ((0.0));
            waterVertices[ind].location.z = ((zIndex * GEOM_SIZE) / (GRID_DIM - 1) + origin[1]) * f;
            waterVertices[ind].cords.x = (xIndex / (GRID_DIM - 1));
            waterVertices[ind++].cords.y = (zIndex / (GRID_DIM - 1));
        }
    }

    ind = 0;
    for (int zIndex = 0; zIndex < GRID_DIM - 1; zIndex += 1) {
        for (int xIndex = 0; xIndex < GRID_DIM - 1; xIndex += 1) {
            int topLeft = zIndex * GRID_DIM + xIndex,
                topRight = topLeft + 1,
                bottomLeft = topLeft + GRID_DIM,
                bottomRight = bottomLeft + 1;

            waterIndices[ind++] = (topLeft);
            waterIndices[ind++] = (bottomLeft);
            waterIndices[ind++] = (bottomRight);
            waterIndices[ind++] = (bottomRight);
            waterIndices[ind++] = (topRight);
            waterIndices[ind++] = (topLeft);
        }
    }
    
}

// declaration
void InitSpec();

void init()
{
    /*cubeModel = mat4(1.f);
    cubeModel = rotate(cubeModel, radians(30.f), vec3(1.f, 0.f, 0.f));*/
    //view = mat4(1.f); // not used
    //view = translate(view, vec3(0.f, 60.f, 0.f)); // not used

    projection = perspective(radians(45.f), WIDTH / HEIGHT, 1.f, 10000.0f); // 10 000 for FAR

    //float vertices[] = {
    //    // positions          // normals           // texture coords
    //    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    //     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    //     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    //    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    //     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    //     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    //    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    //     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    //     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    //    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    //     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    //     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    //};

    //cubeVAO = new VAO();

    //cubeVBO = new VBO(vertices, sizeof(vertices), false);
    //
    //cubeVAO->AttribPointerAndEnable(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
    //cubeVAO->AttribPointerAndEnable(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
    //cubeVAO->AttribPointerAndEnable(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));

    //cubeMat.diffuseTex.load("container2.png");
    //cubeMat.specularTex.load("container22.png");

    //lShader = new LightShader();

    std::string faces[6] = { "right.jpg",
        "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg" };

    skyBox.load(faces);

    skyBoxShader = new Shader();
    skyBoxShader->recompile(Cubemap::vSkyboxShader, Cubemap::fSkyboxShader);

    // plane
    planeVAO = new VAO();
    //float vertices
    makeWaterPlane();
    planeVBO = new VBO(&waterVertices[0], waterVertices.size() * sizeof(waterVertex), false);
    planeEBO = new EBO(&waterIndices[0], waterIndices.size() * sizeof(uint32_t), false);

    /*planeVBO = new VBO(&waterVertices[0], waterVertices.size() * sizeof(float), false);
    planeEBO = new EBO(&waterIndices[0], waterIndices.size() * sizeof(uint32_t), false);*/

    planeVAO->AttribPointerAndEnable(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // location
    planeVAO->AttribPointerAndEnable(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // texture coord

    //Sandbox::mainCamera.AddMovementInput(vec3(0.f, 0.f, 0.f));
    Sandbox::mainCamera.SetLocation(vec3(-500.f, 750.f, 125.f));
    Sandbox::mainCamera.cameraSpeed = 10.f;
    dl.direction = vec3(-1.0f, 1.0f, 1.0f) * -1.f;
    dl.diffuse = vec3(3.2, 9.6, 12.8);

    //
    // Shaders and Textures

    planeShader = new Shader("WaterShaders\\vWaterShader.glsl", "WaterShaders\\fWaterShader.glsl");

    // TEXTURTES:
    InitSpecTex = new Texture();
    InitSpecTex->loadEmptyR32f(TEX_RES, TEX_RES, GL_NEAREST, GL_REPEAT);
    PongTex = new Texture();
    PongTex->loadEmptyR32f(TEX_RES, TEX_RES, GL_NEAREST, GL_CLAMP_TO_BORDER);
    SpecTex = new Texture();
    SpecTex->loadEmptyRGBA32f(TEX_RES, TEX_RES, GL_NEAREST, GL_CLAMP_TO_BORDER);
    NormalTex = new Texture();
    NormalTex->loadEmptyRGBA32f(TEX_RES, TEX_RES, GL_LINEAR, GL_CLAMP_TO_BORDER);
    DisplacementTex = new Texture();
    DisplacementTex->loadEmptyRGBA32f(TEX_RES, TEX_RES, GL_LINEAR, GL_CLAMP_TO_BORDER);

    PingTransformTex = new Texture();
    PingTransformTex->loadEmptyRGBA32f(TEX_RES, TEX_RES, GL_NEAREST, GL_CLAMP_TO_BORDER);
    PongTransformTex = new Texture();
    PongTransformTex->loadEmptyRGBA32f(TEX_RES, TEX_RES, GL_NEAREST, GL_CLAMP_TO_BORDER);

    isPing = true;

    // RANDOM 
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    int ind = 0;
    auto* phaseArray = new std::vector<float>(TEX_RES * TEX_RES);
    for (int i = 0; i < TEX_RES; i ++) {
        for (int j = 0; j < TEX_RES; j ++) {
            (*phaseArray)[ind++] = dis(gen) * 2.0 * 3.14159265359;
        }
    }
    PingTex = new Texture();
    PingTex->loadR32f(TEX_RES, TEX_RES, phaseArray->data(), GL_NEAREST, GL_CLAMP_TO_BORDER);
    delete phaseArray;

    // buffers
    InitSpecBuffer = new FrameBuffer();
    InitSpecBuffer->AttachTexture(InitSpecTex);
    PingBuffer = new FrameBuffer();
    PingBuffer->AttachTexture(PingTex);
    PongBuffer = new FrameBuffer();
    PongBuffer->AttachTexture(PongTex);
    SpecBuffer = new FrameBuffer();
    SpecBuffer->AttachTexture(SpecTex);
    DisplacementBuffer = new FrameBuffer();
    DisplacementBuffer->AttachTexture(DisplacementTex);
    NormalBuffer = new FrameBuffer();
    NormalBuffer->AttachTexture(NormalTex);
    PingTBuffer = new FrameBuffer();
    PingTBuffer->AttachTexture(PingTransformTex);
    PongTBuffer = new FrameBuffer();
    PongTBuffer->AttachTexture(PongTransformTex);

    // Shaders
    std::string sPath = "WaterShaders\\";
    std::string sPathInput = sPath + "vInput.glsl";
    ButterflyShader = new Shader(sPathInput.c_str(), (sPath + "fButterfly.glsl").c_str());
    InitSpecShader = new Shader(sPathInput.c_str(), (sPath + "fInitSpectrum.glsl").c_str());
    PhaseShader = new Shader(sPathInput.c_str(), (sPath + "fPhase.glsl").c_str());
    SpecShader = new Shader(sPathInput.c_str(), (sPath + "fSpectrum.glsl").c_str());
    NormalShader = new Shader(sPathInput.c_str(), (sPath + "fNormals.glsl").c_str());


#ifndef USE_STRIPS
    // quad 
    quadVAO = new VAO();
    float fff[12] = {
    -1.0, -1.0,
    -1.0, 1.0,
    1.0, -1.0,

    -1.0, 1.0,
    1.0, -1.0,
    1.0, 1.0
    };
    quadVBO = new VBO(fff, 6 * 2 * sizeof(float), false);
    quadVAO->AttribPointerAndEnable(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
#else
    quadVAO = new VAO();
    float fff[12] = {
    -1.0, -1.0,
    -1.0, 1.0,
    1.0, -1.0,

    -1.0, 1.0,
    };
    quadVBO = new VBO(fff, 4 * 2 * sizeof(float), false);
    quadVAO->AttribPointerAndEnable(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
#endif

    InitSpec();
}

#ifndef USE_STRIPS
#define DRAW_QUAD() quadVAO->Bind(); \
                    glDrawArrays(GL_TRIANGLES, 0, 6);
#else
#define DRAW_QUAD() quadVBO->Bind(); \
                    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); \
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif

void Clean()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
}

void InitSpec()
{
    InitSpecBuffer->Bind();
    glViewport(0, 0, TEX_RES, TEX_RES);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);

    InitSpecShader->activate();
    InitSpecShader->setFloat("resolution", (float)TEX_RES);
    InitSpecShader->setVec2("wind", wind);
    InitSpecShader->setFloat("size", (float)SIZE);

    quadVAO->Bind();
    DRAW_QUAD();

    InitSpecBuffer->UnBind();
}

void PingPong(float delta)
{
    PhaseShader->activate();
    if (isPing) 
    {
        PongBuffer->Bind(); glViewport(0, 0, TEX_RES, TEX_RES);
        //Clean();
        PingTex->Bind(0);

        PhaseShader->setFloat("size", (float)SIZE);
        PhaseShader->setFloat("delta", delta);
        PhaseShader->setFloat("resolution", (float)TEX_RES);

        quadVAO->Bind();
        DRAW_QUAD();

        PongBuffer->UnBind();
    }
    else
    {
        PingBuffer->Bind(); glViewport(0, 0, TEX_RES, TEX_RES);
        //Clean();
        PongTex->Bind(0);

        PhaseShader->setFloat("size", (float)SIZE);
        PhaseShader->setFloat("delta", delta);
        PhaseShader->setFloat("resolution", (float)TEX_RES);

        quadVAO->Bind();
        DRAW_QUAD();

        PingBuffer->UnBind();
    }
    isPing = !isPing;
}

float waveFactor = 1.5f;
void Spectrum()
{
    SpecShader->activate();
    SpecBuffer->Bind(); glViewport(0, 0, TEX_RES, TEX_RES);
    //Clean();
    if (isPing)
        PingTex->Bind(0);
    else
        PongTex->Bind(0);

    InitSpecTex->Bind(1);

    SpecShader->setFloat("size", (float)SIZE);
    SpecShader->setFloat("waveFactor", waveFactor);
    SpecShader->setFloat("resolution", (float)TEX_RES);

    quadVAO->Bind();
    DRAW_QUAD();

    SpecBuffer->UnBind();
}

void Butterfly()
{
    int iterations = std::log2(TEX_RES) * 2; // OR use bin shift to move in loop
    //
    for (int i = 0; i < iterations; i++)
    {
        ButterflyShader->activate();

        if (i >= iterations / 2)
        {
            ButterflyShader->setBool("isVertical", true);
        }
        else ButterflyShader->setBool("isVertical", false);

        if (i == 0)
        {
            PingTBuffer->Bind(); glViewport(0, 0, TEX_RES, TEX_RES);
            SpecTex->Bind(0);
        }
        else if (i == iterations - 1)
        {
            DisplacementBuffer->Bind(); glViewport(0, 0, TEX_RES, TEX_RES);
            if (iterations % 2 == 0)
            {
                PingTransformTex->Bind(0);
            }
            else PongTransformTex->Bind(0);
        }
        else if (i % 2 == 1)
        {
            PongTBuffer->Bind(); glViewport(0, 0, TEX_RES, TEX_RES);
            PingTransformTex->Bind(0);
        }
        else
        {
            PingTBuffer->Bind(); glViewport(0, 0, TEX_RES, TEX_RES);
            PongTransformTex->Bind(0);
        }

        ButterflyShader->setFloat("stSize", pow(2, (i % (iterations / 2)) + 1));
        ButterflyShader->setFloat("tSize", (float)TEX_RES);

        quadVAO->Bind();
        DRAW_QUAD();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Normals()
{
    NormalBuffer->Bind(); Clean(); glViewport(0, 0, TEX_RES, TEX_RES);

    NormalShader->activate();
    NormalShader->setFloat("resolution", (float)TEX_RES);
    DisplacementTex->Bind(0);
    NormalShader->setFloat("size", (float)SIZE);

    quadVAO->Bind();
    DRAW_QUAD();

    NormalBuffer->UnBind();
}

void update(float delta)
{
    //Clean();
    //
    // 
    //
    PingPong(delta);
    Spectrum();
    Butterfly();
    Normals();

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST); glEnable(GL_MULTISAMPLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    planeShader->activate();
    planeShader->setMatrix4f("view",Sandbox::mainCamera.GetViewMatrix());
    planeShader->setMatrix4f("projection", projection);

    planeShader->setFloat("size", (float)SIZE);
    planeShader->setFloat("geomSize", GEOM_SIZE);
    std::string tempS;

    dl.SendToShader(planeShader, 0, "dirLight", tempS);
    planeShader->setVec3("cameraLocation", Sandbox::mainCamera.GetLocation());

    //planeShader->setInt("displacementTex", 0);
    DisplacementTex->Bind(0);

    //planeShader->setInt("normalTex", 1);
    NormalTex->Bind(1);

    //skyBox.Bind(2);

    /*Shader::colorShaderMVP.activate();
    Shader::colorShaderMVP.setMatrix4f("model", mat4(1.f));
    Shader::colorShaderMVP.setMatrix4f("view", Sandbox::mainCamera.GetViewMatrix());
    Shader::colorShaderMVP.setMatrix4f("projection", projection);
    Shader::colorShaderMVP.setColor("color", 1.0, 0, 0);*/

    planeVAO->Bind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, waterIndices.size(), GL_UNSIGNED_INT, 0);

    skyBox.drawSkybox(*skyBoxShader, Sandbox::mainCamera.GetViewMatrix(), projection);

    /*quadVAO->Bind();
    Shader::textureShader.activate();
    DisplacementTex->Bind(0);
    DRAW_QUAD();*/

    //vec3 loc = Sandbox::mainCamera.GetLocation();
    //printf("x = %f, y = %f, z = %f \n", loc.x, loc.y, loc.z);
}

void close()
{
    delete lShader;//
    delete cubeVAO;//
    delete cubeVBO;//
    delete planeVAO;
    delete planeVBO;
    
    delete planeShader;

    delete InitSpecShader;
    delete SpecShader;
    delete NormalShader;
    delete PhaseShader;

    delete PingTex; delete PongTex; delete SpecTex, delete InitSpecTex; delete NormalTex;
    delete DisplacementTex;

    delete InitSpecBuffer;
    delete SpecBuffer;
    delete PingBuffer;
    delete PongBuffer;
    delete DisplacementBuffer;
    delete NormalBuffer;
    delete PingTBuffer;
    delete PongTBuffer;
}

void onW(int action, int mods)
{
    if (IS_PRESSED(action) || IS_REPEATING(action))
    {
        vec3 dir = Sandbox::mainCamera.GetFrontVector();
        Sandbox::mainCamera.AddMovementInput(vec3(dir.x, 0.f, dir.z));
    }
}

void onS(int action, int mods)
{
    if (IS_PRESSED(action) || IS_REPEATING(action))
    {
        vec3 dir = Sandbox::mainCamera.GetFrontVector();
        Sandbox::mainCamera.AddMovementInput(vec3(dir.x, 0.f, dir.z) * -1.f);
    }
}

void onA(int action, int mods)
{
    if (IS_PRESSED(action) || IS_REPEATING(action))
    {
        vec3 dir = Sandbox::mainCamera.GetRightVector();
        Sandbox::mainCamera.AddMovementInput(vec3(dir.x, 0.f, dir.z) * -1.f);
    }
}

void onD(int action, int mods)
{
    if (IS_PRESSED(action) || IS_REPEATING(action))
    {
        vec3 dir = Sandbox::mainCamera.GetRightVector();
        Sandbox::mainCamera.AddMovementInput(vec3(dir.x, 0.f, dir.z));
    }
}

void onSpace(int action, int mods)
{
    vec3 dir = vec3(0.f, 1.f, 0.f);
    if (IS_SHIFT(mods))
        Sandbox::mainCamera.AddMovementInput(dir * -1.f);
    else Sandbox::mainCamera.AddMovementInput(dir);
}

void onZ(int action, int mods)
{
    float fwd = 1.f;
    if (IS_SHIFT(mods)) fwd = -1.f;

    Sandbox::mainCamera.AddMovementInput(Sandbox::mainCamera.GetFrontVector() * fwd);

}

float wAngle = 0.f;
void onWindChange(int action, int mods)
{
    if (IS_RELEASED(action)) return;

    IS_SHIFT(mods) ? wAngle -= 5.f : wAngle += 5.f;

    wind = rotate(wind, wAngle);

    InitSpec();
}

void onWaveChange(int action, int mods)
{
    if (IS_RELEASED(action)) return;

    waveFactor = IS_SHIFT(mods) ? clamp(waveFactor - 0.2f, 0.0f, 2.5f) : clamp(waveFactor + 0.2f, 0.0f, 3.5f);
}

void onSizeChange(int action, int mods)
{
    if (IS_RELEASED(action)) return;

    SIZE = IS_SHIFT(mods) ? clamp(SIZE - 1, 150, 1000) : clamp(SIZE + 1, 50, 1000);

    InitSpec();
}

int main()
{
    std::cout << "Sanbox .2\n";

    std::cout << "Komande:\n";
    std::cout << "WASD for movement, Spacebar up, Shift + Spacebar down\n";
    std::cout << "Mouse moves camera\n";
    std::cout << "Z moves camera forward, Shift + Z backward\n";
    std::cout << "1 Wind direction, Shift + 1 undo\n";
    std::cout << "2 Wawe, Shift + 2 Redo\n";

    Sandbox::BindEvents(init, update, close);

    PlayerInput::BindKeyEvent(KEY_(W), onW);
    PlayerInput::BindKeyEvent(KEY_(S), onS);
    PlayerInput::BindKeyEvent(KEY_(A), onA);
    PlayerInput::BindKeyEvent(KEY_(D), onD);
    PlayerInput::BindKeyEvent(KEY_(SPACE), onSpace);
    PlayerInput::BindKeyEvent(KEY_(Z), onZ);
    PlayerInput::BindKeyEvent(KEY_(1), onWindChange);
    PlayerInput::BindKeyEvent(KEY_(2), onWaveChange);
    PlayerInput::BindKeyEvent(KEY_(3), onSizeChange);

    Sandbox::initSandbox(WIDTH, HEIGHT, "WATER");

    Sandbox::updateSandbox(); // while loop

    Sandbox::closeSandbox();   
}
