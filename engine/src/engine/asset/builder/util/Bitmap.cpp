#include "Bitmap.h"

namespace se
{
#define UCHAR(x) (unsigned char) ((x) & 0xff)

    memory::BinaryBlob asset::builder::util::CreateBitmapData(const int width, const int height)
    {
        const int pad = -width * 3 & 3;
        const size_t size = s_BitmapHeaderSize + (width * 3 + pad) * height;
        auto* bitmap = static_cast<unsigned char *>(std::malloc(size));
        bitmap[0] = UCHAR('B');
        bitmap[1] = UCHAR('M');

        bitmap[2] = UCHAR(size);
        bitmap[3] = UCHAR(size >> 8);
        bitmap[4] = UCHAR(size >> 16);
        bitmap[5] = UCHAR(size >> 24);

        bitmap[6] = UCHAR(0);
        bitmap[7] = UCHAR(0);
        bitmap[8] = UCHAR(0);
        bitmap[9] = UCHAR(0);

        bitmap[10] = UCHAR(s_BitmapHeaderSize);
        bitmap[11] = UCHAR(s_BitmapHeaderSize >> 8);
        bitmap[12] = UCHAR(s_BitmapHeaderSize >> 16);
        bitmap[13] = UCHAR(s_BitmapHeaderSize >> 24);

        int lol = 40;
        bitmap[14] = UCHAR(lol);
        bitmap[15] = UCHAR(lol >> 8);
        bitmap[16] = UCHAR(lol >> 16);
        bitmap[17] = UCHAR(lol >> 24);

        bitmap[18] = UCHAR(width);
        bitmap[19] = UCHAR(width >> 8);
        bitmap[20] = UCHAR(width >> 16);
        bitmap[21] = UCHAR(width >> 24);

        bitmap[22] = UCHAR(height);
        bitmap[23] = UCHAR(height >> 8);
        bitmap[24] = UCHAR(height >> 16);
        bitmap[25] = UCHAR(height >> 24);

        lol = 1;
        bitmap[26] = UCHAR(lol);
        bitmap[27] = UCHAR(lol >> 8);

        lol = 24;
        bitmap[28] = UCHAR(lol);
        bitmap[29] = UCHAR(lol >> 8);

        for (size_t i = 30; i < s_BitmapHeaderSize; ++i)
        {
            bitmap[i] = UCHAR(0);
        }

        memset(bitmap + s_BitmapHeaderSize, 0, width * height * 3);

        return memory::BinaryBlob(bitmap, size);
    }
}
