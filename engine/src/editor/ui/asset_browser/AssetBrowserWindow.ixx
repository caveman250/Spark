module;

#include "editor/ui/ToolWindow.h"
#include "engine/ecs/ecs_fwd.h"


export module AssetBrowserWindow;
import AssetReference;
import FileWidget;
import Font;
import Spark.Math;

namespace se::ecs
{
    class World;
}

namespace se::editor::ui::asset_browser
{
    class AssetBrowserWindow : public ToolWindow
    {
    public:
        explicit AssetBrowserWindow(Editor* editor) : ToolWindow(editor) {}
        void ConstructUI() override;
        void DestroyUI() override;
        void Update() override;

        const ecs::Id& GetWindow() const { return m_Window; }
        const std::string& GetActiveFolder() const { return m_ActiveFolder; }

        void SetActiveFolder(const std::string& activeFolder, bool setSelection);
    private:

        void CreatePathBar(const asset::AssetReference<asset::Font>& font);
        void CreatePathItem(ecs::World* world,
                            const std::string& name,
                            const std::string& path,
                            const asset::AssetReference<asset::Font>& font);

        ecs::Id m_Window = ecs::InvalidEntity;
        ecs::Id m_GridBoxEntity = ecs::InvalidEntity;
        ecs::Id m_GridBG = ecs::InvalidEntity;
        ecs::Id m_PathBarBox = ecs::InvalidEntity;
        std::vector<ecs::Id> m_PathBarItems = {};
        std::string m_ActiveFolder = "/assets";
        std::vector<std::shared_ptr<FileWidget>> m_FileWidgets = {};
    };
}
