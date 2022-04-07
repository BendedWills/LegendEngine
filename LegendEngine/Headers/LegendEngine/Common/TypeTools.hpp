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
		std::string_view prettyFunc{ LEGENDENGINE_PRETTY_FUNCTION };

		auto first = prettyFunc.find_first_not_of(' ',
			prettyFunc.find_first_of(LEGENDENGINE_PRETTY_FUNCTION_PREFIX)
			+ 1
		);
		auto last = prettyFunc.find_last_of(
			LEGENDENGINE_PRETTY_FUNCTION_SUFFIX) - first;

		std::string prettyFuncStr(prettyFunc.begin(), prettyFunc.end());

		return prettyFuncStr.substr(first, last);
	}
}

#endif //_LEGENDENGINE_TYPETOOLS_HPP