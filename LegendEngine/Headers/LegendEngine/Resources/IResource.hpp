// Resources are objects like Materials, Shaders, Textures, etc...

#ifndef _LEGENDENGINE_RESOURCES_IRESOURCE_HPP
#define _LEGENDENGINE_RESOURCES_IRESOURCE_HPP

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/IApplicationHolder.hpp>

namespace LegendEngine::Resources
{
	class IResource :
		public IDisposable,
		public IApplicationHolder
	{
	public:
		IResource();
		LEGENDENGINE_DISPOSE_ON_DESTRUCT(IResource);
		LEGENDENGINE_NO_COPY(IResource);
	protected:
		void OnInit();

		void OnDispose();

		// Called when the resource's Dispose function is called.
		virtual void OnResourceDispose() {}
	};
}

#endif //_LEGENDENGINE_RESOURCES_IRESOURCE_HPP