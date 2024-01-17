#pragma once

#include <atomic>

namespace filesystem
{
    void Initialise();

    template<typename T>
    struct ReadHandle
    {
        const T * m_ptParsedFile;
        std::atomic_int m_nReferenceCount;
    };

    template<typename T>
    ReadHandle<T> * ReadBlocking( const char *const pszFilePath );

    /*
     *  Decrements a ReadHandle's reference count
     *  If the handle's reference count is 0 after this function is called, free the asset pointed to by the handle
     */
    template<typename T>
    void ReadHandle_Derefence( ReadHandle<T> *const pReadHandle );

    // TODO: functions for generating jobsystem jobs
    // e.g. CreateMultipleReadJob
};
