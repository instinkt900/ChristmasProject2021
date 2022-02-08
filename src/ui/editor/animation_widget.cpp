#include "game_pch.h"
#include "animation_widget.h"
#include "ui/layouts/layout_entity_group.h"
#include "ui/group.h"
#include "ui/layouts/animation_clip.h"
#include "ui/layouts/animation_track.h"
#include "imgui_internal.h"
#include "actions/move_keyframe_action.h"
#include "actions/composite_action.h"
#include "editor_layer.h"
#include "actions/delete_keyframe_action.h"

namespace {
    using namespace ui;

    char const* ToString(AnimationTrack::Target target) {
        switch (target) {
        case AnimationTrack::Target::TopOffset:
            return "TopOffset";
        case AnimationTrack::Target::BottomOffset:
            return "BottomOffset";
        case AnimationTrack::Target::LeftOffset:
            return "LeftOffset";
        case AnimationTrack::Target::RightOffset:
            return "RightOffset";
        default:
            return "Unknown";
        }
    }
}

#ifndef IMGUI_DEFINE_MATH_OPERATORS
static ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
    return ImVec2(a.x + b.x, a.y + b.y);
}
#endif

namespace ui {
    AnimationWidget::AnimationWidget(EditorLayer& editorLayer, Group* group)
        : m_editorLayer(editorLayer)
        , m_group(group) {
    }

    int AnimationWidget::GetClipCount() const {
        auto layoutEntity = m_group->GetLayoutEntity();
        if (auto layout = static_cast<LayoutEntityGroup*>(layoutEntity.get())) {
            return static_cast<int>(layout->GetAnimationClips().size());
        }
        return 0;
    }

    void AnimationWidget::GetClip(int clipIndex, int** startFrame, int** endFrame, char const** clipName, unsigned int* color) {
        auto layoutEntity = m_group->GetLayoutEntity();
        if (auto layout = static_cast<LayoutEntityGroup*>(layoutEntity.get())) {
            auto& clips = layout->GetAnimationClips();

            if (startFrame)
                *startFrame = &clips[clipIndex]->m_startFrame;
            if (endFrame)
                *endFrame = &clips[clipIndex]->m_endFrame;
            if (clipName)
                *clipName = clips[clipIndex]->m_name.c_str();
            if (color)
                *color = 0xFF00CCAA;
        }
    }

    char const* AnimationWidget::GetChildLabel(int index) const {
        auto child = m_group->GetChildren()[index];
        if (child->GetId() != "") {
            return child->GetId().c_str();
        }
        static std::string tmp;
        tmp = fmt::format("Child {}", index);
        return tmp.c_str();
    }

    char const* AnimationWidget::GetTrackLabel(AnimationTrack::Target target) const {
        static std::string temp_string;
        temp_string = magic_enum::enum_name(target);
        return temp_string.c_str();
    }

    void AnimationWidget::Draw() {
        if (ImGui::Begin("Animation")) {
            m_currentFrame = m_editorLayer.GetSelectedFrame();
            DrawWidget();
            m_editorLayer.SetSelectedFrame(m_currentFrame);
        }
        ImGui::End();
    }

    void AnimationWidget::SelectKeyframe(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo) {
        if (!IsKeyframeSelected(entity, target, frameNo)) {
            if (auto keyframe = entity->GetAnimationTracks().at(target)->GetKeyframe(frameNo)) {
                m_selectedKeyframes.push_back({ entity, target, frameNo, keyframe });
            }
        }
    }

    void AnimationWidget::DeselectKeyframe(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo) {
        auto const it = std::find_if(std::begin(m_selectedKeyframes), std::end(m_selectedKeyframes), [&](auto context) {
            return context.entity == entity && context.target == target && context.frameNo == frameNo;
        });
        if (std::end(m_selectedKeyframes) != it) {
            m_selectedKeyframes.erase(it);
        }
    }

    bool AnimationWidget::IsKeyframeSelected(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo) {
        auto const it = std::find_if(std::begin(m_selectedKeyframes), std::end(m_selectedKeyframes), [&](auto context) {
            return context.entity == entity && context.target == target && context.frameNo == frameNo;
        });
        return std::end(m_selectedKeyframes) != it;
    }

    void AnimationWidget::ClearSelectedKeyframes() {
        m_selectedKeyframes.clear();
    }

