#include "OutlineWindow.h"

#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/ScrollBoxComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/TitleBarComponent.h>
#include <engine/ui/components/TreeViewComponent.h>
#include <engine/ui/components/WindowComponent.h>
#include <engine/ui/util/ScrollBoxUtil.h>
#include <engine/ui/util/TreeViewUtil.h>
#include <engine/ui/util/WindowUtil.h>
#include "engine/bits/FlagUtil.h"

#include "engine/Application.h"

namespace se::editor::ui
{
    void OutlineWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();

        se::ui::components::RectTransformComponent* windowTransform;
        se::ui::components::WindowComponent* windowComp;
        se::ui::components::TitleBarComponent* titleBarComp;
        ecs::Id childArea;
        m_Window = ::se::ui::util::CreateWindow(&windowTransform,
            &windowComp,
            &titleBarComp,
            childArea,
            "Outline",
            [this](){ DestroyUI(); },
            true);
        windowTransform->anchors = { 0.f, 0.f, 0.f, 0.f };
        windowTransform->minX = 20;
        windowTransform->maxX = 420;
        windowTransform->minY = 200;
        windowTransform->maxY = 720;

        se::ui::components::ScrollBoxComponent* scrollBox = nullptr;
        se::ui::components::RectTransformComponent* scrollBoxTransform = nullptr;
        auto scrollBoxEntity = ::se::ui::util::CreateScrollBox(&scrollBox, &scrollBoxTransform, true);
        world->AddChild(childArea, scrollBoxEntity);

        se::ui::components::RectTransformComponent* transformComp = nullptr;
        se::ui::components::TreeViewComponent* treeViewComp = nullptr;
        m_TreeViewEntity = ::se::ui::util::CreateTreeView(&treeViewComp, &transformComp, true);
        world->AddChild(scrollBoxEntity, m_TreeViewEntity);
        transformComp->anchors = { 0.f, 1.f, 0.f, 0.f };

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

        auto world = Application::Get()->GetWorld();
        for (const auto& child : world->GetChildren(m_TreeViewEntity))
        {
            world->RemoveChild(m_TreeViewEntity, child);
        }

        for (const auto& entity : world->GetRootEntities())
        {
            auto entityFlags = *world->GetFlags(entity);
            if (bits::GetFlag<ecs::IdFlags>(entityFlags, ecs::IdFlags::Editor))
            {
                continue;
            }

            AddEntityUI(world, entity, m_TreeViewEntity);

            if (m_Editor->GetSelectedEntity() == ecs::s_InvalidEntity)
            {
                m_Editor->SelectEntity(entity);
            }
        }
    }

    void OutlineWindow::AddEntityUI(ecs::World* world, ecs::Id entity, ecs::Id parent) const
    {
        se::ui::components::TreeNodeComponent* treeNodeComp = nullptr;
        se::ui::components::TextComponent* textComp = nullptr;

        auto treeView = world->GetComponent<se::ui::components::TreeViewComponent>(m_TreeViewEntity);
        auto treeNode = se::ui::util::InsertTreeNode(m_TreeViewEntity, treeView, parent, *entity.name, &treeNodeComp, &textComp, true);
        textComp->text = *entity.name;

        for (const auto& child : world->GetChildren(entity))
        {
            AddEntityUI(world, child, treeNode);
        }
    }
}
