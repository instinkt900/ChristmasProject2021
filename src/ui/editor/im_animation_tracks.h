#pragma once

struct ImDrawList;
struct ImRect;
namespace ImAnimation {
    struct IAnimation {
        bool focused = false;
        virtual int GetFrameMin() const = 0;
        virtual int GetFrameMax() const = 0;

        virtual void BeginEdit(int /*index*/) {}
        virtual void EndEdit() {}

        virtual int GetClipCount() const { return 0; }
        virtual void GetClip(int /*clipIndex*/, int** /*startFrame*/, int** /*endFrame*/, char const** /*clipName*/, unsigned int* /*color*/) = 0;
        virtual void DoubleClickClip(int /*clipIndex*/) {}
        virtual void BeginEditClip(int /*clipIndex*/) {}
        virtual void EndEditClip() {}

        virtual int GetTrackCount() const = 0;
        virtual const char* GetTrackLabel(int /*index*/) const { return ""; }
        virtual void GetKeyframes(int trackIndex, int** keyframes, int** keyframeTypes, int* keyframeCount) = 0;
        virtual void DoubleClickKey(int /*trackIndex*/, int /*keyframeIndex*/) {}

        virtual void Add(int /*type*/) {}
        virtual void Del(int /*index*/) {}
        virtual void Duplicate(int /*index*/) {}

        virtual void Copy() {}
        virtual void Paste() {}

        virtual size_t GetCustomHeight(int /*index*/) { return 0; }
        virtual void DoubleClick(int /*index*/) {}
        virtual void CustomDraw(int /*index*/, ImDrawList* /*draw_list*/, const ImRect& /*rc*/, const ImRect& /*legendRect*/, const ImRect& /*clippingRect*/, const ImRect& /*legendClippingRect*/) {}
        virtual void CustomDrawCompact(int /*index*/, ImDrawList* /*draw_list*/, const ImRect& /*rc*/, const ImRect& /*clippingRect*/) {}
    };

    // return true if selection is made
    bool Animation(IAnimation* sequence, int* currentFrame, bool* expanded, int* firstFrame);
}
