#include "ShaderCombiner.h"

#include "spark.h"
#include "engine/render/VertexBuffer.h"
#include "engine/asset/shader/ast/EndOfExpressionNode.h"
#include "engine/asset/shader/ast/MainNode.h"

#include "engine/asset/shader/ast/InputAttributeNode.h"
#include "engine/asset/shader/ast/OutputNode.h"
#include "engine/asset/shader/ast/NameGenerator.h"
#include "engine/asset/shader/ast/OutputPortNode.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/shader/ast/VariableDeclarationNode.h"
#include "engine/asset/shader/ast/VariableReferenceNode.h"
#include "engine/asset/shader/ast/VertexPositionOutputNode.h"

namespace se::asset::shader::compiler
{
    void FixDuplicateNames(const ast::Shader &left, ast::Shader &right)
    {
        std::map<std::string, std::string> nameMap;
        for (const auto& node: left.GetNodes())
        {
            node->CollectUsedNames(nameMap);
        }
        for (const auto &[name, port]: left.GetInputPorts())
        {
            port->CollectUsedNames(nameMap);
        }
        for (const auto &[name, port]: left.GetOutputPorts())
        {
            port->CollectUsedNames(nameMap);
        }
        for (auto &[name, newName]: nameMap)
        {
            newName = ast::NameGenerator::GetName();
        }
        for (const auto& node: right.GetNodes())
        {
            node->ApplyNameRemapping(nameMap);
        }
        for (const auto &[name, port]: right.GetInputPorts())
        {
            port->ApplyNameRemapping(nameMap);
        }
        for (const auto &[name, port]: right.GetOutputPorts())
        {
            port->ApplyNameRemapping(nameMap);
        }

        for (auto &scope: right.GetScopeStack())
        {
            for (const auto &[name, newName]: nameMap)
            {
                if (scope.m_Variables.contains(name))
                {
                    ast::AstType::Type type = scope.m_Variables.at(name);
                    scope.m_Variables.erase(name);
                    scope.m_Variables.insert({newName, type});
                }
            }
        }

        for (const auto &[name, newName]: nameMap)
        {
            if (right.GetGlobalVariables().contains(name))
            {
                ast::AstType::Type type = right.GetGlobalVariables().at(name);
                right.GetGlobalVariables().erase(name);
                right.GetGlobalVariables().insert({newName, type});
            }
        }
    }

    std::map<std::string, std::string> RemapOutputsToOthersInputs(ast::Shader &left, const ast::Shader &right)
    {
        std::set<std::shared_ptr<ast::OutputPortNode>> outputPortsToRemove;
        std::map<std::string, std::string> ret;
        auto main = left.FindMain();
        for (const auto &[name, port]: left.GetOutputPorts())
        {
            for (const auto &[otherName, otherPort]: right.GetInputPorts())
            {
                if (port->GetPortName() == otherPort->GetPortName())
                {
                    auto newName = ast::NameGenerator::GetName();
                    ret[otherPort->GetPortName()] = newName;

                    // replae all writes in current shader with new name
                    std::map<std::string, std::string> renameMap = { {name, newName} };
                    for (const auto& node: left.GetNodes())
                    {
                        node->ApplyNameRemapping(renameMap);
                    }

                    // insert local variable declaration for port being removed
                    main.second->m_Children.insert(main.second->m_Children.begin(), std::make_shared<ast::VariableDeclarationNode>(newName, port->GetType()));
                    main.second->m_Children.insert(main.second->m_Children.begin() + 1, std::make_shared<ast::EndOfExpressionNode>());

                    // mark for removal
                    outputPortsToRemove.insert(port);

                    break;
                }
            }
        }

        for (const auto& port : outputPortsToRemove)
        {
            left.RemoveOutputPort(port->GetName());
        }

        return ret;
    }

    void ReplaceInputPortsWithPreviousStageLocalVars(ast::Shader &left, const std::map<std::string, std::string> &nameMap)
    {
        std::set<std::shared_ptr<ast::InputPortNode>> inputPortsToRemove;
        for (const auto &[name, port]: left.GetInputPorts())
        {
            if (nameMap.contains(port->GetPortName()))
            {
                // replae all reads in current shader with new name
                std::map<std::string, std::string> renameMap = { { name, nameMap.at(port->GetPortName()) } };
                for (const auto& node: left.GetNodes())
                {
                    node->ApplyNameRemapping(renameMap);
                }

                // mark for removal
                inputPortsToRemove.insert(port);
            }
        }

        for (const auto& port : inputPortsToRemove)
        {
            left.RemoveInputPort(port->GetName());
        }
    }

