#pragma once
#include "IAAPerformer.h"
#include <vector>
#include "ConstantBuffers.h"

struct ID3D11Buffer;
struct D3D11_TEXTURE2D_DESC;

class MSAAPerformer : public IAAPerformer
{
public:
    MSAAPerformer(std::function<void(RTV*)> drawFunc, std::function<void()> initializeDepthStencilBuffFunc);

    virtual void OnEnable();
    virtual void OnDisable();

    virtual void DrawScene() override;
    virtual void PostProcessing() override;
    virtual void FillDepthStencilDescWithDefaultValues(D3D11_TEXTURE2D_DESC& desc);

    virtual void Update() override;

    virtual std::string GetName() override;

    void Refresh();


    virtual int GetVariantsAmount() const override { return 6; }


    virtual void SetVariant(int variantIndex) override;

private:
    int m_sampleAmount;

    RTV* m_temporaryRTV;

    cbMSAA m_cbMSAA;
    ID3D11Buffer* m_cbMSAABuff;

    std::function<void()> m_initializeDepthStencilBuffFunc;

    bool m_standardResolve;
};

