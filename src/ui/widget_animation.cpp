#include "game_pch.h"
#include "widget_animation.h"
#include "widget.h"

WidgetAnimationTrack::WidgetAnimationTrack(float& value)
    : m_value(value) {
}

void WidgetAnimationTrack::SetTime(float time) {
    auto firstKeyframeIt = std::begin(m_keyframes);
    auto lastKeyframeIt = std::begin(m_keyframes);
    while (lastKeyframeIt->m_time < time) {
        firstKeyframeIt = lastKeyframeIt;
        ++lastKeyframeIt;
    }

    float const t = time - firstKeyframeIt->m_time;
    switch (firstKeyframeIt->m_type) {
    case WidgetAnimationKeyframeType::Step:
        m_value = firstKeyframeIt->m_value;
        break;
    case WidgetAnimationKeyframeType::Linear:
        m_value = firstKeyframeIt->m_value + (lastKeyframeIt->m_value - firstKeyframeIt->m_value) * t;
        break;
    }
}

WidgetTracks::WidgetTracks(WidgetAnimationTracksDesc const& animationDesc, Widget& parentWidget) {
    for (auto&& trackDesc : animationDesc.tracks) {
        m_tracks.push_back(WidgetAnimationTrack(GetValueRef(parentWidget, trackDesc.first)));
    }
}

void WidgetAnimation::Update(float deltaTime) {
    if (m_playing) {
        m_currentTime += deltaTime;
        if (m_currentTime >= m_maxTime) {
            OnEnd();
        }

        for (auto&& track : m_tracks) {
            track.SetTime(m_currentTime);
        }
    }
}

void WidgetAnimation::OnEnd() {
    switch (m_loopType) {
    case WidgetAnimationLoopType::Stop:
        m_currentTime = m_maxTime;
        m_playing = false;
        break;
    case WidgetAnimationLoopType::Reset:
        m_currentTime = 0;
        m_playing = false;
        break;
    case WidgetAnimationLoopType::Loop:
        m_currentTime -= m_maxTime;
        break;
    }
}

float& WidgetAnimation::GetValueRef(Widget& parentWidget, WidgetAnimationTrackType type) const {
    switch (type) {
    case WidgetAnimationTrackType::AnchorLeft:
        return parentWidget.m_layoutBounds.topLeft.anchor.x;
    case WidgetAnimationTrackType::AnchorRight:
        return parentWidget.m_layoutBounds.bottomRight.anchor.x;
    case WidgetAnimationTrackType::AnchorTop:
        return parentWidget.m_layoutBounds.topLeft.anchor.y;
    case WidgetAnimationTrackType::AnchorBottom:
        return parentWidget.m_layoutBounds.bottomRight.anchor.y;
    case WidgetAnimationTrackType::OffsetLeft:
        return parentWidget.m_layoutBounds.topLeft.offset.x;
    case WidgetAnimationTrackType::OffsetRight:
        return parentWidget.m_layoutBounds.bottomRight.offset.x;
    case WidgetAnimationTrackType::OffsetTop:
        return parentWidget.m_layoutBounds.topLeft.offset.y;
    case WidgetAnimationTrackType::OffsetBottom:
        return parentWidget.m_layoutBounds.bottomRight.offset.y;
    default:
        assert(false);
        static float dummy = 0;
        return dummy;
    }
}
