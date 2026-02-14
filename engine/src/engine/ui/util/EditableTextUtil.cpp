#include "EditableTextUtil.h"

#include "engine/input/Key.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/TextCaretComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/render/Material.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/MouseInputComponent.h"

namespace se::ui::util
{
#if SPARK_EDITOR
    NewEditableText CreateEditableText(ecs::World* world,
                               const asset::AssetReference<asset::Font>& font,
                               int fontSize)

    {
        NewEditableText ret = {};
        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();
        auto assetManager = asset::AssetManager::Get();

        ret.entity = world->CreateEntity(editor->GetEditorScene(), "Label");
        ret.text = world->AddComponent<EditableTextComponent>(ret.entity);
        ret.text->font = font;
        ret.text->fontSize = fontSize;
        ret.mouseInput = world->AddComponent<MouseInputComponent>(ret.entity);
        ret.widget = world->AddComponent<WidgetComponent>(ret.entity);
        auto caretEntity = world->CreateEntity(editor->GetEditorScene(), "Caret");
        auto caretRect = world->AddComponent<RectTransformComponent>(caretEntity);
        caretRect->anchors = { 0.f, 0.f, 0.f, 1.f };
        caretRect->minY = 1;
        caretRect->maxY = 1;
        caretRect->maxX = 2;
        world->AddComponent<WidgetComponent>(caretEntity);
        auto caretComp = world->AddComponent<TextCaretComponent>(caretEntity);
        caretComp->active = false;
        auto caretImage = world->AddComponent<ImageComponent>(caretEntity);
        auto material = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_lightbg.sass");
        caretImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        world->AddChild(ret.entity, caretEntity);
        std::function movedCb = [ret, world, caretEntity](int pos)
        {
            auto* editText = world->GetComponent<EditableTextComponent>(ret.entity);
            auto* textRect = world->GetComponent<RectTransformComponent>(ret.entity);
            math::IntVec2 offset = GetCaretPosition(pos, *editText, *textRect);

            auto* caretRect = world->GetComponent<RectTransformComponent>(caretEntity);
            caretRect->minX = static_cast<float>(offset.x);
            caretRect->maxX = caretRect->minX + 2;
            caretRect->rect = CalculateScreenSpaceRect(*caretRect, *textRect);
        };
        ret.text->onCaretMoved.Subscribe(std::move(movedCb));

        return ret;
    }

    void SetEnabled(MouseInputComponent* mouseInput, bool enabled)
    {
        mouseInput->buttonMask = enabled ? 0xffffffff : 0;
    }

    math::IntVec2 GetCaretPosition(int pos,
                                   const EditableTextComponent& text,
                                   const RectTransformComponent& rect)
    {
        return MeasureText(rect.rect, text.font.GetAsset(), text.fontSize, text.editText, true, text.wrap, pos);
    }

    int CalcCaretPosition(const math::Vec2& mousePos,
                          const EditableTextComponent& text,
                          const RectTransformComponent& rect)
    {
        auto window = Application::Get()->GetWindow();
        math::Vec2 localMousePos = (mousePos - math::Vec2(rect.rect.topLeft)) / window->GetContentScale();
        return GetCharIndexForPosition(localMousePos, rect.rect, text.font.GetAsset(), text.fontSize, text.editText, true, text.wrap, text.alignment);
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
            keyInputComp.keyMask = static_cast<input::Key>(0xFFFFFFFF);
        }

        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<TextCaretComponent>()
                .WithComponent<WidgetComponent>();
        Application::Get()->GetWorld()->ChildEach(entity, system, dec, [](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* carets = updateData.GetComponentArray<TextCaretComponent>();
            auto* widgets = updateData.GetComponentArray<WidgetComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                carets[i].active = true;
                widgets[i].visibility = Visibility::Visible;
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
        keyInputComp.keyMask = static_cast<input::Key>(0);

        auto dec = ecs::HeirachyQueryDeclaration()
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

    void SetCaretPos(EditableTextComponent& textComp,
                     int pos)
    {
        int oldPos = textComp.caretPosition;
        textComp.caretPosition = std::clamp<int>(pos, 0, (int)textComp.editText.size());
        if (textComp.caretPosition != oldPos)
        {
            textComp.onCaretMoved.Broadcast(textComp.caretPosition);
        }
    }
#endif
}
