#pragma once

namespace util
{
    /*
     *  Object that calls a function provided in its constructor
     *  Intended to be placed at file/namespace scope to have its constructor during static initialisation
     */
    struct StaticInitialise
    {
        StaticInitialise() = delete;
        StaticInitialise( void( *fnFunc )() ) { fnFunc(); }
    };
}
