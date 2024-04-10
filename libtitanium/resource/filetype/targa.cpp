#include "targa.hpp"

namespace resource
{
    bool ImageTGA::ParseResource( resource::ReadBuf rawDataReadBuf, ImageTGA *const o_readData )
    {
        o_readData->imageIDLength = ReadBuf::GetByte( &rawDataReadBuf );
        o_readData->colourMapType = ReadBuf::GetByte( &rawDataReadBuf );
        o_readData->imageType = static_cast<eImageType>( ReadBuf::GetByte( &rawDataReadBuf ) );

        o_readData->colourMapSpecification.firstEntryIndex = ReadBuf::GetU16( &rawDataReadBuf );
        o_readData->colourMapSpecification.length = ReadBuf::GetU16( &rawDataReadBuf );
        o_readData->colourMapSpecification.depth = ReadBuf::GetByte( &rawDataReadBuf );

        // ignore image origin, we just care about the image data and realistically nobody is going to actually use this
        ReadBuf::GetU16( &rawDataReadBuf ); // x
        ReadBuf::GetU16( &rawDataReadBuf ); // y

        o_readData->imageSize.x = ReadBuf::GetU16( &rawDataReadBuf );
        o_readData->imageSize.y = ReadBuf::GetU16( &rawDataReadBuf );
        o_readData->pixelDepth = ReadBuf::GetU16( &rawDataReadBuf );

        // ignore image descriptor
        if ( ReadBuf::GetByte( &rawDataReadBuf ) ) return false;

        return true;
    }

    void ImageTGA::Cleanup( ImageTGA *const o_readData ) {}
}
