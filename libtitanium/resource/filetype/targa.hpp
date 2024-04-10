#include <libtitanium/util/numerics.hpp>
#include <libtitanium/util/maths.hpp>
#include <libtitanium/util/assert.hpp>
#include <libtitanium/resource/readbuf.hpp>

namespace resource
{
    struct ImageTGA
    {
        enum class eImageType : byte
        {
            COLORMAPPED,
            UNMAPPED_RGB,
            RLE_COLOURMAPPED = 9,
            RLE_RGB = 10,
        };

        byte imageIDLength;
        byte colourMapType;
        eImageType imageType;

        struct {
            u16 firstEntryIndex;
            u16 length;
            byte depth;
        } colourMapSpecification;

        util::maths::Vec2<u16> imageSize;
        byte pixelDepth;

        static bool ParseResource( resource::ReadBuf rawDataReadBuf, ImageTGA *const o_readData );
        static void Cleanup( ImageTGA *const pFile );

        // TODO
        /*static void ToGPUTexture( ImageTGA *const pFile )
        {

        }*/
    };
}
