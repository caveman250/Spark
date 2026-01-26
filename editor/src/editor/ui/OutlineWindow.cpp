#include "OutlineWindow.h"

#include "editor/EditorRuntime.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/ScrollBoxComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/TreeViewComponent.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/util/ScrollBoxUtil.h"
#include "engine/ui/util/TreeViewUtil.h"
#include "engine/ui/util/WindowUtil.h"
#include "engine/Application.h"

namespace se::editor::ui
{
    void OutlineWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        se::ui::components::RectTransformComponent* windowTransform;
        se::ui::components::WindowComponent* windowComp;
        se::ui::components::TitleBarComponent* titleBarComp;
        ecs::Id childArea;
        ecs::Id titleArea;
        m_Window = ::se::ui::util::CreateWindow(&windowTransform,
            &windowComp,
            &titleBarComp,
            childArea,
            titleArea,
            "Outline",
            editor->GetEditorScene());
        windowTransform->anchors = { 0.f, 0.f, 0.f, 0.f };
        windowTransform->minX = 20;
        windowTransform->maxX = 300;
        windowTransform->minY = 20;
        windowTransform->maxY = 380;

        se::ui::components::ScrollBoxComponent* scrollBox = nullptr;
        se::ui::components::ScrollViewComponent* scrollView = nullptr;
        se::ui::components::RectTransformComponent* scrollBoxTransform = nullptr;
        ecs::Id scrollViewEntity;
        ecs::Id scrollBar;
        auto scrollBoxEntity = ::se::ui::util::CreateScrollBox(&scrollBox, scrollViewEntity, &scrollView, &scrollBoxTransform, scrollBar, editor->GetEditorScene());
        world->AddChild(childArea, scrollBoxEntity);

        se::ui::components::RectTransformComponent* transformComp = nullptr;
        se::ui::components::TreeViewComponent* treeViewComp = nullptr;
        m_TreeViewEntity = ::se::ui::util::CreateTreeView(&treeViewComp, &transformComp, editor->GetEditorScene());
        world->AddChild(scrollViewEntity, m_TreeViewEntity);
        transformComp->anchors = { 0.f, 1.f, 0.f, 0.f };
        transformComp->maxX = 15;
        transformComp->minX = 5;
        transformComp->maxY = 5;

        m_Valid = true;

        RebuildOutline();
    }

    void OutlineWindow::DestroyUI()
    {
        m_Valid = false;
    }

    void OutlineWindow::RebuildOutline() const
    {
        if (!m_Valid)
        {
            return;
        }

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        for (const auto& child : world->GetChildren(m_TreeViewEntity))
        {
            world->DestroyEntity(child);
        }

        auto treeViewRect = world->GetComponent<se::ui::components::RectTransformComponent>(m_TreeViewEntity);

        for (const auto& entity : world->GetRootEntities())
        {
            if (*entity.scene == editor->GetEditorScene() ||
                *entity.scene == world->GetDefaultScene())
            {
                continue;
            }

            AddEntityUI(world, entity, m_TreeViewEntity, treeViewRect);

            if (m_Editor->GetSelectedEntity() == ecs::s_InvalidEntity)
            {
                m_Editor->SelectEntity(entity);
            }
        }

        ecs::Id singletonComponentsNode = {};
        {
            se::ui::components::TreeNodeComponent* treeNodeComp = nullptr;
            se::ui::components::TextComponent* textComp = nullptr;
            std::string name = "Singleton Components";
            singletonComponentsNode = se::ui::util::InsertTreeNode(m_TreeViewEntity, treeViewRect, m_TreeViewEntity, name, &treeNodeComp, &textComp, editor->GetEditorScene());
            textComp->text = name;
        }

        for (const auto& singletonComponent : world->GetSingletonComponents())
        {
            se::ui::components::TreeNodeComponent* treeNodeComp = nullptr;
            se::ui::components::TextComponent* textComp = nullptr;
            se::ui::util::InsertTreeNode(m_TreeViewEntity, treeViewRect, singletonComponentsNode, singletonComponent->GetTypeName(), &treeNodeComp, &textComp, editor->GetEditorScene());
            textComp->text = singletonComponent->GetTypeName();

            std::function<void()> selectedCb = [singletonComponent, this]
            {
                m_Editor->SelectSingletonComponent(singletonComponent);
            };
            treeNodeComp->onSelected.Subscribe(std::move(selectedCb));
        }
    }

    void OutlineWindow::AddEntityUI(ecs::World* world, const ecs::Id& entity, const ecs::Id& parent, se::ui::components::RectTransformComponent* treeViewRect) const
    {
        auto editor = Application::Get()->GetEditorRuntime();
        se::ui::components::TreeNodeComponent* treeNodeComp = nullptr;
        se::ui::components::TextComponent* textComp = nullptr;

        auto treeNode = se::ui::util::InsertTreeNode(m_TreeViewEntity, treeViewRect, parent, *entity.name, &treeNodeComp, &textComp, editor->GetEditorScene());
        textComp->text = *entity.name;

        std::function<void()> selectedCb = [entity, this]()
        {
            m_Editor->SelectEntity(entity);
        };
        treeNodeComp->onSelected.Subscribe(std::move(selectedCb));

        for (const auto& child : world->GetChildren(entity))
        {
            AddEntityUI(world, child, treeNode, treeViewRect);
        }
    }
}
