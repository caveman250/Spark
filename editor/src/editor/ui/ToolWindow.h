#pragma once

namespace se::editor
{
    class Editor;
}

namespace se::editor::ui
{
    class ToolWindow 
    {
    public:
        ToolWindow(Editor* editor) : m_Editor(editor) {}
        virtual void Update() {}
        virtual void ConstructUI() = 0;
        virtual void DestroyUI() = 0;
    protected:
        Editor* m_Editor = nullptr;
    };
}
