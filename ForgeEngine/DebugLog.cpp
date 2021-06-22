#include "DebugLog.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <DirectXCommonClasses/Time.h>
#include "UIRenderingSystem.h"
#include "Window.h"
#include "Core.h"

using namespace std;
using namespace DirectX;

DebugLog::DebugLog(const Window* const& window)
{
    m_window = window;
}

DebugLog::~DebugLog()
{

}

void DebugLog::Initialize(const Window* const& window)
{
    s_instance = new DebugLog(window);
}

void DebugLog::Release()
{
    delete s_instance;
}

void DebugLog::AddToLogsQueue(const std::string& message, const DirectX::XMFLOAT4& color, const float& lifeTime)
{
    LogInfo info;
    info.Message = message;
    info.Color = color;
    info.DeathTime = Time::GetTime() + lifeTime;
    m_logsQueue.push_back(info);
}

void DebugLog::AddOrUpdateError(const std::string& message)
{
    auto found = m_errorsQueue.find(message);

    if (found != m_errorsQueue.end())
    {
        found->second.DeathTime = Time::GetTime() + ERROR_LIFE_TIME;
        return;
    }

    ErrorInfo info;
    info.DeathTime = Time::GetTime() + ERROR_LIFE_TIME;
    m_errorsQueue.emplace(message, info);
}

void DebugLog::PrintAll()
{
    PrintLogs();
    PrintErrors();
}

void DebugLog::PrintLogs()
{
    for (int i = 0; i < m_logsQueue.size(); ++i)
    {
        if (m_logsQueue[i].ShownAtLeastOnce && m_logsQueue[i].DeathTime < Time::GetTime())
        {
            m_logsQueue.erase(m_logsQueue.begin() + i);
            --i;
            continue;
        }

        m_logsQueue[i].ShownAtLeastOnce = true;
        Core::GetUIRenderingSystem()->Print(m_logsQueue[i].Message, LOG_TEXT_SIZE, MARGIN, MARGIN + LOG_TEXT_SIZE * i, m_logsQueue[i].Color, TextAnchor::Top | TextAnchor::Left);
    }
}

void DebugLog::PrintErrors()
{
    static std::vector<std::string> toRemove;

    int i = 0;
    for (const auto& it : m_errorsQueue)
    {
        if (it.second.DeathTime < Time::GetTime())
        {
            toRemove.push_back(it.first);
            continue;
        }

        float tmpA = (it.second.DeathTime - Time::GetTime()) / ERROR_LIFE_TIME;
        float alpha = tmpA * tmpA;

        Core::GetUIRenderingSystem()->Print(it.first, ERROR_TEXT_SIZE, (float)m_window->GetWidth() - MARGIN, MARGIN + ERROR_TEXT_SIZE * i, XMFLOAT4(1.0f, 0.0f, 0.0f, alpha), TextAnchor::Top | TextAnchor::Right);
        ++i;
    }

    for (const auto& rem : toRemove)
    {
        m_errorsQueue.erase(rem);
    }

    toRemove.clear();


}

void DebugLog::Draw()
{
    s_instance->PrintAll();
}

DebugLog* DebugLog::s_instance;

std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT3& vec)
{
    os << "[" << vec.x << " , " << vec.y << " , " << vec.z << "]";
    return os;
}

std::string to_string(const XMFLOAT3& vec)
{
    static std::stringstream ss;

    ss.str(string());
    ss << "[" << vec.x << " , " << vec.y << " , " << vec.z << "]";

    return ss.str();
}
