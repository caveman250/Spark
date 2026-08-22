module Signal;

import Application;
import Containers;

namespace se::ecs
{
    void OnSignalDestroyed(BaseSignal* signal)
    {
        Application::Get()->GetWorld()->OnSignalDestroyed(this);
    }

    void BroadcastSignal(BaseSignal* signal)
    {
        Application::Get()->GetWorld()->AddPendingSignal(this);
    }

    void ExecuteSignal(const SignalHandle& signal,
        const std::vector<std::any>& argVec)
    {
        auto world = Application::Get()->GetWorld();
        const auto reflect = static_cast<reflect::System*>(reflect::TypeFromString(signal.systemName));
        const auto system = world->GetAppSystem(reflect->GetStaticId());
        system->Invoke(signal.functionName, arg_vec);
    }
}
