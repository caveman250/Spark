#include "GizmoManager.h"

#include "Transactions.h"
#include "components/RotationGizmoComponent.h"
#include "components/TransformGizmoComponent.h"
#include "engine/ecs/ecs_fwd.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/geo/util/MeshUtil.h"

namespace se::editor
{
    void GizmoManager::CreateGizmo()
    {
        switch (m_GizmoType)
        {
            case GizmoType::Translate:
                CreateTranslateGizmo();
                break;
            case GizmoType::Rotate:
                CreateRotationGizmo();
                break;
            default:
                SPARK_ASSERT(false, "GizmoManager::CreateGizmo - Unsupported gizmo type.");
        }

        SnapGizmoToSelectedEntity();
    }

    void GizmoManager::DestroyGizmo()
    {
        if (m_Gizmo != ecs::InvalidEntity)
        {
            Application::Get()->GetWorld()->DestroyEntity(m_Gizmo);
            m_Gizmo = ecs::InvalidEntity;
            m_GizmoAxisEntities.clear();
        }
    }

    void GizmoManager::SetGizmoType(const GizmoType type)
    {
        if (m_Gizmo != ecs::InvalidEntity && m_GizmoType != type)
        {
            DestroyGizmo();
        }

        m_GizmoType = type;

        const auto* editor = Application::Get()->GetEditor();
        if (!editor->InGameMode())
        {
            if (m_Gizmo == ecs::InvalidEntity)
            {
                CreateGizmo();
            }
            else
            {
                SnapGizmoToSelectedEntity();
            }
        }
    }

    void GizmoManager::OnSelectEntity(const ecs::Id& entity)
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        const auto* editor = app->GetEditor();

