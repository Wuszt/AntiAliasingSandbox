#pragma once
#include <functional>

class RTV;

class IAAPerformer
{
public:
    IAAPerformer(std::function<void(RTV*)> func);
    virtual ~IAAPerformer();

    virtual void OnEnable();
    virtual void OnDisable();

    virtual void Update();

    virtual void DrawScene() = 0;
    virtual void PostProcessing() = 0;

    virtual std::string GetName() = 0;

    virtual int GetVariantsAmount() const = 0;
    virtual void SetVariant(int variantIndex) = 0;

    RTV* GetOutputRTV();

protected:
    std::function<void(RTV*)> m_drawFunc;
    RTV* m_output;
};

