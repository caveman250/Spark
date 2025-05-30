#pragma once
#include "ast/ASTNode.h"
#include "ast/InputNode.h"
#include "ast/InputPortNode.h"
#include "ast/OutputPortNode.h"
#include "ast/OutputNode.h"
#include "spark.h"
#include "ast/Types.h"
#include "ast/Variable.h"
#include "engine/asset/Asset.h"
#include "engine/reflect/Reflect.h"
#include "engine/string/String.h"

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

        std::map<String, shader::ast::Variable> m_Variables;
    };

    SPARK_ENUM()
    enum class ShaderType
    {
        Vertex,
        Fragment
    };

    class Shader : public Asset
    {
        SPARK_CLASS()

        Shader() = default;
        Shader(const Shader& rhs);
        Shader& operator=(const Shader& rhs);

        bool FindVariable(const String& name, shader::ast::AstType& type) const;
        const std::map<String, std::shared_ptr<shader::ast::InputNode>>& GetInputs() const { return m_Inputs; }
        const std::map<String, std::shared_ptr<shader::ast::OutputNode>>& GetOutputs() const { return m_Outputs; }
        const std::map<String, std::shared_ptr<shader::ast::InputPortNode>>& GetInputPorts() const { return m_InputPorts; }
        const std::map<String, std::shared_ptr<shader::ast::OutputPortNode>>& GetOutputPorts() const { return m_OutputPorts; }
        const std::vector<std::shared_ptr<shader::ast::ASTNode>>& GetNodes() const { return m_AstNodes; }
        std::map<String, shader::ast::Variable>& GetGlobalVariables() { return m_GlobalVariables; }
        const std::map<String, shader::ast::Variable>& GetUniformVariables() const { return m_Uniforms; }
        const std::map<String, shader::ast::Variable>& GetSettingVariables() const { return m_Settings; }
        std::vector<AstScope>& GetScopeStack() { return m_ScopeStack; }

        void AddInputPort(const std::shared_ptr<shader::ast::InputPortNode>& node);
        void AddOutputPort(const std::shared_ptr<shader::ast::OutputPortNode>& node);
        void RemoveInputPort(const String& varName);
        void RemoveOutputPort(const String& varName);
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

        const std::shared_ptr<shader::ast::InputPortNode>& FindInputPort(const String& name) const;
        const std::shared_ptr<shader::ast::InputPortNode>& FindInputPortByPortName(const String& portName) const;
        const std::shared_ptr<shader::ast::OutputPortNode>& FindOutputPort(const String& name) const;
        const std::shared_ptr<shader::ast::OutputPortNode>& FindOutputPortByPortName(const String& portName) const;
        const std::shared_ptr<shader::ast::InputNode>& FindInput(const String& name) const;
        const std::shared_ptr<shader::ast::OutputNode>& FindOutput(const String& name) const;
        bool RecordVariableForScope(const String& name, const shader::ast::Variable& var, String& outError);
        bool AddUniform(const String& name, const shader::ast::Variable& var, String& outError);
        bool HasUniform(const String& name) const;
        bool AddSetting(const String& name, const shader::ast::Variable& var, String& outError);

        void InsertNode(size_t at, const std::shared_ptr<shader::ast::ASTNode>& node);
        bool HasUniform(const String& name, const shader::ast::Variable& type);
        bool HasSetting(const String& name, const shader::ast::Variable& type);

        void SetType(ShaderType type) { m_Type = type; }
        ShaderType GetType() const { return m_Type; }

    private:
        SPARK_MEMBER(Serialized)
        ShaderType m_Type = {};

        SPARK_MEMBER()
        bool m_MainDeclared = false;

        SPARK_MEMBER(Serialized)
        std::map<String, std::shared_ptr<shader::ast::InputPortNode>> m_InputPorts;

        SPARK_MEMBER(Serialized)
        std::map<String, std::shared_ptr<shader::ast::OutputPortNode>> m_OutputPorts;

        SPARK_MEMBER()
        std::map<String, std::shared_ptr<shader::ast::InputNode>> m_Inputs;

        SPARK_MEMBER()
        std::map<String, std::shared_ptr<shader::ast::OutputNode>> m_Outputs;

        SPARK_MEMBER(Serialized)
        std::vector<std::shared_ptr<shader::ast::ASTNode>> m_AstNodes;

        std::vector<AstScope> m_ScopeStack;

        SPARK_MEMBER(Serialized)
        std::map<String, shader::ast::Variable> m_Uniforms;

        SPARK_MEMBER(Serialized)
        std::map<String, shader::ast::Variable> m_GlobalVariables;

        SPARK_MEMBER(Serialized)
        std::map<String, shader::ast::Variable> m_Settings;

        friend class shader::ShaderCompiler;
    };

    template <typename T, typename ... Args>
    std::shared_ptr<T> Shader::AddNode(Args&&... args)
    {
        T t = T(std::forward<Args>(args)...);
        return std::static_pointer_cast<T>(AddNode(&t));
    }
}
