#ifndef _LEGENDENGINE_NATIVE_HPP
#define _LEGENDENGINE_NATIVE_HPP

#include <LegendEngine/Common/Ref.hpp>

namespace LegendEngine
{
	template<typename T>
	class NativeHolder
	{
	public:
		NativeHolder() {}
		NativeHolder(const NativeHolder& ref)
		{
			nativeSet = ref.nativeSet;
			native = ref.native;
		}

		void SetNative(Ref<T> native)
		{
			this->native = native;
			this->nativeSet = true;
		}

		T* GetNative()
		{
			return native.get();
		}

		bool IsNativeSet()
		{
			return nativeSet;
		}
	protected:
		bool nativeSet = false;
		Ref<T> native;
	};
}

#endif //_LEGENDENGINE_NATIVE_HPP