#include "Tester.h"
#include "Window.h"
#include "IAAPerformer.h"
#include "Profiler.h"
#include "Camera.h"
#include <DirectXCommonClasses/Time.h>
#include <numeric>

void Tester::UpdateScene()
{
    const int framesBeforeToCapture = 110;
    const float maxDuration = std::numeric_limits<float>::infinity();
    const float minDuration = 1.0f;

    MyApp::UpdateScene();

    ++m_framesCounter;

    m_time += Time::GetDeltaTime();

    if (m_currentCameraPos > 5)
    {
        Core::GetWindow()->SetAsDead();
        return;
    }

    Core::GetCamera()->SetSavedPosition(m_currentCameraPos);

    if (m_framesCounter == framesBeforeToCapture)
    {
        if (m_time < minDuration)
        {
            --m_framesCounter;
            return;
        }

        Profiler::RequestLogsToFile("S" + std::to_string(m_currentCameraPos) + "\\" + GetCurrentPerformer()->GetName());
    }
    else if (m_framesCounter == framesBeforeToCapture + 1)
    {
        RequestScreenshot("S" + std::to_string(m_currentCameraPos) + "\\" + GetCurrentPerformer()->GetName());
    }
    else if (m_framesCounter == framesBeforeToCapture + 2)
    {
        if (++m_currentVariant >= GetCurrentPerformer()->GetVariantsAmount())
        {
            m_currentVariant = 0;

            m_currentAAMode = (AAMode)((int)m_currentAAMode + 1);

            if (m_currentAAMode == AAMode::SSAARes)
                m_currentAAMode = (AAMode)((int)m_currentAAMode + 1);

            if (m_currentAAMode == AAMode::Count)
            {
                ++m_currentCameraPos;
                m_currentAAMode = AAMode::None;
            }

            SetAAPerformer(m_currentAAMode);
        }

        GetCurrentPerformer()->SetVariant(m_currentVariant);
    }
    else if (m_framesCounter > framesBeforeToCapture + 5)
    {
        m_time = 0.0f;
        m_framesCounter = 0;
        Profiler::Reset();
    }
    else if (m_time > maxDuration)
        m_framesCounter = framesBeforeToCapture - 1;
}
