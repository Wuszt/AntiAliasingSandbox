#pragma once
#include "IAAPerformer.h"
#include <vector>
#include "ConstantBuffers.h"

class RTV;
struct ID3D11Buffer;

class TAAPerformer : public IAAPerformer
{
public:
    TAAPerformer(std::function<void(RTV*)> func);

    virtual void OnEnable() override;
    virtual void OnDisable() override;

    virtual void DrawScene() override;
    virtual void PostProcessing() override;

    virtual std::string GetName() override;

    virtual int GetVariantsAmount() const override { return 1; }

    virtual void SetVariant(int variantIndex) override {}

private:
    RTV* m_prevRTV;

    cbTAA m_cbTAA;
    ID3D11Buffer* m_cbTAABuff;
};