    void AnimationWidget::ClearNonMatchingKeyframes(std::shared_ptr<LayoutEntity> entity, int frameNo) {
        m_selectedKeyframes.erase(std::remove_if(std::begin(m_selectedKeyframes), std::end(m_selectedKeyframes), [&](auto& context) {
                                      return context.entity != entity || context.frameNo != frameNo;
                                  }),
                                  std::end(m_selectedKeyframes));
    }

    void AnimationWidget::EndMoveKeyframes() {
        auto CreateAction = [](KeyframeContext& context) -> std::unique_ptr<IEditorAction> {
            auto& track = context.entity->GetAnimationTracks().at(context.target);
            if (context.frameNo != context.current->m_frame) {
                int const targetFrame = context.current->m_frame;
                std::optional<float> replacedValue;
                if (auto replacingKeyframe = track->GetKeyframe(targetFrame)) {
                    if (replacingKeyframe != context.current) {
                        replacedValue = replacingKeyframe->m_value;
                    }
                }
                return std::make_unique<MoveKeyframeAction>(context.entity, context.target, context.frameNo, targetFrame, replacedValue);
            }
            return nullptr;
        };

        std::vector<std::unique_ptr<IEditorAction>> actions;
        for (auto&& context : m_selectedKeyframes) {
            if (auto action = CreateAction(context)) {
                auto const newFrameNo = context.current->m_frame;
                actions.push_back(std::move(action));
                context.entity->GetAnimationTracks().at(context.target)->SortKeyframes();
                context.current = context.entity->GetAnimationTracks().at(context.target)->GetKeyframe(newFrameNo);
                assert(context.current);
                context.frameNo = newFrameNo;
            }
        }

        if (actions.size() > 1) {
            auto compositeAction = std::make_unique<CompositeAction>();
            auto& targetActions = compositeAction->GetActions();
            targetActions.insert(std::end(targetActions), std::make_move_iterator(std::begin(actions)), std::make_move_iterator(std::end(actions)));
            m_editorLayer.AddEditAction(std::move(compositeAction));
        } else if (!actions.empty()) {
            m_editorLayer.AddEditAction(std::move(actions[0]));
        }

        m_editorLayer.Refresh();
    }

    void AnimationWidget::DeleteSelectedKeyframes() {
        if (m_selectedKeyframes.empty()) {
            return;
        }

        std::vector<std::unique_ptr<IEditorAction>> actions;
        for (auto&& context : m_selectedKeyframes) {
            if (context.frameNo == 0) { // dont delete frame 0
                continue;
            }
            auto track = context.entity->GetAnimationTracks().at(context.target);
            if (auto keyframe = track->GetKeyframe(context.frameNo)) {
                float const oldValue = keyframe->m_value;
                track->DeleteKeyframe(context.frameNo);
                auto action = std::make_unique<DeleteKeyframeAction>(context.entity, context.target, context.frameNo, oldValue);
                actions.push_back(std::move(action));
            }
        }

        if (actions.size() > 1) {
            auto compositeAction = std::make_unique<CompositeAction>();
            auto& targetActions = compositeAction->GetActions();
            targetActions.insert(std::end(targetActions), std::make_move_iterator(std::begin(actions)), std::make_move_iterator(std::end(actions)));
            m_editorLayer.AddEditAction(std::move(compositeAction));
        } else if (!actions.empty()) {
            m_editorLayer.AddEditAction(std::move(actions[0]));
        }

        m_selectedKeyframes.clear();
        m_editorLayer.Refresh();
    }

