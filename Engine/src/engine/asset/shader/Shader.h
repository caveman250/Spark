#pragma once
#include "ast/InputPortNode.h"
#include "spark.h"
#include "ast/Types.h"
#include "ast/Variable.h"
#include "engine/asset/Asset.h"
#include "engine/reflect/Reflect.h"

namespace se::asset::shader::compiler
{
    class Parser;
}
namespace se::asset::shader::ast
{
    class MainNode;
    class InputNode;
    class OutputNode;
    class InputPortNode;
    class OutputPortNode;
    class ASTNode;
}
namespace se::asset
{
    namespace shader
    {
        class ShaderCompiler;
    }

    class AstScope
    {
    public:
        AstScope() {}
        AstScope(const std::shared_ptr<shader::ast::ASTNode>& node) : m_Node(node) {}
        std::shared_ptr<shader::ast::ASTNode> m_Node;

        std::map<std::string, shader::ast::Variable> m_Variables;
    };

    DECLARE_SPARK_ENUM_BEGIN(ShaderType, int)
        Vertex,
        Fragment
    DECLARE_SPARK_ENUM_END()

    class Shader : public Asset
    {
        DECLARE_SPARK_CLASS(Shader)

        Shader() = default;
        Shader(const Shader& rhs);
        Shader& operator=(const Shader& rhs);

        bool FindVariable(const std::string& name, shader::ast::AstType::Type& type) const;
        const std::map<std::string, std::shared_ptr<shader::ast::InputNode>>& GetInputs() const { return m_Inputs; }
        const std::map<std::string, std::shared_ptr<shader::ast::OutputNode>>& GetOutputs() const { return m_Outputs; }
        const std::map<std::string, std::shared_ptr<shader::ast::InputPortNode>>& GetInputPorts() const { return m_InputPorts; }
        const std::map<std::string, std::shared_ptr<shader::ast::OutputPortNode>>& GetOutputPorts() const { return m_OutputPorts; }
        const std::vector<std::shared_ptr<shader::ast::ASTNode>>& GetNodes() const { return m_AstNodes; }
        std::map<std::string, shader::ast::Variable>& GetGlobalVariables() { return m_GlobalVariables; }
        const std::map<std::string, shader::ast::Variable>& GetUniformVariables() const { return m_Uniforms; }
        const std::map<std::string, shader::ast::Variable>& GetSettingVariables() const { return m_Settings; }
        std::vector<AstScope>& GetScopeStack() { return m_ScopeStack; }

        void AddInputPort(const std::shared_ptr<shader::ast::InputPortNode>& node);
        void AddOutputPort(const std::shared_ptr<shader::ast::OutputPortNode>& node);
        void RemoveInputPort(const std::string& varName);
        void RemoveOutputPort(const std::string& varName);
        void AddInput(const std::shared_ptr<shader::ast::InputNode>& node);
        void AddOutput(const std::shared_ptr<shader::ast::OutputNode>& node);

        template <typename T, typename... Args>
        std::shared_ptr<T> AddNode(Args&&... args);
        std::shared_ptr<shader::ast::ASTNode> AddNode(shader::ast::ASTNode* node);
        void PushScope(const std::shared_ptr<shader::ast::ASTNode>& node);
        void PopScope();
        uint8_t ScopeDepth() { return static_cast<uint8_t>(m_ScopeStack.size()); }
        bool IsMainDeclared() { return m_MainDeclared; }
        bool IsMainCurrentScope();

        std::pair<uint32_t, std::shared_ptr<shader::ast::MainNode>> FindMain() const;

        const std::shared_ptr<shader::ast::InputPortNode>& FindInputPort(const std::string& name) const;
        const std::shared_ptr<shader::ast::InputPortNode>& FindInputPortByPortName(const std::string& portName) const;
        const std::shared_ptr<shader::ast::OutputPortNode>& FindOutputPort(const std::string& name) const;
        const std::shared_ptr<shader::ast::OutputPortNode>& FindOutputPortByPortName(const std::string& portName) const;
        const std::shared_ptr<shader::ast::InputNode>& FindInput(const std::string& name) const;
        const std::shared_ptr<shader::ast::OutputNode>& FindOutput(const std::string& name) const;
        bool RecordVariableForScope(const std::string& name, const shader::ast::Variable& var, std::string& outError);
        bool AddUniform(const std::string& name, const shader::ast::Variable& var, std::string& outError);
        bool AddSetting(const std::string& name, const shader::ast::Variable& var, std::string& outError);

        void InsertNode(size_t at, const std::shared_ptr<shader::ast::ASTNode>& node);
        bool HasUniform(const std::string& name, const shader::ast::Variable& type);
        bool HasSetting(const std::string& name, const shader::ast::Variable& type);

        void SetType(ShaderType::Type type) { m_Type = type; }
        ShaderType::Type GetType() { return m_Type; }

    private:
        ShaderType::Type m_Type = {};
        bool m_MainDeclared = false;
        std::map<std::string, std::shared_ptr<shader::ast::InputPortNode>> m_InputPorts;
        std::map<std::string, std::shared_ptr<shader::ast::OutputPortNode>> m_OutputPorts;
        std::map<std::string, std::shared_ptr<shader::ast::InputNode>> m_Inputs;
        std::map<std::string, std::shared_ptr<shader::ast::OutputNode>> m_Outputs;
        std::vector<std::shared_ptr<shader::ast::ASTNode>> m_AstNodes;
        std::vector<AstScope> m_ScopeStack;
        std::map<std::string, shader::ast::Variable> m_Uniforms;
        std::map<std::string, shader::ast::Variable> m_GlobalVariables;
        std::map<std::string, shader::ast::Variable> m_Settings;

        friend class shader::ShaderCompiler;
    };

    template <typename T, typename ... Args>
    std::shared_ptr<T> Shader::AddNode(Args&&... args)
    {
        T t = T(std::forward<Args>(args)...);
        return std::static_pointer_cast<T>(AddNode(&t));
    }
}
