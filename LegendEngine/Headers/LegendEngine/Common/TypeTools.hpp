#ifndef _LEGENDENGINE_TYPETOOLS_HPP
#define _LEGENDENGINE_TYPETOOLS_HPP

#include <string>

#if defined(__clang__) || defined(__GNUC__)
#define LEGENDENGINE_PRETTY_FUNCTION __PRETTY_FUNCTION__
#define LEGENDENGINE_PRETTY_FUNCTION_PREFIX '='
#define LEGENDENGINE_PRETTY_FUNCTION_SUFFIX ';'
#elif defined(_MSC_VER)
#define LEGENDENGINE_PRETTY_FUNCTION __FUNCSIG__
#define LEGENDENGINE_PRETTY_FUNCTION_PREFIX '<'
#define LEGENDENGINE_PRETTY_FUNCTION_SUFFIX '>'
#else
#error "Pretty function unsupported"
#endif

namespace LegendEngine::TypeTools
{
	template<typename Type>
	std::string GetTypeName()
	{
		const char* prettyFunc = LEGENDENGINE_PRETTY_FUNCTION;
		uint64_t size = strlen(prettyFunc);

		uint64_t firstOfPrefix = 0;
		for (uint64_t i = 0; i < size; i++)
			if (prettyFunc[i] == LEGENDENGINE_PRETTY_FUNCTION_PREFIX)
			{
				firstOfPrefix = i;
				break;
			}

		uint64_t firstIndex = 0;
		for (uint64_t i = firstOfPrefix + 1; i < size; i++)
			if (prettyFunc[i] != ' ')
			{
				firstIndex = i;
				break;
			}

		uint64_t lastIndex = 0;
		for (uint64_t i = size - 1; i >= 0; i--)
			if (prettyFunc[i] != LEGENDENGINE_PRETTY_FUNCTION_SUFFIX)
			{
				lastIndex = i;
				break;
			}

		return std::string(prettyFunc + firstIndex, lastIndex - firstIndex);
	}
}

#endif //_LEGENDENGINE_TYPETOOLS_HPP