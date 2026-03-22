module le.resources.resource;

namespace le
{
    Resource::Resource()
    {
#ifdef LEGENDENGINE_DEBUG
        if (!Application::HasConstructed())
            return;

        LE_DEBUG("Resource created (this = {:#x})",
            reinterpret_cast<size_t>(this));
#endif
    }

    Resource::~Resource()
    {
#ifdef LEGENDENGINE_DEBUG
        if (!Application::HasConstructed())
            return;

        LE_DEBUG("Resource destroyed (this = {:#x})",
            reinterpret_cast<size_t>(this));
#endif
    }
}