    void ConnectPorts(ast::Shader &left, ast::Shader &right)
    {
        auto portRemap = RemapOutputsToOthersInputs(left, right);
        ReplaceInputPortsWithPreviousStageLocalVars(right, portRemap);
    }

    void CleanupDuplicatePorts(ast::Shader& left, const ast::Shader& right)
    {
        // inputs
        std::set<std::shared_ptr<ast::InputPortNode>> inputPortsToRemove;
        for (auto& [name, port] : left.GetInputPorts())
        {
            auto portName = port->GetPortName();
            for (auto& [otherName, otherPort] : right.GetInputPorts())
            {
                auto otherPortName = otherPort->GetPortName();
                if (otherPortName == portName)
                {
                    std::map<std::string, std::string> renameMap = { { name, otherName } };
                    for (const auto& node: left.GetNodes())
                    {
                        node->ApplyNameRemapping(renameMap);
                    }

                    // mark for removal
                    inputPortsToRemove.insert(port);
                    break;
                }
            }
        }

        for (const auto& port : inputPortsToRemove)
        {
            left.RemoveInputPort(port->GetName());
        }

        // outputs
        std::set<std::shared_ptr<ast::OutputPortNode>> outputPortsToRemove;
        for (auto& [name, port] : left.GetOutputPorts())
        {
            auto portName = port->GetPortName();
            for (auto& [otherName, otherPort] : right.GetOutputPorts())
            {
                auto otherPortName = otherPort->GetPortName();
                if (otherPortName == portName)
                {
                    std::map<std::string, std::string> renameMap = { { name, otherName } };
                    for (const auto& node: left.GetNodes())
                    {
                        node->ApplyNameRemapping(renameMap);
                    }

                    // mark for removal
                    outputPortsToRemove.insert(port);
                    break;
                }
            }
        }

        for (const auto& port : outputPortsToRemove)
        {
            left.RemoveOutputPort(port->GetName());
        }
    }

    void CombineLogic(ast::Shader &left, const ast::Shader &right)
    {
        auto main = left.FindMain();
        auto otherMain = right.FindMain();
        std::vector<std::shared_ptr<ast::ASTNode>> nodesBeforeMain = {};
        std::vector<std::shared_ptr<ast::ASTNode>> nodesAfterMain = {};
        bool hitMain = false;

        for (const auto& node: right.GetNodes())
        {
            if (node == otherMain.second)
            {
                hitMain = true;
                continue;
            }

            if (!hitMain)
            {
                nodesBeforeMain.push_back(node);
            }
            else
            {
                nodesAfterMain.push_back(node);
            }
        }

        for (const auto& node: nodesBeforeMain)
        {
            left.InsertNode(main.first, node);
            main.first++;
        }

        for (const auto& node: otherMain.second->m_Children)
        {
            main.second->m_Children.push_back(node);
        }

        uint32_t offset = main.first + 1u;
        for (const auto& node: nodesAfterMain)
        {
            left.InsertNode(offset, node);
            offset++;
        }
    }

    void MergeRemainingPorts(ast::Shader &left, const ast::Shader &right)
    {
        for (auto& [name, port] : right.GetInputPorts())
        {
            left.AddInputPort(port);
        }

        for (auto& [name, port] : right.GetOutputPorts())
        {
            left.AddOutputPort(port);
        }
    }

