#pragma once

namespace ui {
    struct AnimationClip {
        AnimationClip(nlohmann::json const& json);

        void SetStartTime(float time) {
            m_startTime = time;
            m_endTime = m_startTime + GetDuration();
        }

        float GetDuration() const { return (m_endFrame - m_startFrame) / m_fps; }

        enum class LoopType {
            Stop,
            Loop,
            Reset,
        };

        std::string m_name;
        int m_startFrame;
        int m_endFrame;
        float m_fps;
        LoopType m_loopType;

        float m_startTime = 0;
        float m_endTime = 0;
    };
}
