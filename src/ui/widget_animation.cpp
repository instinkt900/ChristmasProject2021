#include "game_pch.h"
#include "widget_animation.h"
#include "widget.h"

WidgetAnimationTrack::WidgetAnimationTrack(float& value)
    : m_value(value) {
}

void WidgetAnimationTrack::SetTime(float time) {
    // should be able to cache these iterators
    auto firstKeyframeIt = std::begin(m_keyframes);
    auto lastKeyframeIt = std::begin(m_keyframes);
    while (lastKeyframeIt->time < time) {
        firstKeyframeIt = lastKeyframeIt;
        ++lastKeyframeIt;
    }

    float const t = time - firstKeyframeIt->time;
    switch (firstKeyframeIt->type) {
    case WidgetAnimationKeyframeType::Step:
        m_value = firstKeyframeIt->value;
        break;
    case WidgetAnimationKeyframeType::Linear:
        m_value = firstKeyframeIt->value + (lastKeyframeIt->value - firstKeyframeIt->value) * t;
        break;
    }
}

WidgetAnimation::WidgetAnimation(WidgetTracks& tracks, WidgetAnimationDesc const& desc)
    : m_tracks(tracks)
    , m_name(desc.name)
    , m_startTime(desc.startTime)
    , m_endTime(desc.endTime)
    , m_loopType(desc.loopType)
    , m_currentTime(desc.startTime) {
}

void WidgetAnimation::Update(float deltaTime) {
    m_currentTime += deltaTime;
    if (m_currentTime >= m_endTime) {
        switch (m_loopType) {
        case WidgetAnimationLoopType::Stop:
            m_currentTime = m_endTime;
            m_tracks.StopAnimation();
            break;
        case WidgetAnimationLoopType::Reset:
            m_currentTime = m_startTime;
            m_tracks.StopAnimation();
            break;
        case WidgetAnimationLoopType::Loop:
            m_currentTime -= (m_endTime - m_startTime);
            break;
        }
    }
    m_tracks.SetTime(m_currentTime);
}

WidgetTracks::WidgetTracks(WidgetAnimationTracksDesc const& tracksDesc, Widget& parentWidget) {
    for (auto&& trackDesc : tracksDesc.tracks) {
        m_tracks.push_back(WidgetAnimationTrack(GetValueRef(parentWidget, trackDesc.first)));
    }
    for (auto&& [animName, animDesc] : tracksDesc.animations) {
        m_animations.insert(std::make_pair(animName, WidgetAnimation(*this, animDesc)));
    }
}

void WidgetTracks::Update(float deltaTime) {
    if (m_currentAnimation) {
        m_currentAnimation->Update(deltaTime);
    }
}

void WidgetTracks::SetTime(float time) {
    m_currentTime = time;
    for (auto&& track : m_tracks) {
        track.SetTime(m_currentTime);
    }
}

bool WidgetTracks::HasAnimation(std::string const& name) {
    return std::end(m_animations) != m_animations.find(name);
}

bool WidgetTracks::SetAnimation(std::string const& name) {
    auto it = m_animations.find(name);
    if (std::end(m_animations) != it) {
        m_currentAnimation = &(it->second);
        return true;
    }
    return false;
}

void WidgetTracks::StopAnimation() {
    m_currentAnimation = nullptr;
}

float& WidgetTracks::GetValueRef(Widget& parentWidget, WidgetAnimationTrackType type) const {
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
