#define UNICODE 1
#include <stdio.h>

#include "engine/logging/Log.h"
#include "engine/memory/Arena.h"
#include "engine/render/opengl/GL_fwd.h"

#include "engine/render/Renderer.h"
#include "engine/shader/ShaderGenerator.h"
#include "engine/shader/ast/ShaderStage.h"
#include "engine/shader/parser/Lexer.h"
#include "engine/shader/parser/Parser.h"
#include "platform/IRunLoop.h"
#include "platform/IWindow.h"


int main(int argc, char *argv[])
{
    se::IWindow* window = se::IWindow::CreatePlatformWindow(1280, 720);
    se::IRunLoop* runLoop = se::IRunLoop::CreatePlatformRunloop({ window });
    auto renderer = se::render::Renderer::Create();

    std::string vert;
    {
        se::shader::parser::Lexer lexer("../builtin_assets/shader.vert");
        se::shader::parser::Parser parser(lexer);
        auto result = parser.Parse();
        if (std::holds_alternative<se::shader::parser::ParseError>(result))
        {
            auto parseError = std::get<se::shader::parser::ParseError>(result);
            se::logging::Log::Error("Shader Compile Error - %s - line:%u pos:%u: %s", "shader.vert", parseError.line, parseError.pos, parseError.error.c_str());
            return -1;
        }
        vert = se::shader::ShaderGenerator::AstToGlsl(std::get<se::shader::ast::ShaderStage>(result));
    }

    std::string frag;
    {
        se::shader::parser::Lexer lexerFrag("../builtin_assets/shader.frag");
        se::shader::parser::Parser parserFrag(lexerFrag);
        auto result = parserFrag.Parse();
        if (std::holds_alternative<se::shader::parser::ParseError>(result))
        {
            auto parseError = std::get<se::shader::parser::ParseError>(result);
            se::logging::Log::Error("Shader Compile Error - %s - line:%u pos:%u: %s", "shader.frag", parseError.line, parseError.pos, parseError.error.c_str());
            return -1;
        }
        frag = se::shader::ShaderGenerator::AstToGlsl(std::get<se::shader::ast::ShaderStage>(result));
    }

    GLuint VertexArrayID = {};
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
     };

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    char const * VertexSourcePointer = vert.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    GLint Result = GL_FALSE;
    int InfoLogLength;
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    char const * FragmentSourcePointer = frag.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);


    while (true)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(ProgramID);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );

        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

        glDisableVertexAttribArray(0);


        glFlush();

        runLoop->Tick();
    }

    delete window;
    delete runLoop;
}
