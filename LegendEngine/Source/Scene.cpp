#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Application.hpp>

using namespace LegendEngine;

#define LEGENDENGINE_ASSERT_APP_NULL(returnValue) \
    if (pApplication == nullptr) \
        return returnValue;

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

    objects.push_back(pObject);
    //AddComponents(pObject);

    pApplication->OnSceneObjectAdd(this, pObject);
}

bool Scene::HasObject(Objects::Object& object)
{
    LEGENDENGINE_ASSERT_APP_NULL(false);
    return HasObject(&object);
}

bool Scene::HasObject(Objects::Object* pObject)
{
    LEGENDENGINE_ASSERT_APP_NULL(false);
    return std::count(objects.begin(), objects.end(), pObject);
}

bool Scene::RemoveObject(Objects::Object& object)
{
    return RemoveObject(&object);
}

#define LEGENDENGINE_COMPONENT_NAMES() \
    using namespace Objects::Components; \
    std::string meshComponentName = pObject->GetTypeName<MeshComponent>();

bool Scene::RemoveObject(Objects::Object* pObject)
{
    LEGENDENGINE_ASSERT_APP_NULL(false);

    if (!pObject)
    {
        pApplication->Log("Removing object: Object is nullptr. Returning.",
            LogType::WARN);
        return false;
    }

    if (pObject->GetApplication() != pApplication)
        return false;
    
    LEGENDENGINE_COMPONENT_NAMES();
    
    for (uint64_t i = 0; i < objects.size(); i++)
        if (objects[i] == pObject)
        {
            objects.erase(objects.begin() + i);
            //RemoveComponents(pObject);

            pApplication->OnSceneObjectRemove(this, pObject);
        }

    return false;
}

std::vector<Objects::Object*>* Scene::GetObjects()
{
    return &objects;
}

void Scene::OnObjectComponentAdd(Objects::Object*, std::string typeName,
    Objects::Components::Component* pComponent)
{

}

void Scene::OnObjectComponentRemove(Objects::Object*, std::string typeName,
    Objects::Components::Component* pComponent)
{

}

// void Scene::AddComponents(Objects::Object* pObject)
// {
//     LEGENDENGINE_COMPONENT_NAMES();

//     auto components = pObject->GetComponents();
//     for (auto it : *components)
//     {
//         if (it.first == meshComponentName) 
//             meshComponents.push_back((MeshComponent*)it.second.get());
//     }
// }

// void Scene::RemoveComponents(Objects::Object* pObject)
// {
//     LEGENDENGINE_COMPONENT_NAMES();

//     #define LEGENDENGINE_COMPONENT_REMOVE_LOOP(name, nameOfComponent) \
//         if (it.first == nameOfComponent) \
//             for (uint64_t i = 0; i < name.size(); i++) \
//             { \
//                 if (name[i] == it.second.get()) \
//                     name.erase(name.begin() + i); \
//             }

//     auto components = pObject->GetComponents();
//     for (auto it : *components)
//     {
//         LEGENDENGINE_COMPONENT_REMOVE_LOOP(meshComponents, meshComponentName);
//     }
// }
