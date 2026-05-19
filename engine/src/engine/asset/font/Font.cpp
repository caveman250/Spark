#include "Font.h"

#include "engine/Application.h"
#include "engine/asset/builder/FontBlueprint.h"
#include "engine/asset/texture/Texture.h"
#include "platform/IWindow.h"

namespace se::asset
{
    const std::string& Font::GetName() const
    {
        return m_Name;
    }

    const std::shared_ptr<Texture>& Font::GetTextureAsset(int fontSize)
    {
        auto* window = Application::Get()->GetWindow();
        if (fontSize * window->GetContentScale() > builder::FontBlueprint::BitmapCutoffSize)
        {
            return m_SDFTexture.GetAsset();
        }
        else
        {
            return m_BitmapTextures[fontSize - 1].GetAsset();
        }
    }

    const CharData& Font::GetCharData(char c, int fontSize) const
    {
#if SPARK_PLATFORM_LINUX
        if (c == 0)
        {
            // Linux treats trailing spaces as null. just trust that they were supposed to be spaces.
            c = ' ';
        }
#endif

        // TODO this probably wont work for fractional scaling.
        if (fontSize > builder::FontBlueprint::BitmapCutoffSize)
        {
            auto it = m_SDFCharData.find(c);
            if (!SPARK_VERIFY(it != m_SDFCharData.end()))
            {
                static CharData nullCharData = {};
                return nullCharData;
            }

            return it->second;
        }
        else
        {
            const auto& map = m_BitmapCharData.at(fontSize);
            auto it = map.find(c);
            if (!SPARK_VERIFY(it != map.end()))
            {
                static CharData nullCharData = {};
                return nullCharData;
            }

            return it->second;
        }
    }

    float Font::GetLineHeight(const int fontSize) const
    {
        const float scale = static_cast<float>(fontSize) / builder::FontBlueprint::Scale;
        const float height = m_Ascent - m_Descent + m_LineGap;
        return height * scale;
    }

    float Font::GetAscent(const int fontSize) const
    {
        const float scale = static_cast<float>(fontSize) / builder::FontBlueprint::Scale;
        return m_Ascent * scale;
    }

    float Font::GetDescent(const int fontSize) const
    {
        const float scale = static_cast<float>(fontSize) / builder::FontBlueprint::Scale;
        return m_Descent * scale;
    }
}
