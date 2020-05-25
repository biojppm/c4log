#ifndef C4_LOG_EXPORT_HPP_
#define C4_LOG_EXPORT_HPP_

#ifdef _WIN32
    #ifdef C4LOG_SHARED
        #ifdef C4LOG_EXPORTS
            #define C4LOG_EXPORT __declspec(dllexport)
        #else
            #define C4LOG_EXPORT __declspec(dllimport)
        #endif
    #else
        #define C4LOG_EXPORT
    #endif
#else
    #define C4LOG_EXPORT
#endif

#endif /* C4_LOG_EXPORT_HPP_ */