        if (entity != ecs::InvalidEntity && world->HasComponent<ecs::components::TransformComponent>(entity))
        {
            const auto selectedEntityTransform = world->GetComponent<ecs::components::TransformComponent>(editor->GetSelectedEntity());
            m_SelectedEntityInitialRotation = selectedEntityTransform->rot;
            m_SelectedEntityInitialPosition = selectedEntityTransform->pos;

            if (m_Gizmo == ecs::InvalidEntity)
            {
                CreateGizmo();
            }
            else
            {
                SnapGizmoToSelectedEntity();
            }
        }
        else if (m_Gizmo != ecs::InvalidEntity)
        {
            HideGizmo();
        }
    }

    void GizmoManager::CreateTranslateGizmo()
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        const auto* editor = app->GetEditor();

        m_Gizmo = world->CreateEntity(editor->GetEditorScene(), "Translate Gizmo");
        world->AddComponent<ecs::components::TransformComponent>(m_Gizmo);

        CreateTranslateGizmoAxis(components::GizmoAxis::Forward);
        CreateTranslateGizmoAxis(components::GizmoAxis::Right);
        CreateTranslateGizmoAxis(components::GizmoAxis::Up);
    }

    void GizmoManager::CreateTranslateGizmoAxis(components::GizmoAxis axis)
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        const auto* editor = app->GetEditor();

        const auto entity = world->CreateEntity(editor->GetEditorScene(), std::format("Translate Gizmo {}", reflect::EnumResolver<components::GizmoAxis>::Get()->ToString(axis)));
        const auto meshComp = world->AddComponent<ecs::components::MeshComponent>(entity);
        meshComp->materialAsset = "/engine_assets/materials/gizmo.sass";
        meshComp->model = "/engine_assets/models/translate_handle.sass";
        meshComp->renderLayer = -1;
        const auto transform = world->AddComponent<ecs::components::TransformComponent>(entity);
        const auto gizmo = world->AddComponent<components::TransformGizmoComponent>(entity);
        switch (axis)
        {
            case components::GizmoAxis::Forward:
                gizmo->color = math::Vec4(0.f, 0.f, 1.f, .6f);
                transform->pos.z = .5f;
                transform->scale = .5f;
                break;
            case components::GizmoAxis::Right:
                gizmo->color = math::Vec4(1.f, 0.f, 0.f, .6f);
                transform->rot.y = 90;
                transform->pos.x = .5f;
                transform->scale = .5f;
                break;
            case components::GizmoAxis::Up:
                gizmo->color = math::Vec4(0.f, 1.f, 0.f, .6f);
                transform->rot.x = 270;
                transform->pos.y = .5f;
                transform->scale = .5f;
                break;
        }
        gizmo->axis = axis;
        gizmo->onMove.Subscribe([this, world, axis](math::Vec3 worldPos)
        {
            const auto transform = world->GetComponent<ecs::components::TransformComponent>(m_Gizmo);

            switch (axis)
            {
                case components::GizmoAxis::Forward:
                    worldPos.x = transform->pos.x;
                    worldPos.y = transform->pos.y;
                    worldPos.z -= .5f;
                    break;
                case components::GizmoAxis::Right:
                    worldPos.y = transform->pos.y;
                   worldPos.z = transform->pos.z;
                   worldPos.x -= .5f;
                    break;
                case components::GizmoAxis::Up:
                    worldPos.x = transform->pos.x;
                    worldPos.y -= .5f;
                    worldPos.z = transform->pos.z;
                    break;
            }

            UpdateSelectedEntityTranslation(worldPos);
        });

        gizmo->onFinishMove.Subscribe([this, world, editor]()
        {
            const auto transform = world->GetComponent<ecs::components::TransformComponent>(m_Gizmo);
            auto* transactions = Transactions::Get();

            transactions->PushAction([pos = transform->pos, world, editor, this]()
            {
                auto* transform = world->GetComponent<ecs::components::TransformComponent>(editor->GetSelectedEntity());
                transform->pos = pos;
                SnapGizmoToSelectedEntity();
                m_SelectedEntityInitialPosition = transform->pos;
            },
            [pos = m_SelectedEntityInitialPosition, world, editor, this]()
            {
                auto* transform = world->GetComponent<ecs::components::TransformComponent>(editor->GetSelectedEntity());
                transform->pos = pos;
                SnapGizmoToSelectedEntity();
                m_SelectedEntityInitialPosition = transform->pos;
            });
        });

        world->AddChild(m_Gizmo, entity);
    }

    void GizmoManager::CreateRotationGizmo()
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        const auto* editor = app->GetEditor();

        m_Gizmo = world->CreateEntity(editor->GetEditorScene(), "Rotation Gizmo");
        world->AddComponent<ecs::components::TransformComponent>(m_Gizmo);

        const asset::StaticMesh quarterMesh = geo::util::CreateCircleMesh(1.f, 0.7f, 24, 0, 6);
        const asset::StaticMesh fullMesh = geo::util::CreateCircleMesh(1.f, 0.7f, 24);

        CreateRotationGizmoAxis(components::RotationAxis::X, quarterMesh, fullMesh);
        CreateRotationGizmoAxis(components::RotationAxis::Y, quarterMesh, fullMesh);
        CreateRotationGizmoAxis(components::RotationAxis::Z, quarterMesh, fullMesh);
    }

    void GizmoManager::CreateRotationGizmoAxis(components::RotationAxis axis,
                                               const asset::StaticMesh& quarterMesh,
                                               const asset::StaticMesh& fullMesh)
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        auto* editor = app->GetEditor();

        const auto entity = world->CreateEntity(editor->GetEditorScene(), std::format("Rotation Gizmo {}", reflect::EnumResolver<components::RotationAxis>::Get()->ToString(axis)));
        const auto mesh = world->AddComponent<ecs::components::MeshComponent>(entity);
        mesh->renderLayer = -1;
        const auto transform = world->AddComponent<ecs::components::TransformComponent>(entity);
        const auto gizmo = world->AddComponent<components::RotationGizmoComponent>(entity);
        switch (axis)
        {
            case components::RotationAxis::X:
                gizmo->color = math::Vec4(1.f, 0.f, 0.f, .6f);
                gizmo->selectedColor = math::Vec4(1.f, 0.f, 0.f, .3f);
                transform->rot.y = -90;
                mesh->materialAsset = "/engine_assets/materials/rotation_gizmo_x.sass";
                break;
            case components::RotationAxis::Y:
                gizmo->color = math::Vec4(0.f, 1.f, 0.f, .6f);
                gizmo->selectedColor = math::Vec4(0.f, 1.f, 0.f, .3f);
                transform->rot.x = 90;
                mesh->materialAsset = "/engine_assets/materials/rotation_gizmo_y.sass";
                break;
            case components::RotationAxis::Z:
                gizmo->color = math::Vec4(0.f, 0.f, 1.f, .6f);
                gizmo->selectedColor = math::Vec4(0.f, 0.f, 1.f, .3f);
                mesh->materialAsset = "/engine_assets/materials/rotation_gizmo_z.sass";
                break;
        }

        gizmo->axis = axis;
        gizmo->onRotate.Subscribe([this, axis](const float angle)
        {
            Transactions::Get()->PushAction([this, axis, angle]()
            {
                UpdateSelectedEntityRotation(axis, angle);
            },
            [this, axis, angle]()
            {
                UpdateSelectedEntityRotation(axis, -angle);
            });
        });
        gizmo->onBeginRotate.Subscribe([this, axis]
        {
            SetHideOtherGizmoAxis(axis, false);
        });
        gizmo->onFinishRotate.Subscribe([this, world, axis, editor]()
        {
            const auto selectedEntityTransform = world->GetComponent<ecs::components::TransformComponent>(editor->GetSelectedEntity());
            m_SelectedEntityInitialRotation = selectedEntityTransform->rot;
            SetHideOtherGizmoAxis(axis, true);
        });

        gizmo->quarterVertBuffer = render::VertexBuffer::CreateVertexBuffer(quarterMesh);
        gizmo->quarterVertBuffer->CreatePlatformResource();
        gizmo->quarterIndexBuffer = render::IndexBuffer::CreateIndexBuffer(quarterMesh);
        gizmo->quarterIndexBuffer->CreatePlatformResource();
        gizmo->quarterAABB = quarterMesh.aabb;

        gizmo->fullVertBuffer = render::VertexBuffer::CreateVertexBuffer(fullMesh);
        gizmo->fullVertBuffer->CreatePlatformResource();
        gizmo->fullIndexBuffer = render::IndexBuffer::CreateIndexBuffer(fullMesh);
        gizmo->fullIndexBuffer->CreatePlatformResource();
        gizmo->fullAABB = fullMesh.aabb;

        mesh->vertexBuffer = gizmo->quarterVertBuffer;
        mesh->indexBuffer = gizmo->quarterIndexBuffer;
        mesh->aabb = gizmo->quarterAABB;

        world->AddChild(m_Gizmo, entity);
        m_GizmoAxisEntities.push_back(entity);
    }

    void GizmoManager::SetHideOtherGizmoAxis(const components::RotationAxis axis,
                                             const bool visible) const
    {
        auto* world = Application::Get()->GetWorld();
        for (const auto& entity: m_GizmoAxisEntities)
        {
            const auto* gizmoComp = world->GetComponent<components::RotationGizmoComponent>(entity);
            auto* meshComp = world->GetComponent<ecs::components::MeshComponent>(entity);

            if (gizmoComp->axis == axis)
            {
                meshComp->visible = true;
                continue;
            }

            meshComp->visible = visible;
        }
    }

    void GizmoManager::UpdateSelectedEntityTranslation(const math::Vec3& worldPos)
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        const auto* editor = app->GetEditor();

        auto* selectedEntityTransform = world->GetComponent<ecs::components::TransformComponent>(editor->GetSelectedEntity());
        if (const ecs::Id& parent = world->GetParent(editor->GetSelectedEntity()); parent != ecs::InvalidEntity)
        {
            const auto* selectedEntityParentTransform = world->GetComponent<ecs::components::TransformComponent>(parent);
            const auto localPos = math::Inverse(selectedEntityParentTransform->worldTransform) * math::Vec4(worldPos, 1);
            selectedEntityTransform->pos = localPos;
        }
        else
        {
            selectedEntityTransform->pos = worldPos;
        }

        SnapGizmoToSelectedEntity();
    }

    void GizmoManager::UpdateSelectedEntityRotation(const components::RotationAxis axis,
                                                    const float angle) const
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        const auto* editor = app->GetEditor();

        const auto selectedEntityTransform = world->GetComponent<ecs::components::TransformComponent>(editor->GetSelectedEntity());

        math::Vec3 vectorAxis;
        switch (axis)
        {
            case components::RotationAxis::X:
                vectorAxis = math::Vec3(1.0f, 0.0f, 0.0f);
                break;
            case components::RotationAxis::Y:
                vectorAxis = math::Vec3(0.0f, 1.0f, 0.0f);
                break;
            case components::RotationAxis::Z:
                vectorAxis = math::Vec3(0.0f, 0.0f, 1.0f);
                break;
        }

        math::Mat4 mat = math::Rotation(m_SelectedEntityInitialRotation);
        const math::Mat4 axisRotationMatrix = math::Rotate(math::Mat4(), angle, vectorAxis);
        mat = axisRotationMatrix * mat;
        selectedEntityTransform->rot = math::EulerFromMat4(mat);
    }

    void GizmoManager::SnapGizmoToSelectedEntity()
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        const auto* editor = app->GetEditor();

        if (editor->GetSelectedEntity() == ecs::InvalidEntity)
        {
            HideGizmo();
            return;
        }

        const auto selectedEntityTransform = world->GetComponent<ecs::components::TransformComponent>(editor->GetSelectedEntity());
        const auto gizmoTransform = world->GetComponent<ecs::components::TransformComponent>(m_Gizmo);
        if (const ecs::Id& parent = world->GetParent(editor->GetSelectedEntity()); parent != ecs::InvalidEntity)
        {
            const auto selectedEntityParentTransform = world->GetComponent<ecs::components::TransformComponent>(parent);
            const auto localPos = selectedEntityParentTransform->worldTransform * math::Vec4(selectedEntityTransform->pos, 1);
            gizmoTransform->pos = localPos;
        }
        else
        {
            gizmoTransform->pos = selectedEntityTransform->pos;
        }
    }

    void GizmoManager::HideGizmo() const
    {
        const auto world = Application::Get()->GetWorld();

        const auto transform = world->GetComponent<ecs::components::TransformComponent>(m_Gizmo);
        transform->pos = math::Vec3(std::numeric_limits<float>::max());
    }
}
