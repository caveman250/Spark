#pragma once

#include "engine/render/metal/MTL_fwd.h"

namespace se
{
    class ViewDelegate : public MTK::ViewDelegate
    {
    public:
        ViewDelegate();
        ~ViewDelegate() override;
        void drawInMTKView( MTK::View* pView ) override;
    };

    class AppDelegate : public NS::ApplicationDelegate
    {
    public:
        ~AppDelegate() override;
        void applicationWillFinishLaunching( NS::Notification* pNotification ) override;
        void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
        bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override;

    private:
        ViewDelegate* m_ViewDelegate = nullptr;
    };
}
