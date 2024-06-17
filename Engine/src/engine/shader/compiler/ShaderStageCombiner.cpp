#include "ShaderStageCombiner.h"

#include "spark.h"
#include "engine/shader/ast/EndOfExpressionNode.h"
#include "engine/shader/ast/MainNode.h"

#include "engine/shader/ast/InputAttributeNode.h"
#include "engine/shader/ast/OutputNode.h"
#include "engine/shader/ast/NameGenerator.h"
#include "engine/shader/ast/OutputPortNode.h"
#include "engine/shader/ast/ShaderStage.h"
#include "engine/shader/ast/VariableDeclarationNode.h"
#include "engine/shader/ast/VariableReferenceNode.h"
#include "engine/shader/ast/VertexPositionOutputNode.h"

namespace se::shader::compiler
{
    void FixDuplicateNames(const ast::ShaderStage &left, ast::ShaderStage &right)
    {
        std::map<std::string, std::string> nameMap;
        for (auto *node: left.GetNodes())
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
        for (auto *node: right.GetNodes())
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
                    ast::Type type = scope.m_Variables.at(name);
                    scope.m_Variables.erase(name);
                    scope.m_Variables.insert({newName, type});
                }
            }
        }

        for (const auto &[name, newName]: nameMap)
        {
            if (right.GetGlobalVariables().contains(name))
            {
                ast::Type type = right.GetGlobalVariables().at(name);
                right.GetGlobalVariables().erase(name);
                right.GetGlobalVariables().insert({newName, type});
            }
        }
    }

    std::map<std::string, std::string> RemapOutputsToOthersInputs(ast::ShaderStage &left,
        const ast::ShaderStage &right, memory::Arena &tempStore)
    {
        std::set<const ast::OutputPortNode *> outputPortsToRemove;
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
                    for (auto *node: left.GetNodes())
                    {
                        node->ApplyNameRemapping(renameMap);
                    }

                    // insert local variable declaration for port being removed
                    main.second->m_Children.insert(main.second->m_Children.begin(),
                                                 tempStore.Alloc<ast::VariableDeclarationNode>(newName, port->GetType()));
                    main.second->m_Children.insert(main.second->m_Children.begin() + 1, tempStore.Alloc<ast::EndOfExpressionNode>());

                    // mark for removal
                    outputPortsToRemove.insert(port);

                    break;
                }
            }
        }

        for (auto* port : outputPortsToRemove)
        {
            left.RemoveOutputPort(port->GetName());
        }

        return ret;
    }

    void ReplaceInputPortsWithPreviousStageLocalVars(ast::ShaderStage &left, const std::map<std::string, std::string> &nameMap)
    {
        std::set<const ast::InputPortNode *> inputPortsToRemove;
        for (const auto &[name, port]: left.GetInputPorts())
        {
            if (nameMap.contains(port->GetPortName()))
            {
                // replae all reads in current shader with new name
                std::map<std::string, std::string> renameMap = { { name, nameMap.at(port->GetPortName()) } };
                for (auto *node: left.GetNodes())
                {
                    node->ApplyNameRemapping(renameMap);
                }

                // mark for removal
                inputPortsToRemove.insert(port);
            }
        }

        for (auto* port : inputPortsToRemove)
        {
            left.RemoveInputPort(port->GetName());
        }
    }

    void ConnectPorts(ast::ShaderStage &left, ast::ShaderStage &right, memory::Arena& tempStore)
    {
        auto portRemap = RemapOutputsToOthersInputs(left, right, tempStore);
        ReplaceInputPortsWithPreviousStageLocalVars(right, portRemap);
    }

    void CleanupDuplicatePorts(ast::ShaderStage& left, const ast::ShaderStage& right)
    {
        // inputs
        std::set<const ast::InputPortNode *> inputPortsToRemove;
        for (auto& [name, port] : left.GetInputPorts())
        {
            auto portName = port->GetPortName();
            for (auto& [otherName, otherPort] : right.GetInputPorts())
            {
                auto otherPortName = otherPort->GetPortName();
                if (otherPortName == portName)
                {
                    std::map<std::string, std::string> renameMap = { { name, otherName } };
                    for (auto *node: left.GetNodes())
                    {
                        node->ApplyNameRemapping(renameMap);
                    }

                    // mark for removal
                    inputPortsToRemove.insert(port);
                    break;
                }
            }
        }

        for (auto* port : inputPortsToRemove)
        {
            left.RemoveInputPort(port->GetName());
        }

        // outputs
        std::set<const ast::OutputPortNode *> outputPortsToRemove;
        for (auto& [name, port] : left.GetOutputPorts())
        {
            auto portName = port->GetPortName();
            for (auto& [otherName, otherPort] : right.GetOutputPorts())
            {
                auto otherPortName = otherPort->GetPortName();
                if (otherPortName == portName)
                {
                    std::map<std::string, std::string> renameMap = { { name, otherName } };
                    for (auto *node: left.GetNodes())
                    {
                        node->ApplyNameRemapping(renameMap);
                    }

                    // mark for removal
                    outputPortsToRemove.insert(port);
                    break;
                }
            }
        }

        for (auto* port : outputPortsToRemove)
        {
            left.RemoveOutputPort(port->GetName());
        }
    }

    void CombineLogic(ast::ShaderStage &left, const ast::ShaderStage &right)
    {
        std::pair<uint32_t, ast::ASTNode *> main = left.FindMain();

        std::pair<uint32_t, ast::ASTNode *> otherMain = right.FindMain();
        std::vector<ast::ASTNode *> nodesBeforeMain = {};
        std::vector<ast::ASTNode *> nodesAfterMain = {};
        bool hitMain = false;

        for (auto *node: right.GetNodes())
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

        for (auto *node: nodesBeforeMain)
        {
            left.InsertNode(main.first, node);
            main.first++;
        }

        for (auto *node: otherMain.second->m_Children)
        {
            main.second->m_Children.push_back(node);
        }

        uint32_t offset = main.first + 1u;
        for (auto *node: nodesAfterMain)
        {
            left.InsertNode(offset, node);
            offset++;
        }
    }

    void MergeRemainingPorts(ast::ShaderStage &left, const ast::ShaderStage &right)
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

    void ShaderStageCombiner::ResolveCombinedShaderPorts(ast::ShaderStage& shader, memory::Arena& arena)
    {
        for (auto& [name, port] : shader.GetInputPorts())
        {
            if (port->GetPortName() == "InVertPos")
            {
                shader.AddInput(arena.Alloc<ast::InputAttributeNode>(GetInputLoc(port->GetPortName()), port->GetType(), name));
            }
            else if (port->GetPortName() == "InVertColour")
            {
                shader.AddInput(arena.Alloc<ast::InputAttributeNode>(GetInputLoc(port->GetPortName()), port->GetType(), name));
            }
            else if (port->GetPortName() == "InVertUV")
            {
                shader.AddInput(arena.Alloc<ast::InputAttributeNode>(GetInputLoc(port->GetPortName()), port->GetType(), name));
            }
            else if (port->GetPortName() == "FragVertColour")
            {
                shader.AddInput(arena.Alloc<ast::InputNode>(port->GetType(), "fragmentVertColour"));
                std::map<std::string, std::string> renameMap = { { name, "fragmentVertColour" } };
                for (auto* node : shader.GetNodes())
                {
                    node->ApplyNameRemapping(renameMap);
                }
            }
            else if (port->GetPortName() == "FragVertUV")
            {
                shader.AddInput(arena.Alloc<ast::InputNode>(port->GetType(), "fragmentVertUV"));
                std::map<std::string, std::string> renameMap = { { name, "fragmentVertUV" } };
                for (auto* node : shader.GetNodes())
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
            if (port->GetPortName() == "FinalFragColour")
            {
                shader.AddOutput(arena.Alloc<ast::OutputNode>(port->GetType(), name));
            }
            else if (port->GetPortName() == "FinalVertPos")
            {
                for (auto* node : shader.GetNodes())
                {
                    ForEachChild(node, [port](ast::ASTNode* child)
                    {
                        if (auto* varRefNode = dynamic_cast<ast::VariableReferenceNode*>(child))
                        {
                            if (varRefNode->GetName() == port->GetName())
                            {
                                varRefNode->SetName("gl_Position");
                            }
                        }
                    });
                }
            }
            else if (port->GetPortName() == "FinalVertColour")
            {
                shader.AddOutput(arena.Alloc<ast::OutputNode>(port->GetType(), "fragmentVertColour"));
                // urgh names have to match the frag shader.
                std::map<std::string, std::string> renameMap = { { name, "fragmentVertColour" } };
                for (auto* node : shader.GetNodes())
                {
                    node->ApplyNameRemapping(renameMap);
                }
            }
            else if (port->GetPortName() == "FinalVertUV")
            {
                shader.AddOutput(arena.Alloc<ast::OutputNode>(port->GetType(), "fragmentVertUV"));
                // urgh names have to match the frag shader.
                std::map<std::string, std::string> renameMap = { { name, "fragmentVertUV" } };
                for (auto* node : shader.GetNodes())
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

    void ShaderStageCombiner::ForEachChild(ast::ASTNode* node, std::function<void(ast::ASTNode* node)> func)
    {
        for (auto* child : node->m_Children)
        {
            func(child);
            ForEachChild(child, func);
        }
    }

    void CombineUniforms(ast::ShaderStage& leftCopy, const ast::ShaderStage& rightCopy)
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

    ast::ShaderStage ShaderStageCombiner::Combine(const ast::ShaderStage &left, const ast::ShaderStage &right, memory::Arena &tempStore)
    {
        ast::ShaderStage leftCopy = left;
        ast::ShaderStage rightCopy = right;
        FixDuplicateNames(leftCopy, rightCopy);
        ConnectPorts(leftCopy, rightCopy, tempStore);
        CleanupDuplicatePorts(leftCopy, rightCopy);
        CombineUniforms(leftCopy, rightCopy);
        CombineLogic(leftCopy, rightCopy);
        MergeRemainingPorts(leftCopy, rightCopy);

        return leftCopy;
    }

    uint8_t ShaderStageCombiner::GetInputLoc(const std::string& inputName)
    {
        auto it = std::ranges::find(m_ConsumedInputs, inputName);
        if (it == m_ConsumedInputs.end())
        {
            m_ConsumedInputs.push_back(inputName);
            return static_cast<uint8_t>(m_ConsumedInputs.size()) - 1;
        }
        return static_cast<uint8_t>(it - m_ConsumedInputs.begin());
    }

    uint8_t ShaderStageCombiner::GetOutputLoc(const std::string& outputName)
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
