#include "CollisionSettings.h"

#include "engine/io/VFS.h"
#include "engine/reflect/Util.h"

namespace se::geo
{
    CollisionSettings& CollisionSettings::Get()
    {
        static geo::CollisionSettings ret = {};
        static bool init = false;
        if (!init)
        {
            if (io::VFS::Get().Exists("/config/collision_settings.sass"))
            {
                auto db = asset::binary::Database::Load("/config/collision_settings.sass", true);
                ret = reflect::DeserialiseType<geo::CollisionSettings>(db);
            }
            init = true;
        }
        ret.matrix.Init(ret.channels);
        return ret;
    }

    void CollisionMatrix::Init(const std::vector<string::Symbol>& collisionChannels)
    {
        channelIndices.clear();
        for (uint8_t i = 0; i < collisionChannels.size(); i++)
        {
            channelIndices[collisionChannels[i]] = i;
        }

        if (collisionMatrix.size() < collisionChannels.size())
        {
            collisionMatrix.resize(collisionChannels.size());
            for (auto& channel : collisionMatrix)
            {
                channel.resize(collisionChannels.size(), true);
            }
        }
    }

    void CollisionMatrix::SetCollides(string::Symbol a, string::Symbol b, bool collides)
    {
        int indexA = channelIndices[a];
        int indexB = channelIndices[b];
        collisionMatrix[indexA][indexB] = collides ? 1 : 0;
        collisionMatrix[indexB][indexA] = collides ? 1 : 0;
    }

    bool CollisionMatrix::Collides(string::Symbol a, string::Symbol b) const
    {
        return collisionMatrix[channelIndices.at(a)][channelIndices.at(b)];
    }

    string::Symbol CollisionSettings::GetChannel(int index) const
    {
        return channels[index];
    }

    int CollisionSettings::GetChannelIndex(const string::Symbol& channel) const
    {
        for (uint8_t i = 0; i < channels.size(); i++)
        {
            if (channels.at(i) == channel)
            {
                return i;
            }
        }

        return -1;
    }
}
