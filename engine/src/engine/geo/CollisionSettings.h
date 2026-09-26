#pragma once
#include "spark.h"
#include "engine/reflect/Object.h"

namespace se::geo
{
    struct CollisionMatrix : reflect::ObjectBase
    {
        SPARK_CLASS()

    public:
        void Init(const std::vector<string::Symbol>& collisionChannels);

        void SetCollides(string::Symbol a, string::Symbol b, bool collides);
        bool Collides(string::Symbol a, string::Symbol b) const;

    private:
        SPARK_MEMBER(Serialized)
        std::vector<std::vector<int8_t>> collisionMatrix = { { 1 } };

        std::unordered_map<string::Symbol, int8_t> channelIndices = {};
    };

    struct CollisionSettings : reflect::ObjectBase
    {
        SPARK_CLASS()

        static CollisionSettings& Get();

        SPARK_MEMBER(Serialized)
        std::vector<string::Symbol> channels  = { "Default" };

        string::Symbol GetChannel(int index) const;
        int GetChannelIndex(const string::Symbol& channel) const;

        SPARK_MEMBER(Serialized)
        CollisionMatrix matrix = {};
    };
}
