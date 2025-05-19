#pragma once

namespace se::editor
{
    class EditorRuntime;
}

namespace se::editor::ui
{
    class ToolWindow 
    {
    public:
        ToolWindow(EditorRuntime* editor) : m_Editor(editor) {}
        virtual void Update() {}
        virtual void ConstructUI() = 0;
        virtual void DestroyUI() = 0;
    protected:
        EditorRuntime* m_Editor = nullptr;
    };
}
