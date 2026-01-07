#include "System.h"

namespace se::ecs
{
    bool System::DependsOn(const Id& other) const
    {
        return std::ranges::contains(m_Declaration.dependencies, other);
    }

    void System::Init()
    {
        RunQuery(std::bind(&System::OnInit, this, std::placeholders::_1), true);
    }

    void System::Update()
    {
        RunQuery(std::bind(&System::OnUpdate, this, std::placeholders::_1), false);
    }

    void System::Render()
    {
        RunQuery(std::bind(&System::OnRender, this, std::placeholders::_1), false);
    }

    void System::Shutdown()
    {
        RunQuery(std::bind(&System::OnShutdown, this, std::placeholders::_1), true);
    }
}
