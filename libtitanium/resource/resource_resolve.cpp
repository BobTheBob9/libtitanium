#include "resource.hpp"
#include "readbuf.hpp"

namespace resource
{
    GetReadBufResult GetReadBufForPath( ResourceLoader *const pResourceLoader, const util::data::Span<char> strPath )
    {
        // TODO: this should resolve from multiple different sources when available (e.g. paks and stuff)
        ReadBuf r_readBuf;
        bool r_success = ReadBufSource_File_CreateReadBuf( &r_readBuf, strPath, nullptr );
        return { .success = r_success, .readBuf = r_readBuf };
    }
}
