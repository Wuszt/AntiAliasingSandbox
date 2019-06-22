#pragma once
#include <DirectXMath.h>
#include <vector>

class Transform;
class Component;

class Object
{
public:
    Object();
    virtual ~Object();

    std::string Name;
    inline Transform* GetTransform() { return m_transform; }
    virtual void Update();
    virtual void Start() {}

    template<typename T>
    T* TryToGetComponent()
    {
        auto result = std::find_if(m_components.begin(), m_components.end(), [](Component* comp) -> bool { return dynamic_cast<T*>(comp); });

        if (result == nullptr)
            return nullptr;

        return dynamic_cast<T*>(result);
    }

    template<typename T, typename ... Args>
    T* AddComponent(Args&& ... args)
    {
        T* comp = new T(this, std::forward<Args>(args)...);

        m_components.push_back(comp);

        return comp;
    }

protected:
    Transform* m_transform;

private:
    std::vector<Component*> m_components;
    bool m_started = false;
};

