#include "Profiler.h"
#include "engine/threads/SpinLockGuard.h"
#include "engine/reflect/Util.h"
#include "engine/io/VFS.h"

namespace se::profiling
{
    Profiler * Profiler::Get()
    {
        static Profiler* s_Profiler = new Profiler();
        return s_Profiler;
    }

    Profiler::Profiler()
        : m_StartTime(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    {}

    void Profiler::BeginFrame()
    {
        if (!m_FrameRecords.empty())
        {
            auto& frameRecord = m_FrameRecords.back();
            frameRecord.endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() - m_StartTime;
            frameRecord.duration = (frameRecord.endTime - frameRecord.startTime) / 1000000000.0;

            for (auto& [_, threadRecord] : frameRecord.threadRecords)
            {
                if (!threadRecord.profileRecords.empty())
                {
                    threadRecord.endTime = threadRecord.profileRecords.back().endTime;
                    threadRecord.duration = (threadRecord.endTime - threadRecord.startTime) / 1000000000.0;
                }
            }
        }

        m_FrameRecords.emplace_back();
        m_FrameRecords.back().startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() - m_StartTime;
    }

    void Profiler::CreateThreadRecordIfMissing(const std::thread::id &threadId)
    {
        threads::SpinLockGuard guard(m_ThreadRecordsLock);
        auto& record = m_FrameRecords.back();
        size_t threadHash = std::hash<std::thread::id>{}(threadId);
        if (!record.threadRecords.contains(threadHash))
        {
            ThreadRecord threadRecord;
            threadRecord.startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() - m_StartTime;
            threadRecord.profileRecords.reserve(1024);
            record.threadRecords.insert(std::make_pair(threadHash, threadRecord));
        }

        if (!m_ScopeStack.contains(threadId))
        {
            m_ScopeStack.insert(std::make_pair(threadId, std::vector<ProfileRecord*>()));
        }
    }

    void Profiler::PushProfileMarker(const std::string &name)
    {
        threads::SpinLockGuard guard(m_ThreadRecordsLock);
        auto threadId = std::this_thread::get_id();
        auto& scopeStack = m_ScopeStack.at(threadId);
        ProfileRecord record;
        record.name = name;
        record.startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() - m_StartTime;
        if (scopeStack.empty())
        {
            auto& threadRecord = m_FrameRecords.back().threadRecords.at(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            threadRecord.profileRecords.push_back(record);
            scopeStack.push_back(&threadRecord.profileRecords.back());
        }
        else
        {
            scopeStack.back()->children.push_back(record);
            scopeStack.push_back(&scopeStack.back()->children.back());
        }

    }
    void Profiler::PopProfileMarker()
    {
        threads::SpinLockGuard guard(m_ThreadRecordsLock);
        auto threadId = std::this_thread::get_id();
        auto& scopeStack = m_ScopeStack.at(threadId);

        auto& record = scopeStack.back();
        record->endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() - m_StartTime;
        record->duration = (record->endTime - record->startTime) / 1000000000.0;
        scopeStack.pop_back();
    }
}
