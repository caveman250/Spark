#include "PropertyUtil.h"
#include "engine/Application.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/asset/AssetManager.h"

namespace se::editor::ui::properties::util
{
    ecs::Id CreateMissingPropertyEditorText(reflect::Type* type, float leftAnchor, int minY)
    {
        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();
        auto world = app->GetWorld();
        auto textEntity = world->CreateEntity(editor->GetEditorScene(), "MissingPropertyEditorText");
        auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
        text->font = "/engine_assets/fonts/Arial.sass";
        text->fontSize = 14;
        text->text = std::format("Missing Property Editor of type {}.",
                                 type->GetTypeName(nullptr));
        text->wrap = se::ui::text::WrapMode::WordChar;
        auto transform = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        transform->minX = 5;
        transform->maxX = 5;
        transform->minY = static_cast<float>(minY);
        transform->anchors = { leftAnchor, 1.f, 0.f, 0.f };
        world->AddComponent<se::ui::components::WidgetComponent>(textEntity);

        return textEntity;
    }
}