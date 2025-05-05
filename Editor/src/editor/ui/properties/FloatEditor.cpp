#include "FloatEditor.h"

#include <engine/Application.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/ImageComponent.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/EditableTextComponent.h>
#include <engine/ui/components/WidgetComponent.h>
#include "engine/render/MaterialInstance.h"
#include "engine/ui/util/EditableTextUtil.h"
#include "engine/ui/util/MeshUtil.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(float, FloatEditor, float);

    DEFINE_SPARK_CLASS_BEGIN(FloatEditor)
    DEFINE_SPARK_CLASS_END(FloatEditor)

    void FloatEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<float*>(value);
    }

    void FloatEditor::ConstructUI(const String& name, bool constructTitle)
    {
        PropertyEditor::ConstructUI(name, constructTitle);

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        auto bg = world->CreateEntity("Float Editor", true);
        auto bgTransform = world->AddComponent<RectTransformComponent>(bg);
        bgTransform->anchors = { .left = constructTitle ? 0.5f : 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        bgTransform->minY = 2;
        bgTransform->maxY = 34;

        m_RectTransform->maxY = bgTransform->maxY + 2;

        world->AddComponent<WidgetComponent>(bg);
        auto image = world->AddComponent<ImageComponent>(bg);
        auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = render::Material::CreateMaterial({vert}, {frag});
            material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));
        }
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        world->AddChild(m_WidgetId, bg);

        auto innerImageEntity = world->CreateEntity("Border", true);
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        static std::shared_ptr<render::Material> innerMaterial = nullptr;
        if (!innerMaterial)
        {
            innerMaterial = render::Material::CreateMaterial({vert}, {frag});
            innerMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
        }
        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = 2;
        world->AddChild(bg, innerImageEntity);

        EditableTextComponent* editableText = nullptr;
        m_Label = se::ui::util::CreateEditableText(world, ariel, 16, &editableText);
        editableText->text = std::format("{}", *m_Value);
        std::function cb = [this](String newVal)
        {
            float f;
            if (sscanf_s(newVal.Data(), "%f", &f) == 1)
            {
                *m_Value = f;
            }
        };
        editableText->onComitted.Subscribe(std::move(cb));

        auto labelRect = world->AddComponent<RectTransformComponent>(m_Label);
        labelRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        labelRect->minY = 2;
        labelRect->maxY = 2;
        labelRect->minX = 2;
        labelRect->maxX = 2;
        world->AddChild(innerImageEntity, m_Label);
    }

    void FloatEditor::Update()
    {
        if (auto text = Application::Get()->GetWorld()->GetComponent<EditableTextComponent>(m_Label))
        {
            text->text = std::format("{:.2f}", *m_Value);
        }
    }
}
