#pragma once
#include "IAAPerformer.h"
#include <vector>
#include "ConstantBuffers.h"

class RTV;
struct ID3D11Buffer;
class RTV;

class SSAAPerformer : public IAAPerformer
{
public:
    SSAAPerformer(std::function<void(RTV*)> func);

    virtual void OnEnable() override;

    virtual void DrawScene() override;
    virtual void PostProcessing() override;

    virtual void Update() override;

    virtual std::string GetName() override;


    virtual int GetVariantsAmount() const override { return 6; }


    virtual void SetVariant(int variantIndex) override;

private:
    std::vector<RTV*> m_rtvs;

    int m_samplesAmount;

    cbSSAA m_cbSSAA;
    ID3D11Buffer* m_cbSSAABuff;

    bool m_rotated = false;
};

