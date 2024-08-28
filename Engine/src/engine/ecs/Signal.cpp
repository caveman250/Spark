#include "Signal.h"

namespace se::ecs
{
    Signal::~Signal()
    {
        Application::Get()->GetWorld()->OnSignalDestroyed(*this);
    }

    void Signal::Unsubscribe(SignalHandle handle)
    {
        if (m_RegisteredHandles.contains(handle))
        {
            m_RegisteredHandles.erase(handle);
        }
    }

    void Signal::Broadcast() const
    {
        Application::Get()->GetWorld()->AddPendingSignal(*this);
    }

    void Signal::Execute() const
    {
        for (const auto& [id, func] : m_RegisteredHandles)
        {
            func();
        }
    }
}
