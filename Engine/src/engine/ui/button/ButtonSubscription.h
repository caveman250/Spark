#pragma once
#include "engine/threads/SpinLock.h"

namespace se::ui::components
{
    struct ButtonComponent;
}

namespace se::ui::button
{
    class ButtonSubscription
    {
    public:
        static std::shared_ptr<ButtonSubscription> CreateButtonSubscription(components::ButtonComponent& button);

        bool Pressed() const;
        bool Released() const;
        bool Hovered() const;

        void UpdateFromButton(const components::ButtonComponent& buttomComp);

    private:
        threads::SpinLock m_Lock;
        bool m_Pressed;
        bool m_WasPressed;
        bool m_Hovered;
    };
}
