#pragma once

struct ImDrawList;
struct ImRect;
namespace ImAnimation {
    struct IAnimation {
        bool focused = false;

        virtual void BeginEdit(int /*index*/) {}
        virtual void EndEdit() {}

        virtual int GetClipCount() const { return 0; }
        virtual void GetClip(int /*clipIndex*/, int** /*startFrame*/, int** /*endFrame*/, char const** /*clipName*/, unsigned int* /*color*/) = 0;
        virtual void DoubleClickClip(int /*clipIndex*/) {}
        virtual void BeginEditClip(int /*clipIndex*/) {}
        virtual void EndEditClip() {}

        virtual int GetTrackCount() const = 0;
        virtual const char* GetTrackLabel(int /*index*/) const { return ""; }
        virtual void GetKeyframes(int trackIndex, int** keyframes, int* keyframeCount) = 0;
        virtual void DoubleClickKey(int /*trackIndex*/, int /*keyframeIndex*/) {}

        virtual void DoubleClick(int /*index*/) {}
    };

    // return true if selection is made
    bool Animation(IAnimation* sequence, int* currentFrame, bool* expanded, int minFrame, int maxFrame, int* firstFrame);
}
