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
                               const int fontSize)

    {
        NewEditableText ret = {};
        const auto app = Application::Get();
        const auto editor = app->GetEditorRuntime();
        const auto assetManager = asset::AssetManager::Get();

        ret.entity = world->CreateEntity(editor->GetEditorScene(), "Label");
        ret.text = world->AddComponent<EditableTextComponent>(ret.entity);
        ret.text->font = font;
        ret.text->fontSize = fontSize;
        ret.mouseInput = world->AddComponent<MouseInputComponent>(ret.entity);
        ret.widget = world->AddComponent<WidgetComponent>(ret.entity);
        auto caretEntity = world->CreateEntity(editor->GetEditorScene(), "Caret");
        const auto caretRect = world->AddComponent<RectTransformComponent>(caretEntity);
        caretRect->anchors = { 0.f, 0.f, 0.f, 1.f };
        caretRect->minY = 1;
        caretRect->maxY = 1;
        caretRect->maxX = 2;
        world->AddComponent<WidgetComponent>(caretEntity);
        const auto caretComp = world->AddComponent<TextCaretComponent>(caretEntity);
        caretComp->active = false;
        const auto caretImage = world->AddComponent<ImageComponent>(caretEntity);
        const auto material = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_lightbg.sass");
        caretImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        world->AddChild(ret.entity, caretEntity);
        std::function movedCb = [ret, world, caretEntity](const int pos)
        {
            const auto* window = Application::Get()->GetWindow();
            const auto* editText = world->GetComponent<EditableTextComponent>(ret.entity);
            const auto* textRect = world->GetComponent<RectTransformComponent>(ret.entity);
            const float offset = GetCaretPosition(pos, *editText);

            auto* caretRect = world->GetComponent<RectTransformComponent>(caretEntity);
            caretRect->minX = offset + editText->renderOffset / window->GetContentScale();
            caretRect->maxX = caretRect->minX + 2;
            caretRect->rect = CalculateScreenSpaceRect(*caretRect, *textRect);
        };
        ret.text->onCaretMoved.Subscribe(std::move(movedCb));

        return ret;
    }

    void SetEditTextMouseInputEnabled(MouseInputComponent* mouseInput,
                                        const bool enabled)
    {
        mouseInput->buttonMask = enabled ? 0xffffffff : 0;
    }

    float GetCaretPosition(const int pos, const EditableTextComponent& text)
    {
        const auto window = Application::Get()->GetWindow();
        const auto& verts = text.vertBuffer->GetVertexStreams().at(render::VertexStreamType::Position);
        const size_t numVerts = verts.data.size() / verts.stride;

        if (pos == static_cast<int>(text.editText.length()))
        {
            // get TR of last char
            const size_t vertex = numVerts - 2;
            const size_t index = vertex * verts.stride;
            SPARK_ASSERT(index < verts.data.size());
            // get x component
            const float ret = verts.data[index];
            return ret / window->GetContentScale();
        }
        else
        {
            // get BL of pos
            constexpr uint8_t numVertsPerChar = 4;
            const size_t vertex = pos * numVertsPerChar;
            const size_t index = vertex * verts.stride;
            SPARK_ASSERT(index < verts.data.size());
            // get x component
            const float ret = verts.data[index];
            return ret / window->GetContentScale();
        }
    }

    int CalcCaretPosition(const math::Vec2& mousePos,
                          const EditableTextComponent& text,
                          const RectTransformComponent& rect)
    {
        const auto window = Application::Get()->GetWindow();
        const math::Vec2 localMousePos = mousePos - math::Vec2(rect.rect.topLeft) - math::Vec2(text.renderOffset, 0.f);
        return static_cast<int>(GetCharIndexForPosition(localMousePos,
            rect.rect,
            text.font.GetAsset(),
            static_cast<int>(text.fontSize * window->GetContentScale()),
            text.editText,
            true,
            text.wrap,
            text.alignment));
    }

    void BeginEditingText(const ecs::System* system,
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

        const auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<TextCaretComponent>()
                .WithComponent<WidgetComponent>();
        Application::Get()->GetWorld()->ChildEach(entity, system, dec, [](const ecs::SystemUpdateData& updateData)
        {
            auto* caret = updateData.GetComponentArray<TextCaretComponent>();
            auto* widget = updateData.GetComponentArray<WidgetComponent>();

            caret->active = true;
            widget->visibility = Visibility::Visible;

            return true;
        });
    }

    void EndEditingText(ecs::System* system,
                        const ecs::Id& entity,
                        EditableTextComponent& textComp,
                        KeyInputComponent& keyInputComp)
    {
        textComp.inEditMode = false;
        textComp.renderOffset = 0.f;
        keyInputComp.keyMask = static_cast<input::Key>(0);

        const auto dec = ecs::HeirachyQueryDeclaration()
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
                   const int delta)
    {
        SetCaretPos(textComp, textComp.caretPosition + delta);
    }

    void SetCaretPos(EditableTextComponent& textComp,
                     const int pos)
    {
        const int oldPos = textComp.caretPosition;
        textComp.caretPosition = std::clamp<int>(pos, 0, static_cast<int>(textComp.editText.size()));

        if (textComp.caretPosition != oldPos)
        {
            textComp.onCaretMoved.Broadcast(textComp.caretPosition);
        }
    }
#endif
}
