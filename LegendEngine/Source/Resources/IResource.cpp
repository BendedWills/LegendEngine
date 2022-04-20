#include <LegendEngine/Resources/IResource.hpp>
#include <LegendEngine/Application.hpp>

#include <Tether/Common/VectorUtils.hpp>

using namespace LegendEngine::Resources;

IResource::IResource()
{
	singleDispose = true;
}

void IResource::OnInit()
{
	pApplication->resources.push_back(this);
}

void IResource::OnDispose()
{
	Tether::VectorUtils::EraseAll(pApplication->resources, this);
	OnResourceDispose();
}
