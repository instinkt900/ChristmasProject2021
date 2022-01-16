#include "game_pch.h"
#include "group.h"
#include "events/event_dispatch.h"
#include "debug/inspectors.h"
#include "inspectors.h"
#include "layouts/layout_entity_group.h"
#include "layouts/animation_clip_info.h"

namespace ui {
    Group::Group() {
    }

    Group::Group(std::shared_ptr<LayoutEntityGroup> layoutEntityGroup)
        : Node(layoutEntityGroup) {
        for (auto&& childEntity : layoutEntityGroup->GetChildren()) {
            AddChild(childEntity->Instantiate());
        }
    }

    Group::~Group() {
    }

    bool Group::OnEvent(Event const& event) {
        if (Node::OnEvent(event)) {
            return true;
        }
        EventDispatch dispatch(event);
        for (auto&& child : m_children) {
            dispatch.Dispatch(child.get());
        }
        return dispatch.GetHandled();
    }

    void Group::Update(uint32_t ticks) {
        if (m_currentAnimationClip) {
            m_animTime += ticks / 1000.0f;
            if (m_animTime >= m_currentAnimationClip->GetDuration()) {
                switch (m_currentAnimationClip->GetLoopType()) {
                case AnimationClipInfo::LoopType::Stop:
                    m_currentAnimationClip = nullptr;
                    break;
                case AnimationClipInfo::LoopType::Loop:
                    m_animTime -= m_currentAnimationClip->GetDuration();
                    break;
                case AnimationClipInfo::LoopType::Reset:
                    m_animTime = 0;
                    m_currentAnimationClip = nullptr;
                    break;
                }
            }

            for (auto&& child : m_children) {
                child->SetAnimTime(m_animTime);
            }

            if (m_currentAnimationClip == nullptr) {
                for (auto&& child : m_children) {
                    child->DeactivateClip();
                }
            }
        }

        for (auto&& child : m_children) {
            child->Update(ticks);
        }
    }

    void Group::Draw(SDL_Renderer& renderer) {
        Node::Draw(renderer);
        for (auto&& child : m_children) {
            child->Draw(renderer);
        }
    }

    void Group::AddChild(std::shared_ptr<Node> child) {
        m_children.push_back(child);
        child->SetParent(this);
    }

    void Group::RemoveChild(std::shared_ptr<Node> child) {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (std::end(m_children) != it) {
            (*it)->SetParent(nullptr);
            m_children.erase(it);
        }
    }

    bool Group::SetAnimation(std::string const& name) {
        if (m_layout) {
            auto layout = std::static_pointer_cast<LayoutEntityGroup>(m_layout);
            auto& animationClips = layout->GetAnimationClips();
            auto it = animationClips.find(name);
            if (std::end(animationClips) != it) {
                m_currentAnimationClip = it->second.get();
                m_animTime = 0;
                for (auto&& child : m_children) {
                    child->ActivateClip(name);
                }
                return true;
            }
        }
        return false;
    }

    void Group::DebugDraw() {
        Node::DebugDraw();
        for (int i = 0; i < m_children.size(); ++i) {
            auto& child = m_children[i];
            auto const label = fmt::format("Child {}: {}", i, child->GetId());
            if (ImGui::TreeNode(label.c_str())) {
                child->DebugDraw();
                ImGui::TreePop();
            }
        }
    }

    void Group::UpdateChildBounds() {
        for (auto&& child : m_children) {
            child->RecalculateBounds();
        }
    }
}
