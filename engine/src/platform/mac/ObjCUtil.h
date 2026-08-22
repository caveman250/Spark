#pragma once

#include "engine/input/MouseButton.h"
#include "engine/render/DrawMode.h"
#include "engine/render/metal/MTL_fwd.h"
#include <map>

namespace se::render
{
    class Material;
    class VertexBuffer;
    class MaterialInstance;
}

namespace se::render::commands
{
    class SubmitUI;
    class SubmitGeo;
}

namespace se::platform::mac
{
    NSWindowPtr GetNativeWindow();
    int GetWindowWidth();
    int GetWindowHeight();
    void OnWindowResize(int width, int height);
    void OnKeyStateChange(uint32_t key, bool modifierFlagsChanged, uint32_t modifierFlags, bool keyDown);
    void OnLeftMouseDown();
    void OnRightMouseDown();
    void OnLeftMouseUp();
    void OnRightMouseUp();
    void OnMouseDown(input::MouseButton button);
    void OnMouseUp(input::MouseButton button);
    void OnMouseScroll(float delta);
    void OnMouseMove(int x, int y);
    void OnMouseDragged(int x, int y);
    void OnRightMouseDragged(int x, int y);
}
