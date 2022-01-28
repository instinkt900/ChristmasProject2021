#pragma once

namespace ui {
    class AnimationClip {
    public:
        enum class LoopType {
            Stop,
            Loop,
            Reset,
        };

        AnimationClip(nlohmann::json const& json);

        std::string const& GetName() const { return m_name; }
        int GetStartFrame() const { return m_startFrame; }
        int GetEndFrame() const { return m_endFrame; }
        float GetFPS() const { return m_fps; }
        LoopType GetLoopType() const { return m_loopType; }
        float GetDuration() const { return (m_endFrame - m_startFrame) / m_fps; }

        void SetStartTime(float time) {
            m_startTime = time;
            m_endTime = m_startTime + GetDuration();
        }

        float GetStartTime() const { return m_startTime; }
        float GetEndTime() const { return m_endTime; }

    private:
        std::string m_name;
        int m_startFrame;
        int m_endFrame;
        float m_fps;
        LoopType m_loopType;

        float m_startTime = 0;
        float m_endTime = 0;
    };
}
