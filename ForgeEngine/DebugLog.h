#pragma once

#include <DirectXMath.h>
#include <sstream>
#include <unordered_map>
#include <vector>

#define MARGIN 5.0f
#define ERROR_LIFE_TIME 5.0f
#define ERROR_TEXT_SIZE 20.0f
#define LOG_TEXT_SIZE 20.0f

class RenderingSystem;
class Window;

struct ErrorInfo
{
    float DeathTime;
};

struct LogInfo : public ErrorInfo
{
    std::string Message;
    DirectX::XMFLOAT4 Color;
    bool ShownAtLeastOnce = false;
};

class DebugLog
{
public:
    static void Initialize(const RenderingSystem* const& renderingSystem, const Window* const& window);
    static void Draw();
    static void Release();
    inline static bool IsInitialized() { return s_instance; }

    template<class T>
    static void LogError(const T& val)
    {
        s_instance->AddOrUpdateError(GetString(val));
    }

    template<class T>
    static void Log(const T& val, const float& lifeTime = 0.0f, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
    {
        s_instance->AddToLogsQueue(GetString(val), color, lifeTime);
    }

private:
    DebugLog(const RenderingSystem* const& renderingSystem, const Window* const& window);
    ~DebugLog();

    static DebugLog* s_instance;

    template<class T>
    static std::string GetString(const T& val)
    {
        std::ostringstream ss;
        ss << val;
        return ss.str();
    }

    const RenderingSystem* m_renderingSystem;
    const Window* m_window;
    void AddToLogsQueue(const std::string& message, const DirectX::XMFLOAT4& color, const float& lifeTime);
    void AddOrUpdateError(const std::string& message);
    void PrintAll();
    void PrintLogs();
    void PrintErrors();

    std::unordered_map<std::string, ErrorInfo> m_errorsQueue;
    std::vector<LogInfo> m_logsQueue;
};

std::ostream& operator<< (std::ostream& os, const DirectX::XMFLOAT3& vec);

