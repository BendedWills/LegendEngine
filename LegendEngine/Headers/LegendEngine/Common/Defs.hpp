#ifndef _LEGENDENGINE_DEFS_HPP
#define _LEGENDENGINE_DEFS_HPP

/**
 * @brief Disables object copying.
 */
#define LEGENDENGINE_NO_COPY(typename) \
    typename(const typename&) = delete; \
    typename(typename&&) = delete; \
    typename& operator=(const typename&) = delete; \
    typename& operator=(typename&&) = delete;

#endif //_LEGENDENGINE_DEFS_HPP