    bool AnimationWidget::DrawWidget() {
        m_childExpanded.resize(m_group->GetChildCount());

        bool ret = false;
        ImGuiIO& io = ImGui::GetIO();
        int cx = (int)(io.MousePos.x);
        int cy = (int)(io.MousePos.y);
        static float framePixelWidth = 10.f;
        static float framePixelWidthTarget = 10.f;
        int legendWidth = 200;

        static int movingClipEntry = -1;
        static int movingEntry = -1;
        static int movingPos = -1;
        static int movingPart = -1;
        int ItemHeight = 20;

        int childCount = static_cast<int>(m_group->GetChildCount());
        int displayedTrackCount = childCount;
        for (int i = 0; i < childCount; ++i) {
            if (m_childExpanded[i]) {
                displayedTrackCount += static_cast<int>(m_group->GetChildren()[i]->GetLayoutEntity()->GetAnimationTracks().size());
            }
        }
        int rowCount = displayedTrackCount + 1;
        ImGui::BeginGroup();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();     // ImDrawList API uses screen coordinates!
        ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // Resize canvas to what's available
        int firstFrameUsed = m_firstFrame;

        int controlHeight = rowCount * ItemHeight;
        int frameCount = ImMax(m_maxFrame - m_minFrame, 1);

        static bool MovingScrollBar = false;
        static bool MovingCurrentFrame = false;
        static bool KeyframeGrabbed = false;

        // zoom in/out
        const int visibleFrameCount = (int)floorf((canvas_size.x - legendWidth) / framePixelWidth);
        const float barWidthRatio = ImMin(visibleFrameCount / (float)frameCount, 1.f);
        const float barWidthInPixels = barWidthRatio * (canvas_size.x - legendWidth);

        static bool panningView = false;
        static ImVec2 panningViewSource;
        static int panningViewFrame;
        if (ImGui::IsWindowFocused() && io.KeyAlt && io.MouseDown[2]) {
            if (!panningView) {
                panningViewSource = io.MousePos;
                panningView = true;
                panningViewFrame = m_firstFrame;
            }
            m_firstFrame = panningViewFrame - int((io.MousePos.x - panningViewSource.x) / framePixelWidth);
            m_firstFrame = ImClamp(m_firstFrame, m_minFrame, m_maxFrame - visibleFrameCount);
        }
        if (panningView && !io.MouseDown[2]) {
            panningView = false;
        }
        framePixelWidthTarget = ImClamp(framePixelWidthTarget, 0.1f, 50.f);

        framePixelWidth = ImLerp(framePixelWidth, framePixelWidthTarget, 0.33f);

        frameCount = m_maxFrame - m_minFrame;
        if (visibleFrameCount >= frameCount && m_firstFrame)
            m_firstFrame = m_minFrame;


        bool hasScrollBar(true);

        // test scroll area
        ImVec2 headerSize(canvas_size.x, (float)ItemHeight);
        ImVec2 scrollBarSize(canvas_size.x, 14.f);
        ImGui::InvisibleButton("topBar", headerSize);
        draw_list->AddRectFilled(canvas_pos, canvas_pos + headerSize, 0xFFFF0000, 0);
        ImVec2 childFramePos = ImGui::GetCursorScreenPos();
        ImVec2 childFrameSize(canvas_size.x, canvas_size.y - 8.f - headerSize.y - (hasScrollBar ? scrollBarSize.y : 0));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
        ImGui::BeginChildFrame(889, childFrameSize);
        ImGui::InvisibleButton("contentBar", ImVec2(canvas_size.x, float(controlHeight)));
        const ImVec2 contentMin = ImGui::GetItemRectMin();
        const ImVec2 contentMax = ImGui::GetItemRectMax();
        const ImRect contentRect(contentMin, contentMax);
        const float contentHeight = contentMax.y - contentMin.y;

        // full background
        draw_list->AddRectFilled(canvas_pos, canvas_pos + canvas_size, 0xFF242424, 0);

        // current frame top
        ImRect topRect(ImVec2(canvas_pos.x + legendWidth, canvas_pos.y), ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + ItemHeight));

        // moving current frame
        if (!MovingCurrentFrame && !MovingScrollBar && movingEntry == -1 && movingClipEntry == -1 && m_currentFrame >= 0 && topRect.Contains(io.MousePos) && io.MouseDown[0]) {
            MovingCurrentFrame = true;
        }
        if (MovingCurrentFrame) {
            if (frameCount) {
                m_currentFrame = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                if (m_currentFrame < m_minFrame)
                    m_currentFrame = m_minFrame;
                if (m_currentFrame >= m_maxFrame)
                    m_currentFrame = m_maxFrame;
            }
            if (!io.MouseDown[0])
                MovingCurrentFrame = false;
        }

