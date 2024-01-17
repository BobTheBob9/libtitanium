#pragma once

// NDEBUG is specified by cmake if not in a debug build
#ifndef NDEBUG
#define HAS_DEBUG 1
#else
#define HAS_DEBUG 0
#endif // #ifdndef NDEBUG
