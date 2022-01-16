#include "game_pch.h"
#include "animation_clip_info.h"

namespace ui {
    NLOHMANN_JSON_SERIALIZE_ENUM(AnimationClipInfo::LoopType, {
                                                                      { AnimationClipInfo::LoopType::Stop, "stop" },
                                                                      { AnimationClipInfo::LoopType::Loop, "loop" },
                                                                      { AnimationClipInfo::LoopType::Reset, "reset" },
                                                                  })

    AnimationClipInfo::AnimationClipInfo(nlohmann::json const& json, float startTime)
        : m_name(json["name"])
        , m_startFrame(json["start_frame"])
        , m_endFrame(json["end_frame"])
        , m_fps(json["fps"])
        , m_loopType(json["loop_type"])
        , m_startTime(startTime)
        , m_endTime(startTime + ((m_endFrame - m_startFrame) / m_fps)) {
    }
}
