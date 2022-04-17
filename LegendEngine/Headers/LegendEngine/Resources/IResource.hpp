#ifndef _LEGENDENGINE_IRESOURCE_HPP
#define _LEGENDENGINE_IRESOURCE_HPP

#include <LegendEngine/Common/IDisposable.hpp>

namespace LegendEngine::Resources
{
	class IResource : public IDisposable
	{
	public:
		LEGENDENGINE_DISPOSE_ON_DESTRUCT(IResource);

	protected:
		void OnDispose();

		// Called when the resource's Dispose function is called.
		virtual void OnResourceDispose() {}
	};
}

#endif //_LEGENDENGINE_IRESOURCE_HPP