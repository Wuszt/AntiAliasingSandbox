#pragma once
#include "Core.h"

struct ID3D11Buffer;
class RTV;
class IAAPerformer;
class DummyAAPerformer;
class SSAAPerformer;
class MSAAPerformer;
class FXAAPerformer;
class TAAPerformer;
class SSAAResolutionPerformer;

enum class AAMode
{
    None,
    SSAA,
    SSAARes,
    MSAA,
    FXAA,
    TAA,
    Count
};

class MyApp : public Core
{
public:
    using Core::Core;

protected:
    IAAPerformer* GetCurrentPerformer() const { return m_currentPerformer; }
    void SetAAPerformer(AAMode mode);
    virtual void UpdateScene() override;

private:
    virtual void InitScene() override;
    virtual void PostProcessing() override;
    virtual void DrawScene() override;
    virtual void FillDepthStencilDescWithDefaultValues(D3D11_TEXTURE2D_DESC& desc) override;
    virtual void OnResizeWindow(const int& width, const int& height) override;

    Object* m_car;

    DummyAAPerformer* m_noaa;
    SSAAPerformer* m_ssaa;
    SSAAResolutionPerformer* m_ssaaRes;
    MSAAPerformer* m_msaa;
    FXAAPerformer* m_fxaa;
    TAAPerformer* m_taa;

    IAAPerformer* m_currentPerformer;
};