    void ShaderCombiner::ResolveCombinedShaderPorts(ast::Shader& shader)
    {
        for (auto& [name, port] : shader.GetInputPorts())
        {
            if (port->GetPortName().starts_with("Vertex_In"))
            {
                shader.AddInput(std::make_shared<ast::InputAttributeNode>(GetInputLoc(port->GetPortName()), port->GetType(), name));
            }
            else if (port->GetPortName().starts_with("Frag_In"))
            {
                std::string varName = port->GetPortName();
                varName = std::regex_replace(varName, std::regex("Frag_In"), "Vertex_Out");
                shader.AddInput(std::make_shared<ast::InputNode>(port->GetType(), varName));
                std::map<std::string, std::string> renameMap = { { name, varName } };
                for (const auto& node : shader.GetNodes())
                {
                    node->ApplyNameRemapping(renameMap);
                }
            }
            else
            {
                debug::Log::Error("Unhandled unresolved input port! {0}", port->GetPortName());
            }
        }

        for (auto& [name, port] : shader.GetOutputPorts())
        {
            if (port->GetPortName() == "Vertex_OutPos")
            {
                for (const auto& node : shader.GetNodes())
                {
                    ForEachChild(node, [port](const std::shared_ptr<ast::ASTNode>& child)
                    {
                        if (const auto& varRefNode = std::dynamic_pointer_cast<ast::VariableReferenceNode>(child))
                        {
                            if (varRefNode->GetName() == port->GetName())
                            {
                                varRefNode->SetName("gl_Position");
                            }
                        }
                    });
                }
            }
            else if (port->GetPortName().starts_with("Frag_Out"))
            {
                shader.AddOutput(std::make_shared<ast::OutputNode>(port->GetType(), name));
            }
            else if (port->GetPortName().starts_with("Vertex_Out"))
            {
                shader.AddOutput(std::make_shared<ast::OutputNode>(port->GetType(), port->GetPortName()));
                // names have to match the frag shader.
                std::map<std::string, std::string> renameMap = { { name, port->GetPortName() } };
                for (const auto& node : shader.GetNodes())
                {
                    node->ApplyNameRemapping(renameMap);
                }
            }
            else
            {
                debug::Log::Error("Unhandled unresolved output port! {0}", port->GetPortName());
            }
        }
    }

    void ShaderCombiner::ForEachChild(const std::shared_ptr<ast::ASTNode>& node, std::function<void(const std::shared_ptr<ast::ASTNode>& node)> func)
    {
        for (const auto& child : node->m_Children)
        {
            func(child);
            ForEachChild(child, func);
        }
    }

    void CombineUniforms(ast::Shader& leftCopy, const ast::Shader& rightCopy)
    {
        for (const auto& [name, type] : rightCopy.GetUniformVariables())
        {
            if (!leftCopy.HasUniform(name, type))
            {
                std::string outError;
                leftCopy.AddUniform(name, type, outError);
            }
        }
    }

    ShaderCombiner::ShaderCombiner(const render::VertexBuffer& vb)
        : m_VertexBuffer(vb)
    {

    }

    ast::Shader ShaderCombiner::Combine(const ast::Shader &left, const ast::Shader &right)
    {
        ast::Shader leftCopy = left;
        ast::Shader rightCopy = right;
        FixDuplicateNames(leftCopy, rightCopy);
        ConnectPorts(leftCopy, rightCopy);
        CleanupDuplicatePorts(leftCopy, rightCopy);
        CombineUniforms(leftCopy, rightCopy);
        CombineLogic(leftCopy, rightCopy);
        MergeRemainingPorts(leftCopy, rightCopy);

        return leftCopy;
    }

    uint8_t ShaderCombiner::GetInputLoc(const std::string& inputName)
    {
        render::VertexStreamType targetType;
        if (inputName == "Vertex_InPos")
        {
            targetType = render::VertexStreamType::Position;
        }
        else if (inputName == "Vertex_InUV")
        {
            targetType = render::VertexStreamType::UV;
        }
        else if (inputName == "Vertex_InNormal")
        {
            targetType = render::VertexStreamType::Normal;
        }
        else
        {
            SPARK_ASSERT(false);
        }

        return std::distance(std::begin(m_VertexBuffer.GetVertexStreams()), m_VertexBuffer.GetVertexStreams().find(targetType));
    }

    uint8_t ShaderCombiner::GetOutputLoc(const std::string& outputName)
    {
        auto it = std::ranges::find(m_ConsumedOutputs, outputName);
        if (it == m_ConsumedOutputs.end())
        {
            m_ConsumedOutputs.push_back(outputName);
            return static_cast<uint8_t>(m_ConsumedOutputs.size() - 1);
        }
        return static_cast<uint8_t>(it - m_ConsumedOutputs.begin());
    }
}
