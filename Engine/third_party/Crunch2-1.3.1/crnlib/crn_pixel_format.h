/*
 * Copyright (c) 2010-2016 Richard Geldreich, Jr. and Binomial LLC
 * Copyright (c) 2020 FrozenStorm Interactive, Yoann Potinet
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation or credits
 *    is required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include "crn_dxt.h"
#include "crnlib.h"
#include "dds_defs.h"

namespace crnlib
{
    namespace pixel_format_helpers
    {
        CRN_EXPORT uint get_num_formats();
        CRN_EXPORT pixel_format get_pixel_format_by_index(uint index);

        CRN_EXPORT const char* get_pixel_format_string(pixel_format fmt);

        CRN_EXPORT const char* get_crn_format_string(crn_format fmt);

        inline bool is_grayscale(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_L8:
            case PIXEL_FMT_A8L8:
                return true;
            default:
                break;
            }
            return false;
        }

        inline bool is_dxt1(pixel_format fmt)
        {
            return (fmt == PIXEL_FMT_DXT1) || (fmt == PIXEL_FMT_DXT1A);
        }

        // has_alpha() should probably be called "has_opacity()" - it indicates if the format encodes opacity
        // because some swizzled DXT5 formats do not encode opacity.
        inline bool has_alpha(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_DXT1A:
            case PIXEL_FMT_DXT2:
            case PIXEL_FMT_DXT3:
            case PIXEL_FMT_DXT4:
            case PIXEL_FMT_DXT5:
            case PIXEL_FMT_DXT5A:
            case PIXEL_FMT_A8R8G8B8:
            case PIXEL_FMT_A8:
            case PIXEL_FMT_A8L8:
            case PIXEL_FMT_DXT5_AGBR:
            case PIXEL_FMT_ETC2A:
            case PIXEL_FMT_ETC2AS:
                return true;
            default:
                break;
            }
            return false;
        }

        inline bool is_alpha_only(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_A8:
            case PIXEL_FMT_DXT5A:
                return true;
            default:
                break;
            }
            return false;
        }

        inline bool is_normal_map(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_3DC:
            case PIXEL_FMT_DXN:
            case PIXEL_FMT_DXT5_xGBR:
            case PIXEL_FMT_DXT5_xGxR:
            case PIXEL_FMT_DXT5_AGBR:
                return true;
            default:
                break;
            }
            return false;
        }

        inline int is_dxt(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_DXT1:
            case PIXEL_FMT_DXT1A:
            case PIXEL_FMT_DXT2:
            case PIXEL_FMT_DXT3:
            case PIXEL_FMT_DXT4:
            case PIXEL_FMT_DXT5:
            case PIXEL_FMT_3DC:
            case PIXEL_FMT_DXT5A:
            case PIXEL_FMT_DXN:
            case PIXEL_FMT_DXT5_CCxY:
            case PIXEL_FMT_DXT5_xGxR:
            case PIXEL_FMT_DXT5_xGBR:
            case PIXEL_FMT_DXT5_AGBR:
            case PIXEL_FMT_ETC1:
            case PIXEL_FMT_ETC2:
            case PIXEL_FMT_ETC2A:
            case PIXEL_FMT_ETC1S:
            case PIXEL_FMT_ETC2AS:
                return true;
            default:
                break;
            }
            return false;
        }

        inline int get_fundamental_format(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_DXT1A:
                return PIXEL_FMT_DXT1;
            case PIXEL_FMT_DXT5_CCxY:
            case PIXEL_FMT_DXT5_xGxR:
            case PIXEL_FMT_DXT5_xGBR:
            case PIXEL_FMT_DXT5_AGBR:
                return PIXEL_FMT_DXT5;
            default:
                break;
            }
            return fmt;
        }

        inline dxt_format get_dxt_format(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_DXT1:
                return cDXT1;
            case PIXEL_FMT_DXT1A:
                return cDXT1A;
            case PIXEL_FMT_DXT2:
                return cDXT3;
            case PIXEL_FMT_DXT3:
                return cDXT3;
            case PIXEL_FMT_DXT4:
                return cDXT5;
            case PIXEL_FMT_DXT5:
                return cDXT5;
            case PIXEL_FMT_3DC:
                return cDXN_YX;
            case PIXEL_FMT_DXT5A:
                return cDXT5A;
            case PIXEL_FMT_DXN:
                return cDXN_XY;
            case PIXEL_FMT_DXT5_CCxY:
                return cDXT5;
            case PIXEL_FMT_DXT5_xGxR:
                return cDXT5;
            case PIXEL_FMT_DXT5_xGBR:
                return cDXT5;
            case PIXEL_FMT_DXT5_AGBR:
                return cDXT5;
            case PIXEL_FMT_ETC1:
                return cETC1;
            case PIXEL_FMT_ETC2:
                return cETC2;
            case PIXEL_FMT_ETC2A:
                return cETC2A;
            case PIXEL_FMT_ETC1S:
                return cETC1S;
            case PIXEL_FMT_ETC2AS:
                return cETC2AS;
            default:
                break;
            }
            return cDXTInvalid;
        }

        inline pixel_format from_dxt_format(dxt_format dxt_fmt)
        {
            switch (dxt_fmt)
            {
            case cDXT1:
                return PIXEL_FMT_DXT1;
            case cDXT1A:
                return PIXEL_FMT_DXT1A;
            case cDXT3:
                return PIXEL_FMT_DXT3;
            case cDXT5:
                return PIXEL_FMT_DXT5;
            case cDXN_XY:
                return PIXEL_FMT_DXN;
            case cDXN_YX:
                return PIXEL_FMT_3DC;
            case cDXT5A:
                return PIXEL_FMT_DXT5A;
            case cETC1:
                return PIXEL_FMT_ETC1;
            case cETC2:
                return PIXEL_FMT_ETC2;
            case cETC2A:
                return PIXEL_FMT_ETC2A;
            case cETC1S:
                return PIXEL_FMT_ETC1S;
            case cETC2AS:
                return PIXEL_FMT_ETC2AS;
            default:
                break;
            }
            CRNLIB_ASSERT(false);
            return PIXEL_FMT_INVALID;
        }

        inline bool is_pixel_format_non_srgb(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_3DC:
            case PIXEL_FMT_DXN:
            case PIXEL_FMT_DXT5A:
            case PIXEL_FMT_DXT5_CCxY:
            case PIXEL_FMT_DXT5_xGxR:
            case PIXEL_FMT_DXT5_xGBR:
            case PIXEL_FMT_DXT5_AGBR:
                return true;
            default:
                break;
            }
            return false;
        }

        inline bool is_crn_format_non_srgb(crn_format fmt)
        {
            switch (fmt)
            {
            case cCRNFmtDXN_XY:
            case cCRNFmtDXN_YX:
            case cCRNFmtDXT5A:
            case cCRNFmtDXT5_CCxY:
            case cCRNFmtDXT5_xGxR:
            case cCRNFmtDXT5_xGBR:
            case cCRNFmtDXT5_AGBR:
                return true;
            default:
                break;
            }
            return false;
        }

        inline uint get_bpp(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_DXT1:
                return 4;
            case PIXEL_FMT_DXT1A:
                return 4;
            case PIXEL_FMT_ETC1:
                return 4;
            case PIXEL_FMT_ETC2:
                return 4;
            case PIXEL_FMT_ETC2A:
                return 8;
            case PIXEL_FMT_ETC1S:
                return 4;
            case PIXEL_FMT_ETC2AS:
                return 8;
            case PIXEL_FMT_DXT2:
                return 8;
            case PIXEL_FMT_DXT3:
                return 8;
            case PIXEL_FMT_DXT4:
                return 8;
            case PIXEL_FMT_DXT5:
                return 8;
            case PIXEL_FMT_3DC:
                return 8;
            case PIXEL_FMT_DXT5A:
                return 4;
            case PIXEL_FMT_R8G8B8:
                return 24;
            case PIXEL_FMT_A8R8G8B8:
                return 32;
            case PIXEL_FMT_A8:
                return 8;
            case PIXEL_FMT_L8:
                return 8;
            case PIXEL_FMT_A8L8:
                return 16;
            case PIXEL_FMT_DXN:
                return 8;
            case PIXEL_FMT_DXT5_CCxY:
                return 8;
            case PIXEL_FMT_DXT5_xGxR:
                return 8;
            case PIXEL_FMT_DXT5_xGBR:
                return 8;
            case PIXEL_FMT_DXT5_AGBR:
                return 8;
            default:
                break;
            }
            CRNLIB_ASSERT(false);
            return 0;
        };

        inline uint get_dxt_bytes_per_block(pixel_format fmt)
        {
            switch (fmt)
            {
            case PIXEL_FMT_DXT1:
                return 8;
            case PIXEL_FMT_DXT1A:
                return 8;
            case PIXEL_FMT_DXT5A:
                return 8;
            case PIXEL_FMT_ETC1:
                return 8;
            case PIXEL_FMT_ETC2:
                return 8;
            case PIXEL_FMT_ETC2A:
                return 16;
            case PIXEL_FMT_ETC1S:
                return 8;
            case PIXEL_FMT_ETC2AS:
                return 16;
            case PIXEL_FMT_DXT2:
                return 16;
            case PIXEL_FMT_DXT3:
                return 16;
            case PIXEL_FMT_DXT4:
                return 16;
            case PIXEL_FMT_DXT5:
                return 16;
            case PIXEL_FMT_3DC:
                return 16;
            case PIXEL_FMT_DXN:
                return 16;
            case PIXEL_FMT_DXT5_CCxY:
                return 16;
            case PIXEL_FMT_DXT5_xGxR:
                return 16;
            case PIXEL_FMT_DXT5_xGBR:
                return 16;
            case PIXEL_FMT_DXT5_AGBR:
                return 16;
            default:
                break;
            }
            CRNLIB_ASSERT(false);
            return 0;
        }

        enum component_flags
        {
            cCompFlagRValid = 1,
            cCompFlagGValid = 2,
            cCompFlagBValid = 4,
            cCompFlagAValid = 8,

            cCompFlagGrayscale = 16,
            cCompFlagNormalMap = 32,
            cCompFlagLumaChroma = 64,

            cDefaultCompFlags = cCompFlagRValid | cCompFlagGValid | cCompFlagBValid | cCompFlagAValid
        };

        CRN_EXPORT component_flags get_component_flags(pixel_format fmt);

        CRN_EXPORT crn_format convert_pixel_format_to_best_crn_format(pixel_format crn_fmt);

        CRN_EXPORT pixel_format convert_crn_format_to_pixel_format(crn_format fmt);

    }  // namespace pixel_format_helpers

}  // namespace crnlib