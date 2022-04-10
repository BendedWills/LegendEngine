#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Application.hpp>

using namespace LegendEngine;

#define LEGENDENGINE_ASSERT_APP_NULL() \
    if (pApplication == nullptr) \
        return;

#define LEGENDENGINE_ASSERT_APP_NULL_RET(returnValue) \
    if (pApplication == nullptr) \
        return returnValue;

void Scene::AddObject(Ref<Objects::Object>& object)
{
	AddObject(object.get());
}

void Scene::AddObject(Objects::Object& object)
{
    AddObject(&object);
}

void Scene::AddObject(Objects::Object* pObject)
{
    LEGENDENGINE_ASSERT_APP_NULL();

    if (!pObject)
    {
        pApplication->Log("Adding object: Object is nullptr. Returning.",
            LogType::WARN);
        return;
    }

    if (pObject->GetApplication() != pApplication)
    {
        pApplication->Log("Tried to add object to scene: Object was created"
            " for another application", LogType::ERROR);
        return;
    }

    if (HasObject(pObject))
    {
		pApplication->Log("Tried to add object to a scene twice. Returning", 
            LogType::WARN);
		return;
    }

    objects.push_back(pObject);
    pObject->AddToScene(this);

    AddObjectComponents(pObject);

    pApplication->OnSceneObjectAdd(this, pObject);
}

bool Scene::HasObject(Ref<Objects::Object>& object)
{
	LEGENDENGINE_ASSERT_APP_NULL_RET(false);
	return HasObject(object.get());
}

bool Scene::HasObject(Objects::Object& object)
{
    LEGENDENGINE_ASSERT_APP_NULL_RET(false);
    return HasObject(&object);
}

bool Scene::HasObject(Objects::Object* pObject)
{
    LEGENDENGINE_ASSERT_APP_NULL_RET(false);
    return std::count(objects.begin(), objects.end(), pObject);
}

bool Scene::RemoveObject(Ref<Objects::Object>& object)
{
	return RemoveObject(object.get());
}

bool Scene::RemoveObject(Objects::Object& object)
{
    return RemoveObject(&object);
}

bool Scene::RemoveObject(Objects::Object* pObject)
{
    LEGENDENGINE_ASSERT_APP_NULL_RET(false);

    if (!pObject)
    {
        pApplication->Log("Removing object: Object is nullptr. Returning.",
            LogType::WARN);
        return false;
    }

    if (pObject->GetApplication() != pApplication)
        return false;
    
    for (uint64_t i = 0; i < objects.size(); i++)
        if (objects[i] == pObject)
        {
            objects[i]->RemoveFromScene(this);

            RemoveObjectComponents(pObject);
            pApplication->OnSceneObjectRemove(this, pObject);

            objects.erase(objects.begin() + i);
        }

    return false;
}

void Scene::ClearObjects()
{
    objects.clear();
}

std::vector<Objects::Object*>* Scene::GetObjects()
{
    return &objects;
}

void Scene::OnObjectComponentAdd(Objects::Object* pObject, std::string typeName,
    Objects::Components::Component* pComponent)
{
    LEGENDENGINE_ASSERT_APP_NULL();

    pApplication->OnSceneObjectComponentAdd(this, pObject, typeName, 
        pComponent);
}

void Scene::OnObjectComponentRemove(Objects::Object* pObject, std::string typeName,
    Objects::Components::Component* pComponent)
{
    LEGENDENGINE_ASSERT_APP_NULL();

    pApplication->OnSceneObjectComponentRemove(this, pObject, typeName, 
        pComponent);
}

void Scene::OnObjectEnable(Objects::Object* pObject)
{
    LEGENDENGINE_ASSERT_APP_NULL();

	pApplication->OnSceneObjectEnable(this, pObject);
}

void Scene::OnObjectDisable(Objects::Object* pObject)
{
    LEGENDENGINE_ASSERT_APP_NULL();
    
    pApplication->OnSceneObjectDisable(this, pObject);
}

void Scene::AddObjectComponents(Objects::Object* pObject)
{
	using namespace Objects::Components;
	using namespace Objects;

    auto* objComponents = pObject->GetComponents();
	for (auto it = objComponents->begin(); it != objComponents->end(); it++)
	{
		const std::string& componentType = it->first;
		Component* pComponent = it->second.get();

		std::vector<Component*>* compVec = &components[componentType];

        compVec->push_back(pComponent);
	}
}

void Scene::RemoveObjectComponents(Objects::Object* pObject)
{
	using namespace Objects::Components;
	using namespace Objects;

	auto* objComponents = pObject->GetComponents();
	for (auto it = objComponents->begin(); it != objComponents->end(); it++)
	{
		const std::string& componentType = it->first;
		Component* pComponent = it->second.get();

		std::vector<Component*>* compVec = &components[componentType];

		auto findIter = std::find(compVec->begin(), compVec->end(), pComponent);

        if (findIter == compVec->end())
            continue;

        compVec->erase(findIter);
	}
}

std::unordered_map<std::string, std::vector<Objects::Components::Component*>>*
Scene::GetObjectComponents()
{
    return &components;
}
