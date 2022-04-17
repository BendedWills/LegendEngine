#include <LegendEngine/Common/IApplicationHolder.hpp>

using namespace LegendEngine;

Application* IApplicationHolder::GetApplication()
{
	return pApplication;
}
