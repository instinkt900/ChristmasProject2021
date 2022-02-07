#include "game_pch.h"
#include "im_animation_tracks.h"
#include "imgui_internal.h"

namespace ImAnimation {
#ifndef IMGUI_DEFINE_MATH_OPERATORS
    static ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
        return ImVec2(a.x + b.x, a.y + b.y);
    }
#endif

    static bool SequencerAddDelButton(ImDrawList* draw_list, ImVec2 pos, bool add = true) {
        ImGuiIO& io = ImGui::GetIO();
        ImRect delRect(pos, ImVec2(pos.x + 16, pos.y + 16));
        bool overDel = delRect.Contains(io.MousePos);
        int delColor = overDel ? 0xFFAAAAAA : 0x50000000;
        float midy = pos.y + 16 / 2 - 0.5f;
        float midx = pos.x + 16 / 2 - 0.5f;
        draw_list->AddRect(delRect.Min, delRect.Max, delColor, 4);
        draw_list->AddLine(ImVec2(delRect.Min.x + 3, midy), ImVec2(delRect.Max.x - 3, midy), delColor, 2);
        if (add)
            draw_list->AddLine(ImVec2(midx, delRect.Min.y + 3), ImVec2(midx, delRect.Max.y - 3), delColor, 2);
        return overDel;
    }

    bool Animation(IAnimation* animation, int* currentFrame, bool* expanded, int minFrame, int maxFrame, int* firstFrame) {
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

        bool popupOpened = false;
        int trackCount = animation->GetTrackCount();
        //if (!trackCount)
        //    return false;
        ImGui::BeginGroup();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();     // ImDrawList API uses screen coordinates!
        ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // Resize canvas to what's available
        int firstFrameUsed = firstFrame ? *firstFrame : 0;


        int controlHeight = (trackCount + 1) * ItemHeight;
        int frameCount = ImMax(maxFrame - minFrame, 1);

        static bool MovingScrollBar = false;
        static bool MovingCurrentFrame = false;
        struct CustomDraw {
            int index;
            ImRect customRect;
            ImRect legendRect;
            ImRect clippingRect;
            ImRect legendClippingRect;
        };
        ImVector<CustomDraw> customDraws;
        ImVector<CustomDraw> compactCustomDraws;
        // zoom in/out
        const int visibleFrameCount = (int)floorf((canvas_size.x - legendWidth) / framePixelWidth);
        const float barWidthRatio = ImMin(visibleFrameCount / (float)frameCount, 1.f);
        const float barWidthInPixels = barWidthRatio * (canvas_size.x - legendWidth);

        ImRect regionRect(canvas_pos, canvas_pos + canvas_size);

        static bool panningView = false;
        static ImVec2 panningViewSource;
        static int panningViewFrame;
        if (ImGui::IsWindowFocused() && io.KeyAlt && io.MouseDown[2]) {
            if (!panningView) {
                panningViewSource = io.MousePos;
                panningView = true;
                panningViewFrame = *firstFrame;
            }
            *firstFrame = panningViewFrame - int((io.MousePos.x - panningViewSource.x) / framePixelWidth);
            *firstFrame = ImClamp(*firstFrame, minFrame, maxFrame - visibleFrameCount);
        }
        if (panningView && !io.MouseDown[2]) {
            panningView = false;
        }
        framePixelWidthTarget = ImClamp(framePixelWidthTarget, 0.1f, 50.f);

        framePixelWidth = ImLerp(framePixelWidth, framePixelWidthTarget, 0.33f);

        frameCount = maxFrame - minFrame;
        if (visibleFrameCount >= frameCount && firstFrame)
            *firstFrame = minFrame;


        // --
        if (expanded && !*expanded) {
            ImGui::InvisibleButton("canvas", ImVec2(canvas_size.x - canvas_pos.x, (float)ItemHeight));
            draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d Frames / %d entries", frameCount, trackCount);
            draw_list->AddText(ImVec2(canvas_pos.x + 26, canvas_pos.y + 2), 0xFFFFFFFF, tmps);
        } else {
            bool hasScrollBar(true);
            /*
         int framesPixelWidth = int(frameCount * framePixelWidth);
         if ((framesPixelWidth + legendWidth) >= canvas_size.x)
         {
             hasScrollBar = true;
         }
         */
            // test scroll area
            ImVec2 headerSize(canvas_size.x, (float)ItemHeight);
            ImVec2 scrollBarSize(canvas_size.x, 14.f);
            ImGui::InvisibleButton("topBar", headerSize);
            draw_list->AddRectFilled(canvas_pos, canvas_pos + headerSize, 0xFFFF0000, 0);
            ImVec2 childFramePos = ImGui::GetCursorScreenPos();
            ImVec2 childFrameSize(canvas_size.x, canvas_size.y - 8.f - headerSize.y - (hasScrollBar ? scrollBarSize.y : 0));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
            ImGui::BeginChildFrame(889, childFrameSize);
            animation->focused = ImGui::IsWindowFocused();
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
            if (!MovingCurrentFrame && !MovingScrollBar && movingEntry == -1 && movingClipEntry == -1 && currentFrame && *currentFrame >= 0 && topRect.Contains(io.MousePos) && io.MouseDown[0]) {
                MovingCurrentFrame = true;
            }
            if (MovingCurrentFrame) {
                if (frameCount) {
                    *currentFrame = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
                    if (*currentFrame < minFrame)
                        *currentFrame = minFrame;
                    if (*currentFrame >= maxFrame)
                        *currentFrame = maxFrame;
                }
                if (!io.MouseDown[0])
                    MovingCurrentFrame = false;
            }

            //header
            draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + ItemHeight), 0xFF3D3837, 0);

            // clips
            int clipCount = animation->GetClipCount();
            for (int i = 0; i < clipCount; i++) {
                int* startFrame;
                int* endFrame;
                char const* clipName;
                unsigned int clipColor;
                animation->GetClip(i, &startFrame, &endFrame, &clipName, &clipColor);

                int px = (int)contentMin.x + legendWidth - int(firstFrameUsed * framePixelWidth);
                int py = (int)contentMin.y;
                ImVec2 slotP1(px + *startFrame * framePixelWidth, py + 2.0f);
                ImVec2 slotP2(px + *endFrame * framePixelWidth + framePixelWidth, py + ItemHeight - 2.0f);
                unsigned int slotColor = clipColor; //0xFFFF0000;
                //unsigned int slotColorHalf = 0x40FF0000;

                if (slotP1.x <= (canvas_size.x + contentMin.x) && slotP2.x >= (contentMin.x + legendWidth)) {
                    draw_list->AddRectFilled(slotP1, slotP2, slotColor, 2);
                }

                if (ImRect(slotP1, slotP2).Contains(io.MousePos) && io.MouseDoubleClicked[0]) {
                    animation->DoubleClickClip(i);
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
                            animation->BeginEditClip(movingClipEntry);
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
                    animation->GetClip(movingClipEntry, &start, &end, NULL, NULL);
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
                    animation->EndEditClip();
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
                bool baseIndex = ((i % modFrameCount) == 0) || (i == maxFrame || i == minFrame);
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

            auto drawLineContent = [&](int i, int /*regionHeight*/) {
                int px = int(contentMin.x + legendWidth + (i - firstFrameUsed) * framePixelWidth);
                int tiretStart = int(contentMin.y);
                int tiretEnd = int(contentMax.y);

                if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legendWidth)) {
                    //draw_list->AddLine(ImVec2((float)px, canvas_pos.y + (float)tiretStart), ImVec2((float)px, canvas_pos.y + (float)tiretEnd - 1), 0xFF606060, 1);

                    draw_list->AddLine(ImVec2(float(px), float(tiretStart)), ImVec2(float(px), float(tiretEnd)), 0x30606060, 1);
                }
            };

            for (int i = minFrame; i <= maxFrame; i += frameStep) {
                drawLine(i, ItemHeight);
            }
            drawLine(minFrame, ItemHeight);
            drawLine(maxFrame, ItemHeight);

            // clip content
            draw_list->PushClipRect(childFramePos, childFramePos + childFrameSize);

            // draw item names in the legend rect on the left
            size_t customHeight = 0;
            for (int i = 0; i < trackCount + 1; i++) {
                if (i == 0) {
                    ImVec2 tpos(contentMin.x + 3, contentMin.y + 2);
                    draw_list->AddText(tpos, 0xFFFFFFFF, "Clips");
                } else {
                    int index = i - 1;
                    ImVec2 tpos(contentMin.x + 3, contentMin.y + i * ItemHeight + 2 + customHeight);
                    draw_list->AddText(tpos, 0xFFFFFFFF, animation->GetTrackLabel(index));
                }
            }

            // clipping rect so items bars are not visible in the legend on the left when scrolled
            //

            // tracks background
            customHeight = ItemHeight;
            for (int i = 0; i < trackCount; i++) {
                unsigned int col = (i & 1) ? 0xFF3A3636 : 0xFF413D3D;

                ImVec2 pos = ImVec2(contentMin.x + legendWidth, contentMin.y + ItemHeight * i + 1 + customHeight);
                ImVec2 sz = ImVec2(canvas_size.x + canvas_pos.x, pos.y + ItemHeight - 1);
                draw_list->AddRectFilled(pos, sz, col, 0);
            }

            draw_list->PushClipRect(childFramePos + ImVec2(float(legendWidth), 0.f), childFramePos + childFrameSize);

            // vertical frame lines in content area
            for (int i = minFrame; i <= maxFrame; i += frameStep) {
                drawLineContent(i, int(contentHeight));
            }
            drawLineContent(minFrame, int(contentHeight));
            drawLineContent(maxFrame, int(contentHeight));

            // keyframes
            customHeight = 0;
            for (int i = 0; i < trackCount; i++) {
                int* keyframes;
                int keyframeCount;
                animation->GetKeyframes(i, &keyframes, &keyframeCount);

                unsigned int keyframeColor = 0xFFc4c4c4;
                ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + ItemHeight * (i + 1) + 1 + customHeight);
                for (int k = 0; k < keyframeCount; k++) {
                    ImVec2 keyP1(pos.x + keyframes[k] * framePixelWidth, pos.y + 2);
                    ImVec2 keyP2(pos.x + keyframes[k] * framePixelWidth + framePixelWidth, pos.y + ItemHeight - 2);
                    ImRect keyRect(keyP1, keyP2);
                    if (keyP1.x <= (canvas_size.x + contentMin.x) && keyP2.x >= (contentMin.x + legendWidth)) {
                        draw_list->AddRectFilled(keyP1, keyP2, keyframeColor, 2);
                    }
                    if (keyRect.Contains(io.MousePos) && io.MouseDoubleClicked[0]) {
                        animation->DoubleClickKey(i, k);
                    }
                    if (movingEntry == -1) {
                        if (!keyRect.Contains(io.MousePos))
                            continue;
                        if (!ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos))
                            continue;
                        if (ImGui::IsMouseClicked(0) && !MovingScrollBar && !MovingCurrentFrame) {
                            movingEntry = i;
                            movingPos = cx;
                            movingPart = k;
                            animation->BeginEdit(movingEntry);
                            break;
                        }
                    }
                }
            }


            // moving
            if (movingEntry >= 0) {
                ImGui::CaptureMouseFromApp();
                int diffFrame = int((cx - movingPos) / framePixelWidth);
                if (std::abs(diffFrame) > 0) {
                    int* keyframes;
                    animation->GetKeyframes(movingEntry, &keyframes, NULL);
                    int& k = keyframes[movingPart];
                    k += diffFrame;
                    if (k < 0) {
                        k = 0;
                    }
                    movingPos += int(diffFrame * framePixelWidth);
                }
                if (!io.MouseDown[0]) {
                    movingEntry = -1;
                    animation->EndEdit();
                }
            }

            // cursor
            if (currentFrame && firstFrame && *currentFrame >= *firstFrame && *currentFrame <= maxFrame) {
                float cursorOffset = contentMin.x + legendWidth + (*currentFrame - firstFrameUsed) * framePixelWidth + framePixelWidth / 2;
                draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), 0xA02A2AFF, framePixelWidth);
                //draw_list->AddRectFilled(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset + framePixelWidth, contentMax.y), 0xA02A2AFF, 2);
                char tmps[512];
                ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", *currentFrame);
                draw_list->AddText(ImVec2(cursorOffset + 10, canvas_pos.y + 2), 0xFF2A2AFF, tmps);
            }

            draw_list->PopClipRect();
            draw_list->PopClipRect();

            ImGui::EndChildFrame();
            ImGui::PopStyleColor();
            if (hasScrollBar) {
                ImGui::InvisibleButton("scrollBar", scrollBarSize);
                ImVec2 scrollBarMin = ImGui::GetItemRectMin();
                ImVec2 scrollBarMax = ImGui::GetItemRectMax();

                // ratio = number of frames visible in control / number to total frames

                float startFrameOffset = ((float)(firstFrameUsed - minFrame) / (float)frameCount) * (canvas_size.x - legendWidth);
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
                        int lastFrame = *firstFrame + newVisibleFrameCount;
                        if (lastFrame > maxFrame) {
                            framePixelWidthTarget = framePixelWidth = (canvas_size.x - legendWidth) / float(maxFrame - *firstFrame);
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
                            int newFirstFrame = *firstFrame + newVisibleFrameCount - visibleFrameCount;
                            newFirstFrame = ImClamp(newFirstFrame, minFrame, ImMax(maxFrame - visibleFrameCount, minFrame));
                            if (newFirstFrame == *firstFrame) {
                                framePixelWidth = framePixelWidthTarget = previousFramePixelWidthTarget;
                            } else {
                                *firstFrame = newFirstFrame;
                            }
                        }
                    }
                } else {
                    if (MovingScrollBar) {
                        if (!io.MouseDown[0]) {
                            MovingScrollBar = false;
                        } else {
                            float framesPerPixelInBar = barWidthInPixels / (float)visibleFrameCount;
                            *firstFrame = int((io.MousePos.x - panningViewSource.x) / framesPerPixelInBar) - panningViewFrame;
                            *firstFrame = ImClamp(*firstFrame, minFrame, ImMax(maxFrame - visibleFrameCount, minFrame));
                        }
                    } else {
                        if (scrollBarThumb.Contains(io.MousePos) && ImGui::IsMouseClicked(0) && firstFrame && !MovingCurrentFrame && movingEntry == -1) {
                            MovingScrollBar = true;
                            panningViewSource = io.MousePos;
                            panningViewFrame = -*firstFrame;
                        }
                        if (!sizingRBar && onRight && ImGui::IsMouseClicked(0))
                            sizingRBar = true;
                        if (!sizingLBar && onLeft && ImGui::IsMouseClicked(0))
                            sizingLBar = true;
                    }
                }
            }
        }

        ImGui::EndGroup();

        if (regionRect.Contains(io.MousePos)) {
            bool overCustomDraw = false;
            for (auto& custom : customDraws) {
                if (custom.customRect.Contains(io.MousePos)) {
                    overCustomDraw = true;
                }
            }
            if (overCustomDraw) {
            } else {
#if 0
            frameOverCursor = *firstFrame + (int)(visibleFrameCount * ((io.MousePos.x - (float)legendWidth - canvas_pos.x) / (canvas_size.x - legendWidth)));
            //frameOverCursor = max(min(*firstFrame - visibleFrameCount / 2, frameCount - visibleFrameCount), 0);

            /**firstFrame -= frameOverCursor;
            *firstFrame *= framePixelWidthTarget / framePixelWidth;
            *firstFrame += frameOverCursor;*/
            if (io.MouseWheel < -FLT_EPSILON)
            {
               *firstFrame -= frameOverCursor;
               *firstFrame = int(*firstFrame * 1.1f);
               framePixelWidthTarget *= 0.9f;
               *firstFrame += frameOverCursor;
            }

            if (io.MouseWheel > FLT_EPSILON)
            {
               *firstFrame -= frameOverCursor;
               *firstFrame = int(*firstFrame * 0.9f);
               framePixelWidthTarget *= 1.1f;
               *firstFrame += frameOverCursor;
            }
#endif
            }
        }

        if (expanded) {
            bool overExpanded = SequencerAddDelButton(draw_list, ImVec2(canvas_pos.x + 2, canvas_pos.y + 2), !*expanded);
            if (overExpanded && io.MouseReleased[0])
                *expanded = !*expanded;
        }

        return ret;
    }
}