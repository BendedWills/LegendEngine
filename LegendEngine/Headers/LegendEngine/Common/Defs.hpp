#ifndef _LEGENDENGINE_DEFS_HPP
#define _LEGENDENGINE_DEFS_HPP

#include <sstream>

/**
 * @brief Disables object copying.
 */
#define LEGENDENGINE_NO_COPY(typename) \
    typename(const typename&) = delete; \
    typename(typename&&) = delete; \
    typename& operator=(const typename&) = delete; \
    typename& operator=(typename&&) = delete;

/**
 * @brief Logs to an application as an object.
 */
#define LEGENDENGINE_OBJECT_LOG(application, object, message, logType) \
    { \
        std::stringstream objlogstream; \
        objlogstream << (object); \
        objlogstream << " (" << (uint64_t)this << "): "; \
        objlogstream << (message); \
        application->Log(objlogstream.str(), (logType)); \
    }

#define LEGENDENGINE_OBJECT_LOG_AS(application, object, pObject, message, logType) \
    { \
        std::stringstream objlogstream; \
        objlogstream << (object); \
        objlogstream << " (" << (uint64_t)(pObject) << "): "; \
        objlogstream << (message); \
        application->Log(objlogstream.str(), (logType)); \
    }

#endif //_LEGENDENGINE_DEFS_HPP