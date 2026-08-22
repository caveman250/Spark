export module Spark.Asset.Builder.Util.Bitmap;

import Spark.Memory.BinaryBlob;

namespace se::asset::builder::util
{
    export constexpr int s_BitmapHeaderSize = 54;
    export memory::BinaryBlob CreateBitmapData(int width, int height);
}
