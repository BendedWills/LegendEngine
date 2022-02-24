#include <LegendEngine/Application3D.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>

#include <sstream>

using namespace LegendEngine;

bool Application3D::InitObject(Object3d::Object& object)
{
    return InitObject(&object);
}

bool Application3D::InitObject(Object3d::Object* pObject)
{
    if (!pObject)
    {
        Log("Initializing object: Object is nullptr. Returning.", 
            LogType::WARN);
        return false;
    }

    if (pObject->pApplication != nullptr)
    {
        std::stringstream str;
        str << "Object (" << (uint64_t)pObject << ") has already been initialized";
        str << " (initialized with pApplication = " 
            << (uint64_t)pObject->pApplication;
        str << ")";

        Log(str.str(), LogType::WARN);

        return false;
    }

    pObject->pApplication = this;

    std::stringstream str;
    str << "Object (" << (uint64_t)pObject << ") initialized to Application3D";
    str << " (" << (uint64_t)this << ")";
    
    Log(str.str(), LogType::DEBUG);

    return true;
}

bool Application3D::InitScene(Scene3D* pScene)
{
    if (!pScene)
    {
        Log("Initializing scene: Scene is nullptr. Returning.", 
            LogType::WARN);
        return false;
    }

    if (pScene->pApplication != nullptr)
    {
        std::stringstream str;
        str << "Scene (" << (uint64_t)pScene << ") has already been initialized";
        str << " (initialized with pApplication = " 
            << (uint64_t)pScene->pApplication;
        str << ")";

        Log(str.str(), LogType::WARN);

        return false;
    }

    pScene->pApplication = this;

    std::stringstream str;
    str << "Scene (" << (uint64_t)pScene << ") initialized to Application3D";
    str << " (" << (uint64_t)this << ")";
    
    Log(str.str(), LogType::DEBUG);

    return true;
}

Scene3D* Application3D::GetDefaultScene()
{
    return &defaultScene;
}

void Application3D::SetActiveScene(Scene3D& scene)
{
    SetActiveScene(&scene);
}

void Application3D::SetActiveScene(Scene3D* pScene)
{
    if (!pScene)
        pRenderer->OnSceneRemove(pScene);
    
    this->activeScene = pScene;

    pRenderer->OnSceneChange(pScene);
}

void Application3D::RemoveActiveScene()
{
    SetActiveScene(nullptr);
}

Scene3D* Application3D::GetActiveScene()
{
    return activeScene;
}

void Application3D::OnSceneObjectAdd(Scene3D* pScene, 
    Object3d::Object* pObject)
{
    if (activeScene != pScene)
        return;
    
    if (pScene == &defaultScene)
        pRenderer->OnDefaultObjectAdd(pScene, pObject);
    else
        pRenderer->OnSceneObjectAdd(pScene, pObject);
}

void Application3D::OnSceneObjectRemove(Scene3D* pScene, 
    Object3d::Object* pObject)
{
    if (activeScene != pScene)
        return;
    
    if (pScene == &defaultScene)
        pRenderer->OnDefaultObjectRemove(pScene, pObject);
    else
        pRenderer->OnSceneObjectRemove(pScene, pObject);
}

bool Application3D::OnAppInit()
{
    InitScene(defaultScene);
    return true;
}
