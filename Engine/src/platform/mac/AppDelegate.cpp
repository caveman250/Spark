#include "AppDelegate.h"

#include "engine/Application.h"

namespace se
{
    AppDelegate::~AppDelegate()
    {
        delete m_ViewDelegate;
    }

    void AppDelegate::applicationWillFinishLaunching(NS::Notification* pNotification)
    {
        NS::Application* pApp = reinterpret_cast<NS::Application*>(pNotification->object());
        pApp->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
    }

    void AppDelegate::applicationDidFinishLaunching(NS::Notification* pNotification)
    {
        PlatformRunLoop::Get()->Init();

        m_ViewDelegate = new ViewDelegate();
        auto window = static_cast<mac::Window*>(Application::Get()->GetPrimaryWindow());
        window->GetView()->setDelegate(m_ViewDelegate);

        NS::Application* pApp = reinterpret_cast<NS::Application*>(pNotification->object());
        pApp->activateIgnoringOtherApps(true);
    }

    bool AppDelegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application*)
    {
        return true;
    }

    ViewDelegate::ViewDelegate()
        : MTK::ViewDelegate()
    {
    }

    ViewDelegate::~ViewDelegate()
    {
    }

    void ViewDelegate::drawInMTKView(MTK::View* pView)
    {
        PlatformRunLoop::Get()->Update();
    }
}
