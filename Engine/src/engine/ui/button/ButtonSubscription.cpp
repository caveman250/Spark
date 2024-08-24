#include "ButtonSubscription.h"

#include "engine/threads/SpinLockGuard.h"
#include "engine/ui/components/ButtonComponent.h"

namespace se::ui::button
{
    std::shared_ptr<ButtonSubscription> ButtonSubscription::CreateButtonSubscription(components::ButtonComponent &button)
    {
        button.subscriptions.push_back(std::make_shared<ButtonSubscription>());
        return button.subscriptions.back();
    }

    bool ButtonSubscription::Pressed() const
    {
        auto guard = threads::SpinLockGuard(m_Lock);
        return m_Pressed;
    }

    bool ButtonSubscription::Released() const
    {
        auto guard = threads::SpinLockGuard(m_Lock);
        return !m_Pressed && m_WasPressed;
    }

    bool ButtonSubscription::Hovered() const
    {
        auto guard = threads::SpinLockGuard(m_Lock);
        return m_Hovered;
    }

    void ButtonSubscription::UpdateFromButton(const components::ButtonComponent &buttomComp)
    {
        auto guard = threads::SpinLockGuard(m_Lock);
        m_Pressed = buttomComp.pressed;
        m_Hovered = buttomComp.hovered;
        m_WasPressed = buttomComp.lastPressed;
    }
}
