#include "game_pch.h"
#include "bounds_widget.h"
#include "anchor_bounds_handle.h"
#include "offset_bounds_handle.h"
#include "events/event_dispatch.h"
#include "ui/node.h"
#include "ui/layouts/layout_entity.h"
#include "ui/editor/editor_layer.h"
#include "ui/editor/actions/composite_action.h"
#include "ui/editor/actions/change_keyframe_action.h"
#include "ui/editor/actions/add_keyframe_action.h"

namespace ui {
    BoundsWidget::BoundsWidget(EditorLayer& editorLayer)
        : m_editorLayer(editorLayer) {
        // anchor manipulators
        m_handles[0] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::TopLeft);
        m_handles[1] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::TopRight);
        m_handles[2] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::BottomLeft);
        m_handles[3] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::BottomRight);
        m_handles[4] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::Top);
        m_handles[5] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::Left);
        m_handles[6] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::Right);
        m_handles[7] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::Bottom);

        // offset manipulators
        m_handles[8] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::TopLeft);
        m_handles[9] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::TopRight);
        m_handles[10] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::BottomLeft);
        m_handles[11] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::BottomRight);
        m_handles[12] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::Top);
        m_handles[13] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::Left);
        m_handles[14] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::Right);
        m_handles[15] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::Bottom);
    }

    BoundsWidget::~BoundsWidget() {
    }

    bool BoundsWidget::OnEvent(Event const& event) {
        EventDispatch dispatch(event);
        for (auto&& handle : m_handles) {
            dispatch.Dispatch(handle.get());
        }
        dispatch.Dispatch(this, &BoundsWidget::OnMouseDown);
        dispatch.Dispatch(this, &BoundsWidget::OnMouseUp);
        dispatch.Dispatch(this, &BoundsWidget::OnMouseMove);
        return dispatch.GetHandled();
    }

    void BoundsWidget::SetSelection(std::shared_ptr<Node> selection) {
        m_holding = false;
        m_selectedNode = selection;

        for (auto&& handle : m_handles) {
            handle->SetTarget(selection.get());
        }

        if (selection) {
            BeginEdit();
            m_holding = true;
        }
    }

    void BoundsWidget::BeginEdit() {
        auto selection = m_selectedNode.lock();
        assert(!m_editContext && selection);
        auto entity = selection->GetLayoutEntity();
        m_editContext = std::make_unique<EditContext>();
        m_editContext->entity = entity;
        m_editContext->originalRect = selection->GetLayoutRect();
    }

    void SetTrackValue(CompositeAction& compositeAction, std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo, float value) {
        auto& tracks = entity->GetAnimationTracks();
        auto track = tracks.at(target);
        if (auto keyframePtr = track->GetKeyframe(frameNo)) {
            // keyframe exists
            float oldValue = keyframePtr->m_value;
            keyframePtr->m_value = value;
            compositeAction.GetActions().push_back(std::make_unique<ChangeKeyframeAction>(entity, target, frameNo, oldValue, value));
        } else {
            // no keyframe
            auto& keyframe = track->GetOrCreateKeyframe(frameNo);
            keyframe.m_value = value;
            compositeAction.GetActions().push_back(std::make_unique<AddKeyframeAction>(entity, target, frameNo, value));
        }
    }

    void BoundsWidget::EndEdit() {
        auto selection = m_selectedNode.lock();
        assert(m_editContext && selection);
        auto entity = selection->GetLayoutEntity();
        auto& tracks = entity->GetAnimationTracks();
        int frameNo = m_editorLayer.GetSelectedFrame();
        auto const& newRect = selection->GetLayoutRect();
        auto rectDelta = newRect - m_editContext->originalRect;

        std::unique_ptr<CompositeAction> editAction = std::make_unique<CompositeAction>();
        if (rectDelta.anchor.topLeft.x != 0) {
            SetTrackValue(*editAction, entity, AnimationTrack::Target::LeftAnchor, frameNo, newRect.anchor.topLeft.x);
        }
        if (rectDelta.anchor.topLeft.y != 0) {
            SetTrackValue(*editAction, entity, AnimationTrack::Target::TopAnchor, frameNo, newRect.anchor.topLeft.y);
        }
        if (rectDelta.anchor.bottomRight.x != 0) {
            SetTrackValue(*editAction, entity, AnimationTrack::Target::RightAnchor, frameNo, newRect.anchor.bottomRight.x);
        }
        if (rectDelta.anchor.bottomRight.y != 0) {
            SetTrackValue(*editAction, entity, AnimationTrack::Target::BottomAnchor, frameNo, newRect.anchor.bottomRight.y);
        }
        if (rectDelta.offset.topLeft.x != 0) {
            SetTrackValue(*editAction, entity, AnimationTrack::Target::LeftOffset, frameNo, newRect.offset.topLeft.x);
        }
        if (rectDelta.offset.topLeft.y != 0) {
            SetTrackValue(*editAction, entity, AnimationTrack::Target::TopOffset, frameNo, newRect.offset.topLeft.y);
        }
        if (rectDelta.offset.bottomRight.x != 0) {
            SetTrackValue(*editAction, entity, AnimationTrack::Target::RightOffset, frameNo, newRect.offset.bottomRight.x);
        }
        if (rectDelta.offset.bottomRight.y != 0) {
            SetTrackValue(*editAction, entity, AnimationTrack::Target::BottomOffset, frameNo, newRect.offset.bottomRight.y);
        }

        if (!editAction->GetActions().empty()) {
            m_editorLayer.AddEditAction(std::move(editAction));
        }

        m_editContext.reset();
    }

    void BoundsWidget::Draw(SDL_Renderer& renderer) {
        CheckSelectionValid();

        auto selection = m_selectedNode.lock();
        if (selection && selection->GetParent()) {
            SDL_SetRenderDrawColor(&renderer, 0x00, 0x00, 0xFF, 0xFF);
            auto const& screenRect = selection->GetScreenRect();
            auto const width = screenRect.bottomRight.x - screenRect.topLeft.x;
            auto const height = screenRect.bottomRight.y - screenRect.topLeft.y;
            SDL_Rect const rect{ screenRect.topLeft.x, screenRect.topLeft.y, width, height };
            SDL_RenderDrawRect(&renderer, &rect);
            for (auto& handle : m_handles) {
                handle->Draw(renderer);
            }
        }
    }

    bool BoundsWidget::OnMouseDown(EventMouseDown const& event) {
        CheckSelectionValid();

        auto selection = m_selectedNode.lock();
        if (!selection || event.GetButton() != MouseButton::Left) {
            return false;
        }

        if (IsInRect(event.GetPosition(), selection->GetScreenRect())) {
            m_holding = true;
            BeginEdit();
            return true;
        }

        return false;
    }

    bool BoundsWidget::OnMouseUp(EventMouseUp const& event) {
        CheckSelectionValid();

        if (event.GetButton() != MouseButton::Left) {
            return false;
        }
        if (m_holding) {
            m_holding = false;
            EndEdit();
        }
        return false;
    }

    bool BoundsWidget::OnMouseMove(EventMouseMove const& event) {
        if (m_holding) {
            auto selection = m_selectedNode.lock();
            auto& bounds = selection->GetLayoutRect();
            bounds.offset.topLeft.x += event.GetDelta().x;
            bounds.offset.bottomRight.x += event.GetDelta().x;
            bounds.offset.topLeft.y += event.GetDelta().y;
            bounds.offset.bottomRight.y += event.GetDelta().y;
            selection->RecalculateBounds();
        }
        return false;
    }

    void BoundsWidget::CheckSelectionValid() {
        auto selection = m_selectedNode.lock();
        if (!selection || !selection->GetParent() || !selection->IsVisible()) {
            SetSelection(nullptr);
        }
    }
}
