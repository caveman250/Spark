#pragma once
#include "engine/threads/SpinLock.h"

namespace se::profiling
{
    struct ProfileRecord
    {
        ProfileRecord()
        {
            children.reserve(1024);
        }

        std::string name;
        double duration;
        int64_t startTime;
        int64_t endTime;
        std::vector<ProfileRecord> children;
    };

    struct ThreadRecord
    {
        double duration = {};
        int64_t startTime = {};
        int64_t endTime = {};
        std::vector<ProfileRecord> profileRecords = {};
    };

    struct FrameRecord
    {
        double duration = {};
        int64_t startTime = {};
        int64_t endTime = {};
        std::unordered_map<std::thread::id, ThreadRecord> threadRecords = {};
    };

    class Profiler 
    {
    public:
        static Profiler* Get();

        Profiler();
        void BeginFrame();
        void CreateThreadRecordIfMissing(const std::thread::id& threadId);
        void PushProfileMarker(const std::string& name);
        void PopProfileMarker();
    private:
        std::unordered_map<std::thread::id, std::vector<ProfileRecord*>> m_ScopeStack;
        std::vector<FrameRecord> m_FrameRecords = {};
        threads::SpinLock m_ThreadRecordsLock = {};
        int64_t m_StartTime = 0;
    };

    struct ProfileScope
    {
        ProfileScope(const std::string& name)
        {
            Profiler::Get()->PushProfileMarker(name);
        }
        ~ProfileScope()
        {
            Profiler::Get()->PopProfileMarker();
        }
    };
}

#define ENABLE_PROFILING 0
#if ENABLE_PROFILING
#define PROFILE_SCOPE(name) auto SPARK_CAT(profileScope, __COUNTER__) = se::profiling::ProfileScope(name);
#define PROFILE_BEGIN_FRAME() se::profiling::Profiler::Get()->BeginFrame();
#define PROFILE_BEGIN_THREAD() se::profiling::Profiler::Get()->CreateThreadRecordIfMissing(std::this_thread::get_id());
#else
#define PROFILE_SCOPE(name)
#define PROFILE_BEGIN_FRAME()
#define PROFILE_BEGIN_THREAD()
#endif
