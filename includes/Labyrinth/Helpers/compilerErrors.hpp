#ifndef LABYRINTH_PENDING_HELPERS_COMPILERERRORS_HPP
#define LABYRINTH_PENDING_HELPERS_COMPILERERRORS_HPP

/// @file
/// File defining a macro to disable compiler warnings for GCC/MSVC
///
/// Used for library headers, whose code we do not control

/// @code{.cpp}
/// 
/// LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH()
/// #include <misbehaving-library-headers>
/// LP_PRAGMA_DISABLE_ALL_WARNINGS_POP()
/// 
/// @endcode

/// @def LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH()
/// Disables all warnings.
/// Warp library #includes with this + end with @ref LP_PRAGMA_DISABLE_ALL_WARNINGS_POP

/// @def LP_PRAGMA_DISABLE_ALL_WARNINGS_POP()
/// Re-enables disabled warnings
/// End wrapped library #includes sections with this


#if defined(__clang__)

#error "clang not supported"

#elif defined(__GNUC__) || defined(__GNUG__)

#define LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH() \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wduplicated-branches\"") \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")    \
    _Pragma("GCC diagnostic ignored \"-Woverloaded-virtual\"") \
    _Pragma("GCC diagnostic ignored \"-Wcast-qual\"") \
    _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")

#define LP_PRAGMA_DISABLE_ALL_WARNINGS_POP() _Pragma("GCC diagnostic pop")

#elif defined(_MSC_VER)

#define LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH()\
    _Pragma("warning(push, 0)")

    
#define LP_PRAGMA_DISABLE_ALL_WARNINGS_POP() \
    _Pragma("warning(pop)")

#elif

#define LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH()
#define LP_PRAGMA_DISABLE_ALL_WARNINGS_POP()
#endif



#endif //LABYRINTH_PENDING_HELPERS_COMPILERERRORS_HPP