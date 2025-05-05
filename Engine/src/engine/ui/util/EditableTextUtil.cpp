#include "EditableTextUtil.h"

#include "engine/input/Key.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/TextCaretComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/asset/AssetManager.h"

namespace se::ui::util
{
    ecs::Id CreateEditableText(ecs::World* world,
                               const std::shared_ptr<asset::Font>& font,
                               int fontSize,
                               EditableTextComponent** text)

    {
        auto ret = world->CreateEntity("Label", true);
        (*text) = world->AddComponent<EditableTextComponent>(ret);
        (*text)->font = font;
        (*text)->fontSize = fontSize;
        auto caretEntity = world->CreateEntity("Caret", true);
        auto caretRect = world->AddComponent<RectTransformComponent>(caretEntity);
        caretRect->anchors = { 0.f, 0.f, 0.f, 1.f };
        caretRect->minY = 1;
        caretRect->maxY = 1;
        caretRect->maxX = 2;
        world->AddComponent<WidgetComponent>(caretEntity);
        auto caretComp = world->AddComponent<TextCaretComponent>(caretEntity);
        caretComp->active = false;
        auto caretImage = world->AddComponent<ImageComponent>(caretEntity);
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            auto assetManager = asset::AssetManager::Get();
            auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
            material = render::Material::CreateMaterial({ vert }, { frag });
            material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));
        }
        caretImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        world->AddChild(ret, caretEntity);
        std::function movedCb = [ret, world, caretEntity](int pos)
        {
            auto* editText = world->GetComponent<EditableTextComponent>(ret);
            auto* textRect = world->GetComponent<RectTransformComponent>(ret);
            math::IntVec2 offset = se::ui::util::GetCaretPosition(pos, *editText, *textRect);

            auto* caretRect = world->GetComponent<RectTransformComponent>(caretEntity);
            caretRect->minX = offset.x;
            caretRect->maxX = caretRect->minX + 2;
            caretRect->rect = se::ui::util::CalculateScreenSpaceRect(*caretRect, *textRect);
        };
        (*text)->onCaretMoved.Subscribe(std::move(movedCb));

        return ret;
    }

    math::IntVec2 GetCaretPosition(int pos,
                                   const EditableTextComponent& text,
                                   const RectTransformComponent& rect)
    {
        return MeasureText(rect.rect, text.font, text.fontSize, text.editText, true, true, pos);
    }

    int CalcCaretPosition(const math::Vec2& mousePos,
                          const EditableTextComponent& text,
                          const RectTransformComponent& rect)
    {
        math::Vec2 localMousePos = mousePos - math::Vec2(rect.rect.topLeft);
        return GetCharIndexForPosition(localMousePos, rect.rect, text.font, text.fontSize, text.editText, true, true, text.alignment);
    }

    void BeginEditingText(ecs::System* system,
                          const ecs::Id& entity,
                          EditableTextComponent& textComp,
                          KeyInputComponent& keyInputComp)
    {
        if (!textComp.inEditMode)
        {
            textComp.inEditMode = true;
            textComp.editText = textComp.text;
            SetCaretPos(textComp, 0);
            keyInputComp.keyMask = static_cast<input::Key::Type>(0xFFFFFFFF);
        }

        auto dec = ecs::ChildQueryDeclaration()
                .WithComponent<TextCaretComponent>()
                .WithComponent<WidgetComponent>();
        system->RunChildQuery(entity, dec, [](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* carets = updateData.GetComponentArray<TextCaretComponent>();
            auto* widgets = updateData.GetComponentArray<WidgetComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                carets[i].active = true;
                widgets[i].renderingEnabled = true;
            }

            return true;
        });
    }

    void EndEditingText(ecs::System* system,
                        const ecs::Id& entity,
                        EditableTextComponent& textComp,
                        KeyInputComponent& keyInputComp)
    {
        textComp.inEditMode = false;
        keyInputComp.keyMask = static_cast<input::Key::Type>(0);

        auto dec = ecs::ChildQueryDeclaration()
                .WithComponent<TextCaretComponent>();
        system->RunChildQuery(entity, dec, [](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* carets = updateData.GetComponentArray<TextCaretComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                carets[i].active = false;
            }

            return true;
        });
    }

    void MoveCaret(EditableTextComponent& textComp,
                   int delta)
    {
        SetCaretPos(textComp, textComp.caretPosition + delta);
    }

    void SetCaretPos(components::EditableTextComponent& textComp,
                     int pos)
    {
        int oldPos = textComp.caretPosition;
        textComp.caretPosition = std::clamp<int>(pos, 0, (int)textComp.editText.Size());
        if (textComp.caretPosition != oldPos)
        {
            textComp.onCaretMoved.Broadcast(textComp.caretPosition);
        }
    }
}
