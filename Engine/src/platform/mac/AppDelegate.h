#pragma once

#include "engine/render/metal/MTL_fwd.h"

namespace se
{
    class ViewDelegate : public MTK::ViewDelegate
    {
    public:
        ViewDelegate();
        virtual ~ViewDelegate() override;
        virtual void drawInMTKView( MTK::View* pView ) override;
    };

    class AppDelegate : public NS::ApplicationDelegate
    {
    public:
        ~AppDelegate();

        virtual void applicationWillFinishLaunching( NS::Notification* pNotification ) override;
        virtual void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
        virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override;

    private:
        ViewDelegate* m_ViewDelegate = nullptr;
    };
}
