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

WidgetAnimation::WidgetAnimation(Widget& parentWidget) {
    m_tracks.push_back(WidgetAnimationTrack(parentWidget.m_layoutBounds.topLeft.anchor.x));
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
