#pragma once

namespace ui {
    struct AnimationClip {
        AnimationClip();
        //AnimationClip(nlohmann::json const& json);

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
        int m_startFrame = 0;
        int m_endFrame = 0;
        float m_fps = 30;
        LoopType m_loopType = LoopType::Stop;

        float m_startTime = 0;
        float m_endTime = 0;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnimationClip, m_name, m_startFrame, m_endFrame, m_fps, m_loopType);
    };
}
