#pragma once

namespace ui {
    class AnimationClipInfo {
    public:
        enum class LoopType {
            Stop,
            Loop,
            Reset,
        };

        AnimationClipInfo(nlohmann::json const& json, float startTime);

        std::string const& GetName() const { return m_name; }
        int GetStartFrame() const { return m_startFrame; }
        int GetEndFrame() const { return m_endFrame; }
        float GetFPS() const { return m_fps; }
        LoopType GetLoopType() const { return m_loopType; }
        float GetStartTime() const { return m_startTime; }
        float GetEndTime() const { return m_endTime; }
        float GetDuration() const { return m_endTime - m_startTime; }

    private:
        std::string m_name;
        int m_startFrame;
        int m_endFrame;
        float m_fps;
        float m_startTime;
        float m_endTime;
        LoopType m_loopType;
    };
}
