#pragma once
#include "IAAPerformer.h"
#include <DirectXMath.h>

class SSAAResolutionPerformer : public IAAPerformer
{
public:
    SSAAResolutionPerformer(std::function<void(RTV*)> func);
    virtual ~SSAAResolutionPerformer();

    virtual void OnEnable() override;
    virtual void OnDisable() override;

    virtual void Update() override;
    virtual std::string GetName() override;


    virtual void DrawScene() override;
    virtual void PostProcessing() override;


    virtual int GetVariantsAmount() const override { return 3; }


    virtual void SetVariant(int variantIndex) override;

private:
    int m_resolutionMultiplier;
    DirectX::XMFLOAT2 m_baseResolution;
};

