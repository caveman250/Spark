module;

export module Spark.UI.Util.ViewportUtil;
import Spark.Math;
import Spark.Geo.Ray;

namespace se::camera
{
    struct ActiveCameraComponent;
}

namespace se::input
{
    struct InputComponent;
}

namespace se::editor::util
{
    math::IntVec2 ScreenSpaceToGameViewportSpace(int mouseX,
                                                 int mouseY);
    bool PosWithinViewport(int mouseX,
                           int mouseY);

    geo::Ray GetEditorMouseRay(const input::InputComponent* inputComp,
                               const camera::ActiveCameraComponent* cameraComp);
}
