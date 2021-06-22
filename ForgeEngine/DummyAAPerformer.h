#pragma once
#include "IAAPerformer.h"
class DummyAAPerformer :public IAAPerformer
{
public:
    DummyAAPerformer(std::function<void(RTV*)> func);

    virtual void DrawScene() override;


    virtual void PostProcessing() override;


    virtual std::string GetName() override;


    virtual int GetVariantsAmount() const override { return 1; }


    virtual void SetVariant(int variantIndex) override {}

};

