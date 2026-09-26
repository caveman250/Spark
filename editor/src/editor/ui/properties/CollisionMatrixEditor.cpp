#include "CollisionMatrixEditor.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/ComboBoxUtil.h"
#include "editor/Transactions.h"
#include "editor/ui/ProjectSettingsWindow.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/TextComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(geo::CollisionMatrix, CollisionMatrixEditor, CollisionMatrix);

    void CollisionMatrixEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = value;
    }

    void CollisionMatrixEditor::ConstructUI(const PropertyEditorParams& params)
    {
       PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditor();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/CascadiaCode.sass");

        auto& collisionSettings = geo::CollisionSettings::Get();
        collisionSettings.matrix.Init(collisionSettings.channels);
        float columnSize = 60.f;
        math::Vec2 size = { 20.f };
        math::Vec2 padding = { 4.f };
        math::Vec2 pos = { padding.x + columnSize, 20.f };

        for (const auto& channel : collisionSettings.channels | std::views::reverse)
        {
            auto label = world->CreateEntity(editor->GetEditorScene(), std::format("Property Title ({})", params.name));
            auto titleText = world->AddComponent<TextComponent>(label);
            titleText->font = "/engine_assets/fonts/CascadiaCode.sass";
            titleText->fontSize = 14;
            titleText->text = channel.ToStringView();
            titleText->wrap = se::ui::text::WrapMode::Char;
            auto labelTransform = world->AddComponent<RectTransformComponent>(label);
            labelTransform->minX = pos.x;
            labelTransform->minY = pos.y;
            labelTransform->maxX = pos.x + columnSize;
            labelTransform->maxY = pos.y + columnSize;
            labelTransform->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f };
            world->AddChild(m_Content, label);
            pos.x += columnSize + padding.x;
        }

        pos.y += padding.y + 20;
        pos.x = padding.x;

        for (size_t i = 0; i < collisionSettings.channels.size(); ++i)
        {
            const auto& channel = collisionSettings.channels[i];

            auto label = world->CreateEntity(editor->GetEditorScene(), std::format("Property Title ({})", params.name));
            auto titleText = world->AddComponent<TextComponent>(label);
            titleText->font = "/engine_assets/fonts/CascadiaCode.sass";
            titleText->fontSize = 14;
            titleText->text = channel.ToStringView();
            titleText->wrap = se::ui::text::WrapMode::Char;
            auto labelTransform = world->AddComponent<RectTransformComponent>(label);
            labelTransform->minX = pos.x;
            labelTransform->minY = pos.y;
            labelTransform->maxX = pos.x + columnSize;
            labelTransform->maxY = pos.y + columnSize;
            labelTransform->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f };
            world->AddChild(m_Content, label);

            pos.x += columnSize + padding.x + size.x / 2;

            for (int j = collisionSettings.channels.size() - 1; j >= 0; j--)
            {
                const auto& otherChannel = collisionSettings.channels[j];
                if (static_cast<size_t>(j) >= i)
                {
                    auto id = CreateCheckBox(pos, size, channel, otherChannel);
                    m_CheckBoxes.push_back(
                        std::make_tuple(id, channel, otherChannel,
                            collisionSettings.matrix.Collides(channel, otherChannel)));
                }

                pos.x += padding.x + columnSize;
            }

            pos.x = padding.x;
            pos.y += padding.y + columnSize;
        }
    }

    void CollisionMatrixEditor::Update()
    {
        auto& collisionSettings = geo::CollisionSettings::Get();
        for (auto& tuple : m_CheckBoxes)
        {
            const ecs::Id& id = std::get<0>(tuple);
            const string::Symbol& channel = std::get<1>(tuple);
            const string::Symbol& otherChannel = std::get<2>(tuple);
            bool& lastValue = std::get<3>(tuple);

            bool value = collisionSettings.matrix.Collides(channel, otherChannel);

            if (value != lastValue)
            {
                const auto& texture = value ? m_CheckedTexture : m_UncheckedTexture;
                if (auto button = Application::Get()->GetWorld()->GetComponent<ButtonComponent>(id))
                {
                    button->image = texture;
                    button->pressedImage = texture;
                    button->hoveredImage = texture;
                }

                if (auto image = Application::Get()->GetWorld()->GetComponent<ImageComponent>(id))
                {
                    image->materialInstance->SetUniform("Texture", 1, &texture);
                }

                lastValue = value;
            }
        }
    }

    ecs::Id CollisionMatrixEditor::CreateCheckBox(const math::Vec2& pos, const math::Vec2& size, const string::Symbol& a, const string::Symbol& b)
    {
        auto* world = Application::Get()->GetWorld();
        auto* editor = Application::Get()->GetEditor();
        auto& collisionSettings = geo::CollisionSettings::Get();

        auto ret = world->CreateEntity(editor->GetEditorScene(), "Bool Editor");
        auto bgTransform = world->AddComponent<RectTransformComponent>(ret);
        bgTransform->minX = pos.x;
        bgTransform->minY = pos.y;
        bgTransform->maxX = pos.x + size.x;
        bgTransform->maxY = pos.y + size.y;
        bgTransform->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<WidgetComponent>(ret);
        world->AddChild(m_Content, ret);

        ecs::Id border = world->CreateEntity(editor->GetEditorScene(), "Border");
        auto button = world->AddComponent<ButtonComponent>(border);
        bool ticked = collisionSettings.matrix.Collides(a, b);
        button->image = ticked ? m_CheckedTexture : m_UncheckedTexture;
        button->pressedImage = ticked ? m_CheckedTexture : m_UncheckedTexture;
        button->hoveredImage = ticked ? m_CheckedTexture : m_UncheckedTexture;
        button->onReleased.Subscribe([&collisionSettings, a, b](input::MouseButton, bool)
        {
            bool oldVal = collisionSettings.matrix.Collides(a, b);
            bool newVal =  !oldVal;
            Transactions::Get()->PushAction([&collisionSettings, newVal, a, b]()
            {
                collisionSettings.matrix.SetCollides(a, b, newVal);
            },
            [&collisionSettings,oldVal, a, b]()
            {
                collisionSettings.matrix.SetCollides(a, b, oldVal);
            });
        });
        auto innerTransform = world->AddComponent<RectTransformComponent>(border);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        world->AddChild(ret, border);

        return border;
    }
}