        //header
        draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);

        // clips
        int clipCount = GetClipCount();
        for (int i = 0; i < clipCount; i++) {
            int* startFrame;
            int* endFrame;
            char const* clipName;
            unsigned int clipColor;
            GetClip(i, &startFrame, &endFrame, &clipName, &clipColor);

            int px = (int)contentMin.x + legendWidth - int(firstFrameUsed * framePixelWidth);
            int py = (int)contentMin.y;
            ImVec2 slotP1(px + *startFrame * framePixelWidth, py + 2.0f);
            ImVec2 slotP2(px + *endFrame * framePixelWidth + framePixelWidth, py + ItemHeight - 2.0f);
            unsigned int slotColor = clipColor;

            if (slotP1.x <= (canvas_size.x + contentMin.x) && slotP2.x >= (contentMin.x + legendWidth)) {
                draw_list->AddRectFilled(slotP1, slotP2, slotColor, 2);
            }

            if (ImRect(slotP1, slotP2).Contains(io.MousePos) && io.MouseDoubleClicked[0]) {
                //animation->DoubleClickClip(i);
            }

            ImRect rects[3] = {
                ImRect(slotP1, ImVec2(slotP1.x + framePixelWidth / 2, slotP2.y)),
                ImRect(ImVec2(slotP2.x - framePixelWidth / 2, slotP1.y), slotP2),
                ImRect(slotP1, slotP2)
            };

            const unsigned int quadColor[] = { 0xFFFFFFFF, 0xFFFFFFFF, slotColor };
            if (movingClipEntry == -1) {
                for (int j = 2; j >= 0; j--) {
                    ImRect& rc = rects[j];
                    if (!rc.Contains(io.MousePos))
                        continue;
                    draw_list->AddRectFilled(rc.Min, rc.Max, quadColor[j], 2);
                }

                for (int j = 0; j < 3; j++) {
                    ImRect& rc = rects[j];
                    if (!rc.Contains(io.MousePos))
                        continue;
                    if (ImGui::IsMouseClicked(0) && !MovingScrollBar && !MovingCurrentFrame) {
                        movingClipEntry = i;
                        movingPos = cx;
                        movingPart = j + 1;
                        BeginEditClip(movingClipEntry);
                        break;
                    }
                }
            }

            ImVec2 tsize = ImGui::CalcTextSize(clipName);
            ImVec2 tpos(slotP1.x + (slotP2.x - slotP1.x - tsize.x) / 2, slotP1.y + (slotP2.y - slotP1.y - tsize.y) / 2);
            draw_list->AddText(tpos, 0xFFFFFFFF, clipName);
        }

        // moving clip
        if (movingClipEntry >= 0) {
            ImGui::CaptureMouseFromApp();
            int diffFrame = int((cx - movingPos) / framePixelWidth);
            if (std::abs(diffFrame) > 0) {
                int *start, *end;
                GetClip(movingClipEntry, &start, &end, NULL, NULL);
                int& l = *start;
                int& r = *end;
                if (movingPart & 1)
                    l += diffFrame;
                if (movingPart & 2)
                    r += diffFrame;
                if (l < 0) {
                    if (movingPart & 2)
                        r -= l;
                    l = 0;
                }
                if (movingPart & 1 && l > r)
                    l = r;
                if (movingPart & 2 && r < l)
                    r = l;
                movingPos += int(diffFrame * framePixelWidth);
            }
            if (!io.MouseDown[0]) {
                movingClipEntry = -1;
                EndEditClip();
            }
        }

        //header frame number and lines
        int modFrameCount = 10;
        int frameStep = 1;
        while ((modFrameCount * framePixelWidth) < 150) {
            modFrameCount *= 2;
            frameStep *= 2;
        };
        int halfModFrameCount = modFrameCount / 2;

        auto drawLine = [&](int i, int regionHeight) {
            bool baseIndex = ((i % modFrameCount) == 0) || (i == m_maxFrame || i == m_minFrame);
            bool halfIndex = (i % halfModFrameCount) == 0;
            int px = int(contentMin.x + legendWidth + (i - firstFrameUsed) * framePixelWidth);
            int tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
            int tiretEnd = baseIndex ? regionHeight : ItemHeight;

            if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth)) {
                draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

                draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)ItemHeight), ImVec2((float)px, canvas_pos.y + (float)regionHeight - 1), 0x30606060, 1);
            }

            if (baseIndex && px > (canvas_pos.x + legendWidth)) {
                char tmps[512];
                ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", i);
                draw_list->AddText(ImVec2((float)px + 3.f, canvas_pos.y), 0xFFBBBBBB, tmps);
            }
        };

        auto drawLineContent = [&](int i) {
            int px = int(contentMin.x + legendWidth + (i - firstFrameUsed) * framePixelWidth);
            int tiretStart = int(contentMin.y);
            int tiretEnd = int(contentMax.y);

            if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth)) {
                draw_list->AddLine(ImVec2(float(px), float(tiretStart)), ImVec2(float(px), float(tiretEnd)), 0x30606060, 1);
            }
        };

        for (int i = m_minFrame; i <= m_maxFrame; i += frameStep) {
            drawLine(i, ItemHeight);
        }
        drawLine(m_minFrame, ItemHeight);
        drawLine(m_maxFrame, ItemHeight);

        float rowYPos = contentMin.y;

        // clip row..
        {
            ImVec2 tpos(contentMin.x + 3, rowYPos);
            draw_list->AddText(tpos, 0xFFFFFFFF, "Clips");
            rowYPos += ItemHeight;
        }

        if (ImGui::BeginPopup("keyframe_popup")) {
            if (ImGui::MenuItem("Delete")) {
                DeleteSelectedKeyframes();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImVec2 tracksMin(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, childFramePos.y);
        ImVec2 tracksMax(childFramePos.x + childFrameSize.x, childFramePos.y + childFrameSize.y);
        ImRect tracksFrame(tracksMin, tracksMax);
        bool pendingSelectionClear = ImGui::IsMouseClicked(ImGuiMouseButton_Left) && tracksFrame.Contains(io.MousePos);

        // track rows
        for (int i = 0; i < childCount; ++i) {
            auto childEntity = m_group->GetChildren()[i]->GetLayoutEntity();

            ImVec2 tpos(contentMin.x + 3, rowYPos);
            draw_list->AddText(tpos, 0xFFFFFFFF, GetChildLabel(i));

            ImVec2 labelMin(contentMin.x, rowYPos);
            ImVec2 labelMax(contentMin.x + legendWidth, rowYPos + ItemHeight);
            ImRect labelRect(labelMin, labelMax);
            if (labelRect.Contains(io.MousePos) && io.MouseClicked[0]) {
                m_childExpanded[i] = !m_childExpanded[i];
            }

            unsigned int col = (i & 1) ? 0xFF3A3636 : 0xFF413D3D;
            ImVec2 rowMin(contentMin.x + legendWidth, rowYPos);
            ImVec2 rowMax(canvas_size.x + canvas_pos.x, rowYPos + ItemHeight);
            draw_list->AddRectFilled(rowMin, rowMax, col, 0);

            bool expanded = m_childExpanded[i];
            auto& childTracks = childEntity->GetAnimationTracks();
            ImVec2 childTrackPos(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, rowYPos);
            unsigned int keyframeColor = 0xFFc4c4c4;
            unsigned int keyframeColorSelected = 0xFFFFa2a2;
            bool subColorSelector = false;
            for (auto&& [target, track] : childTracks) {

                if (expanded) {
                    rowYPos += ItemHeight;

                    ImVec2 tpos(contentMin.x + 23, rowYPos);
                    draw_list->AddText(tpos, 0xFFFFFFFF, GetTrackLabel(target));

                    ImVec2 subRowMin(contentMin.x + legendWidth, rowYPos);
                    ImVec2 subRowMax(canvas_size.x + canvas_pos.x, rowYPos + ItemHeight);
                    unsigned int col = subColorSelector ? 0xFF292525 : 0xFF302C2C;
                    draw_list->AddRectFilled(subRowMin, subRowMax, col, 0);
                }

                for (auto&& keyframe : track->GetKeyframes()) {
                    ImVec2 keyP1(childTrackPos.x + keyframe.m_frame * framePixelWidth, rowYPos + 2);
                    ImVec2 keyP2(childTrackPos.x + keyframe.m_frame * framePixelWidth + framePixelWidth, rowYPos + ItemHeight - 2);
                    ImRect keyRect(keyP1, keyP2);
                    bool selected = IsKeyframeSelected(childEntity, target, keyframe.m_frame);
                    if (keyP1.x <= (canvas_size.x + contentMin.x) && keyP2.x >= (contentMin.x + legendWidth)) {
                        draw_list->AddRectFilled(keyP1, keyP2, selected ? keyframeColorSelected : keyframeColor, 2);
                    }

                    if (!MovingScrollBar && !MovingCurrentFrame) {
                        if (keyRect.Contains(io.MousePos)) {
                            if (ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos)) {
                                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                                    // selection rules
                                    if (!IsKeyframeSelected(childEntity, target, keyframe.m_frame)) {
                                        if ((io.KeyMods & ImGuiKeyModFlags_Ctrl) == 0) {
                                            if (expanded) {
                                                ClearSelectedKeyframes();
                                            } else {
                                                ClearNonMatchingKeyframes(childEntity, keyframe.m_frame);
                                            }
                                        }
                                        SelectKeyframe(childEntity, target, keyframe.m_frame);
                                    }

                                    // action logic
                                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                                        pendingSelectionClear = false;
                                        KeyframeGrabbed = true;
                                        movingPos = cx;
                                    } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                                        ImGui::OpenPopup("keyframe_popup");
                                    }
                                }
                            }
                        }
                    }
                }
                subColorSelector = !subColorSelector;
            }

            rowYPos += ItemHeight;
        }

        // moving
        if (KeyframeGrabbed) {
            ImGui::CaptureMouseFromApp();
            int diffFrame = int((cx - movingPos) / framePixelWidth);
            if (std::abs(diffFrame) > 0) {
                for (auto&& context : m_selectedKeyframes) {
                    context.current->m_frame += diffFrame;
                    if (context.current->m_frame < 0) {
                        context.current->m_frame = 0;
                    }
                }
                movingPos += int(diffFrame * framePixelWidth);
                m_editorLayer.Refresh();
            }
            if (!io.MouseDown[0]) {
                KeyframeGrabbed = false;
                EndMoveKeyframes();
            }
        }

        if (pendingSelectionClear && !ImGui::IsPopupOpen("keyframe_popup")) {
            ClearSelectedKeyframes();
        }

        //// draw item names in the legend rect on the left
        //float rowYPos = contentMin.y + 2;
        //for (int i = 0; i < childCount + 1; i++) {
        //    if (i == 0) {
        //        ImVec2 tpos(contentMin.x + 3, rowYPos);
        //        draw_list->AddText(tpos, 0xFFFFFFFF, "Clips");
        //        rowYPos += ItemHeight;
        //    } else {
        //        int childIndex = i - 1;
        //        auto childEntity = m_group->GetChildren()[childIndex]->GetLayoutEntity();
        //        {
        //            ImVec2 tpos(contentMin.x + 3, rowYPos);
        //            draw_list->AddText(tpos, 0xFFFFFFFF, GetChildLabel(childIndex));
        //            ImVec2 tpos2(tpos.x + childFrameSize.x, tpos.y + ItemHeight);
        //            ImRect labelRect(tpos, tpos2);
        //            if (labelRect.Contains(io.MousePos) && io.MouseClicked[0]) {
        //                m_childExpanded[childIndex] = !m_childExpanded[childIndex];
        //            }
        //            rowYPos += ItemHeight;
        //        }
        //        if (m_childExpanded[childIndex]) {
        //            auto& childTracks = childEntity->GetAnimationTracks();
        //            for (auto&& [target, track] : childTracks) {
        //                ImVec2 tpos(contentMin.x + 23, rowYPos);
        //                draw_list->AddText(tpos, 0xFFFFFFFF, GetTrackLabel(target));
        //                rowYPos += ItemHeight;
        //            }
        //        }
        //    }
        //}

        //// tracks background
        //rowYPos = contentMin.y + ItemHeight + 1;
        //for (int i = 0; i < childCount; i++) {
        //    unsigned int col = (i & 1) ? 0xFF3A3636 : 0xFF413D3D;

        //    ImVec2 pos = ImVec2(contentMin.x + legendWidth, rowYPos);
        //    rowYPos += ItemHeight;
        //    ImVec2 sz = ImVec2(canvas_size.x + canvas_pos.x, rowYPos);
        //    draw_list->AddRectFilled(pos, sz, col, 0);

        //    if (m_childExpanded[i]) {
        //        auto childEntity = m_group->GetChildren()[i]->GetLayoutEntity();
        //        int childTrackCount = static_cast<int>(childEntity->GetAnimationTracks().size());
        //        ImVec2 pos = ImVec2(contentMin.x + legendWidth, rowYPos);
        //        rowYPos += ItemHeight * childTrackCount;
        //        ImVec2 sz = ImVec2(canvas_size.x + canvas_pos.x, rowYPos);
        //        unsigned int col = 0xFF4B4747;
        //        draw_list->AddRectFilled(pos, sz, col, 0);
        //    }
        //}

        //// keyframes
        //rowYPos = contentMin.y + ItemHeight + 1;
        //for (int i = 0; i < childCount; i++) {
        //    auto& childTracks = m_group->GetChildren()[i]->GetLayoutEntity()->GetAnimationTracks();

        //    unsigned int keyframeColor = 0xFFc4c4c4;
        //    bool childExpanded = m_childExpanded[i];
        //    ImVec2 childTrackPos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, rowYPos);
        //    float trackYOffset = static_cast<float>(ItemHeight);
        //    for (auto&& [target, track] : childTracks) {
        //        float childRowYPos = childTrackPos.y + (childExpanded ? trackYOffset : 0);
        //        for (auto&& keyframe : track->GetKeyframes()) {
        //            ImVec2 keyP1(childTrackPos.x + keyframe.m_frame * framePixelWidth, childRowYPos + 2);
        //            ImVec2 keyP2(childTrackPos.x + keyframe.m_frame * framePixelWidth + framePixelWidth, childRowYPos + ItemHeight - 2);
        //            ImRect keyRect(keyP1, keyP2);
        //            if (keyP1.x <= (canvas_size.x + contentMin.x) && keyP2.x >= (contentMin.x + legendWidth)) {
        //                draw_list->AddRectFilled(keyP1, keyP2, keyframeColor, 2);
        //            }
        //            if (keyRect.Contains(io.MousePos) && io.MouseDoubleClicked[0]) {
        //                //animation->DoubleClickKey(i, k);
        //            }
        //            //if (movingEntry == -1) {
        //            //    if (!keyRect.Contains(io.MousePos))
        //            //        continue;
        //            //    if (!ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos))
        //            //        continue;
        //            //    if (ImGui::IsMouseClicked(0) && !MovingScrollBar && !MovingCurrentFrame) {
        //            //        movingEntry = i;
        //            //        movingPos = cx;
        //            //        movingPart = k;
        //            //        animation->BeginEdit(movingEntry);
        //            //        break;
        //            //    }
        //            //}
        //        }
        //        trackYOffset += ItemHeight;
        //    }
        //    rowYPos += ItemHeight;
        //}

        ////// moving
        ////if (movingEntry >= 0) {
        ////    ImGui::CaptureMouseFromApp();
        ////    int diffFrame = int((cx - movingPos) / framePixelWidth);
        ////    if (std::abs(diffFrame) > 0) {
        ////        int* keyframes;
        ////        animation->GetKeyframes(movingEntry, &keyframes, NULL);
        ////        int& k = keyframes[movingPart];
        ////        k += diffFrame;
        ////        if (k < 0) {
        ////            k = 0;
        ////        }
        ////        movingPos += int(diffFrame * framePixelWidth);
        ////    }
        ////    if (!io.MouseDown[0]) {
        ////        movingEntry = -1;
        ////        animation->EndEdit();
        ////    }
        ////}

        // vertical frame lines in content area
        for (int i = m_minFrame; i <= m_maxFrame; i += frameStep) {
            drawLineContent(i);
        }
        drawLineContent(m_minFrame);
        drawLineContent(m_maxFrame);

        // cursor
        if (m_currentFrame >= m_firstFrame && m_currentFrame <= m_maxFrame) {
            float cursorOffset = contentMin.x + legendWidth + (m_currentFrame - firstFrameUsed) * framePixelWidth + framePixelWidth / 2;
            draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), 0xA02A2AFF, framePixelWidth);
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", m_currentFrame);
            draw_list->AddText(ImVec2(cursorOffset + 10, canvas_pos.y + 2), 0xFF2A2AFF, tmps);
        }

        ImGui::EndChildFrame();
        ImGui::PopStyleColor();
        if (hasScrollBar) {
            ImGui::InvisibleButton("scrollBar", scrollBarSize);
            ImVec2 scrollBarMin = ImGui::GetItemRectMin();
            ImVec2 scrollBarMax = ImGui::GetItemRectMax();

            // ratio = number of frames visible in control / number to total frames

            float startFrameOffset = ((float)(firstFrameUsed - m_minFrame) / (float)frameCount) * (canvas_size.x - legendWidth);
            ImVec2 scrollBarA(scrollBarMin.x + legendWidth, scrollBarMin.y - 2);
            ImVec2 scrollBarB(scrollBarMin.x + canvas_size.x, scrollBarMax.y - 1);
            draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF222222, 0);

            ImRect scrollBarRect(scrollBarA, scrollBarB);
            bool inScrollBar = scrollBarRect.Contains(io.MousePos);

            draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF101010, 8);


            ImVec2 scrollBarC(scrollBarMin.x + legendWidth + startFrameOffset, scrollBarMin.y);
            ImVec2 scrollBarD(scrollBarMin.x + legendWidth + barWidthInPixels + startFrameOffset, scrollBarMax.y - 2);
            draw_list->AddRectFilled(scrollBarC, scrollBarD, (inScrollBar || MovingScrollBar) ? 0xFF606060 : 0xFF505050, 6);

            ImRect barHandleLeft(scrollBarC, ImVec2(scrollBarC.x + 14, scrollBarD.y));
            ImRect barHandleRight(ImVec2(scrollBarD.x - 14, scrollBarC.y), scrollBarD);

            bool onLeft = barHandleLeft.Contains(io.MousePos);
            bool onRight = barHandleRight.Contains(io.MousePos);

            static bool sizingRBar = false;
            static bool sizingLBar = false;

            draw_list->AddRectFilled(barHandleLeft.Min, barHandleLeft.Max, (onLeft || sizingLBar) ? 0xFFAAAAAA : 0xFF666666, 6);
            draw_list->AddRectFilled(barHandleRight.Min, barHandleRight.Max, (onRight || sizingRBar) ? 0xFFAAAAAA : 0xFF666666, 6);

            ImRect scrollBarThumb(scrollBarC, scrollBarD);
            static const float MinBarWidth = 44.f;
            if (sizingRBar) {
                if (!io.MouseDown[0]) {
                    sizingRBar = false;
                } else {
                    float barNewWidth = ImMax(barWidthInPixels + io.MouseDelta.x, MinBarWidth);
                    float barRatio = barNewWidth / barWidthInPixels;
                    framePixelWidthTarget = framePixelWidth = framePixelWidth / barRatio;
                    int newVisibleFrameCount = int((canvas_size.x - legendWidth) / framePixelWidthTarget);
                    int lastFrame = m_firstFrame + newVisibleFrameCount;
                    if (lastFrame > m_maxFrame) {
                        framePixelWidthTarget = framePixelWidth = (canvas_size.x - legendWidth) / float(m_maxFrame - m_firstFrame);
                    }
                }
            } else if (sizingLBar) {
                if (!io.MouseDown[0]) {
                    sizingLBar = false;
                } else {
                    if (fabsf(io.MouseDelta.x) > FLT_EPSILON) {
                        float barNewWidth = ImMax(barWidthInPixels - io.MouseDelta.x, MinBarWidth);
                        float barRatio = barNewWidth / barWidthInPixels;
                        float previousFramePixelWidthTarget = framePixelWidthTarget;
                        framePixelWidthTarget = framePixelWidth = framePixelWidth / barRatio;
                        int newVisibleFrameCount = int(visibleFrameCount / barRatio);
                        int newFirstFrame = m_firstFrame + newVisibleFrameCount - visibleFrameCount;
                        newFirstFrame = ImClamp(newFirstFrame, m_minFrame, ImMax(m_maxFrame - visibleFrameCount, m_minFrame));
                        if (newFirstFrame == m_firstFrame) {
                            framePixelWidth = framePixelWidthTarget = previousFramePixelWidthTarget;
                        } else {
                            m_firstFrame = newFirstFrame;
                        }
                    }
                }
            } else {
                if (MovingScrollBar) {
                    if (!io.MouseDown[0]) {
                        MovingScrollBar = false;
                    } else {
                        float framesPerPixelInBar = barWidthInPixels / (float)visibleFrameCount;
                        m_firstFrame = int((io.MousePos.x - panningViewSource.x) / framesPerPixelInBar) - panningViewFrame;
                        m_firstFrame = ImClamp(m_firstFrame, m_minFrame, ImMax(m_maxFrame - visibleFrameCount, m_minFrame));
                    }
                } else {
                    if (scrollBarThumb.Contains(io.MousePos) && ImGui::IsMouseClicked(0) && !MovingCurrentFrame && movingEntry == -1) {
                        MovingScrollBar = true;
                        panningViewSource = io.MousePos;
                        panningViewFrame = -m_firstFrame;
                    }
                    if (!sizingRBar && onRight && ImGui::IsMouseClicked(0))
                        sizingRBar = true;
                    if (!sizingLBar && onLeft && ImGui::IsMouseClicked(0))
                        sizingLBar = true;
                }
            }
        }

        ImGui::EndGroup();

        return ret;
    }
}
