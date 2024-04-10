#include "resource.hpp"

namespace resource
{
    const char *const eLoadFailureReason_ToString( const eLoadFailureReason eReason )
    {
        switch ( eReason )
        {
            case eLoadFailureReason::NONE:
                return "None";

            case eLoadFailureReason::RESOURCE_NOT_FOUND:
                return "Resource not found";

            case eLoadFailureReason::RESOURCE_FAILED_PARSE:
                return "Resource parsing failed";

            case eLoadFailureReason::RESOURCE_FAILED_CREATE_NATIVE_HANDLE:
                return "Native handle creation failed";
        }
    }
}
