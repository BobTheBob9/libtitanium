#include "filesystem.hpp"

#include <stdio.h>

namespace filesystem
{
    // internal state, only 1 instance of this per program, as atm there's not realy much benefit in having multiple
    struct {
    } g_FilesystemState;

    void Initialise() {}

    template<typename T>
    ReadHandle<T> * ReadBlocking( const char *const pszFilePath )
    {        
    }

    template<typename T>
    void ReadHandle_Derefence( ReadHandle<T> *const pReadHandle )
    {
        // TODO: lmao

        if ( !--pReadHandle->m_nReferenceCount )
        {
            
        }
    }
}
