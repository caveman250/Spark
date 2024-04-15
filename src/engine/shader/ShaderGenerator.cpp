#include "ShaderGenerator.h"

#include "ast/ASTNode.h"
#include "ast/InputAttributeNode.h"
#include "ast/OutputNode.h"
#include "ast/ShaderStage.h"
#include "engine/memory/Arena.h"

std::string se::shader::ShaderGenerator::AstToGlsl(const ast::ShaderStage& ast)
{
    memory::Arena arena;
    memory::ArenaAllocator<char> alloc(arena);
    string::ArenaString shader(alloc);
    shader.append("#version 330 core\n"); //TODO this is old right?

    for (const auto& [name, node] : ast.GetInputAttributes())
    {
        node->ToGlsl(shader);
    }

    for (const auto& [name, node] : ast.GetOutputs())
    {
        node->ToGlsl(shader);
    }

    for (const auto* node : ast.GetNodes())
    {
        node->ToGlsl(shader);
    }

    return shader.c_str();
}
