#pragma once

namespace ui {
    class AnimationClipInfo;
    class AnimationTrack;

    class AnimationClip {
    public:
        AnimationClip(AnimationClipInfo const& info, nlohmann::json const& tracksJson);

        auto const& GetTracks() const { return m_tracks; }

    private:
        std::string m_name;
        std::vector<std::shared_ptr<AnimationTrack>> m_tracks;
    };
}
