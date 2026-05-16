#pragma once

#include "engine/ecs/ecs_fwd.h"
#include "engine/math/Vec3.h"

namespace se::asset
{
    struct StaticMesh;
}

namespace se::editor
{
    namespace components
    {
        enum class RotationAxis;
    }

    enum class GizmoType
    {
        Translate,
        Rotate
    };

    class GizmoManager
    {
    public:
        void CreateGizmo();
        void DestroyGizmo();
        GizmoType GetGizmoType() const { return m_GizmoType; }
        void SetGizmoType(GizmoType type);
        void OnSelectEntity(const ecs::Id& entity);

    private:
        void CreateTranslateGizmo();
        void CreateRotationGizmo();
        void CreateRotationGizmoAxis(components::RotationAxis axis,
                                        const asset::StaticMesh& quarterMesh,
                                        const asset::StaticMesh& fullMesh);
        void SetHideOtherGizmoAxis(components::RotationAxis axis, bool visible) const;

        void UpdateSelectedEntityTranslation(const math::Vec3& worldPos);
        void UpdateSelectedEntityRotation(components::RotationAxis axis, float angle) const;
        void SnapGizmoToSelectedEntity();
        void HideGizmo() const;

        ecs::Id m_Gizmo = ecs::InvalidEntity;
        GizmoType m_GizmoType = GizmoType::Translate;
        std::vector<ecs::Id> m_GizmoAxisEntities = {};
        math::Vec3 m_SelectedEntityInitialRotation = {};
    };
}
