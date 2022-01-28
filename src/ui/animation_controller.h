#pragma once

namespace ui {
    class AnimationTrack;
    class Node;

    class AnimationTrackController {
    public:
        AnimationTrackController(float& target, AnimationTrack& track);

        void SetTime(float time);
        void SetFrame(int frame);

    private:
        float& m_target;
        AnimationTrack& m_track;
    };

    class AnimationController {
    public:
        AnimationController(Node* node, std::vector<std::shared_ptr<AnimationTrack>> tracks);

        void SetTime(float time);
        void SetFrame(int frame);

    private:
        std::vector<std::unique_ptr<AnimationTrackController>> m_trackControllers;
    };
}
