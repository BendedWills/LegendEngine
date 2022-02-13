/*
 * IDisposable is an abstract class that is extended when something needs to
 * be disposed when it is destroyed.
 * 
 * It comes with a Dispose function that will fire off the OnDispose virtual
 * function only if the object's initialized field is set to true.
 */

#ifndef _LEGENDENGINE_IDISPOSABLE_HPP
#define _LEGENDENGINE_IDISPOSABLE_HPP

#define LEGENDENGINE_ASSERT_INITIALIZED() \
    if (!initialized) \
        return;

#define LEGENDENGINE_ASSERT_INITIALIZED_RET(returnValue) \
    if (!initialized) \
        return returnValue;

namespace LegendEngine
{
    class IDisposable
    {
    public:
        // Calls the Dispose function
        ~IDisposable();

        /**
         * @returns True if the object is initialized; otherwise, false.
         */
        bool IsInitialized() const;

        /**
         * @brief Disposes the object
         */
        void Dispose();
    protected:
        // This is the fuction that should be overridden for object disposal.
        virtual void OnDispose() {}

        // This field must be set to true in order for OnDispose to be called.
        // After dispose is called, this field will automatically be set to false.
        bool initialized = false;
    };
}

#endif //_LEGENDENGINE_IDISPOSABLE_HPP