#include <LegendEngine/Application3D.hpp>

#include <sstream>

using namespace LegendEngine;

bool Application3D::InitObject(Object3d::Object& object)
{
    return InitObject(&object);
}

bool Application3D::InitObject(Object3d::Object* pObject)
{
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

bool Application3D::InitScene(Scene3D& scene)
{
    return InitScene(&scene);
}

bool Application3D::InitScene(Scene3D* pScene)
{
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
    this->activeScene = pScene;
}

void Application3D::RemoveActiveScene()
{
    activeScene = nullptr;
}

Scene3D* Application3D::GetActiveScene()
{
    return activeScene;
}
