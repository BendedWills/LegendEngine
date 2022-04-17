#include <LegendEngine/Resources/IResource.hpp>

using namespace LegendEngine::Resources;

void IResource::OnDispose()
{
	OnResourceDispose();
}
