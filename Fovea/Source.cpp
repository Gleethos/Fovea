#define _CRT_SECURE_NO_DEPRECATE

#include <GLEW/glew.h>
#include <iostream>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/GL.h>

GLint compileShader(const char* filename, GLenum type) {

    FILE* file = fopen(filename, "rb");

    if (file == NULL) {
        std::cerr << "Cannot open shader " << filename << std::endl;
        abort();
    }

    fseek(file, 0, SEEK_END);
    const int size = ftell(file);
    rewind(file);

    const GLchar* source = new GLchar[size + 1];
    fread(const_cast<char*>(source), sizeof(char), size, file);
    const_cast<char&>(source[size]) = '\0';

    const GLint shader = glCreateShader(type);

    if (not shader) {
        std::cerr << "Cannot create a shader of type " << shader << std::endl;
        abort();
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    {
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (not compiled) {
            std::cerr << "Cannot compile shader " << filename << std::endl;
            abort();
        }
    }

    return shader;

}

GLint compileShaderProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename) {

    const GLint program = glCreateProgram();

    if (not program) {
        std::cerr << "Cannot create a shader program" << std::endl;
        abort();
    }

    glAttachShader(program, compileShader(vertexShaderFilename, GL_VERTEX_SHADER));
    glAttachShader(program, compileShader(fragmentShaderFilename, GL_FRAGMENT_SHADER));

    glLinkProgram(program);

    {
        GLint linked;
        glGetShaderiv(program, GL_LINK_STATUS, &linked);
        if (not linked) {
            std::cerr << "Cannot link shader program with shaders " << vertexShaderFilename << " and " << fragmentShaderFilename << std::endl;
            abort();
        }
    }

    return program;

}

int main() {

    //if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    //    std::cerr << SDL_GetError() << std::endl;
    //    abort();
    //}

    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //if (SDL_SetVideoMode(800, 600, 32, SDL_OPENGL) == NULL) {
    //    std::cerr << SDL_GetError() << std::endl;
    //    abort();
    //}

    //SDL_WM_SetCaption("sled2", "sled2");

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed" << std::endl;
        abort();
    }
    else if (not GLEW_ARB_shading_language_100 or not GLEW_ARB_vertex_shader or not GLEW_ARB_fragment_shader or not GLEW_ARB_shader_objects) {
        std::cerr << "Shaders not available" << std::endl;
        abort();
    }

    const GLint shader = compileShaderProgram("vertex.glsl", "fragment.glsl");

    bool stop = false;
    while (not stop) {

        //SDL_Event event;
        //while (SDL_PollEvent(&event)) {
        //
        //    switch (event.type) {
        //
        //    case SDL_QUIT:
        //        stop = true;
        //        break;
        //
        //    case SDL_KEYDOWN:
        //        stop = (event.key.keysym.sym == SDLK_ESCAPE);
        //        break;
        //
        //    default:
        //        break;
        //
        //    }
        //
        //}

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_QUADS);
        glVertex2f(-.5, -.5);
        glVertex2f(.5, -.5);
        glVertex2f(.5, .5);
        glVertex2f(-.5, .5);
        glEnd();

        //SDL_GL_SwapBuffers();
        //SDL_Delay(1);

    }

    //SDL_Quit();

    return 0;

}
