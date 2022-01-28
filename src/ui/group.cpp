#include "game_pch.h"
#include "group.h"
#include "events/event_dispatch.h"
#include "debug/inspectors.h"
#include "inspectors.h"
#include "layouts/layout_entity_group.h"
#include "layouts/animation_clip.h"

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
            if (m_animTime >= m_currentAnimationClip->GetEndTime()) {
                switch (m_currentAnimationClip->GetLoopType()) {
                case AnimationClip::LoopType::Stop:
                    m_animTime = m_currentAnimationClip->GetEndTime();
                    m_currentAnimationClip = nullptr;
                    break;
                case AnimationClip::LoopType::Loop:
                    m_animTime -= m_currentAnimationClip->GetDuration();
                    break;
                case AnimationClip::LoopType::Reset:
                    m_animTime = m_currentAnimationClip->GetStartTime();
                    m_currentAnimationClip = nullptr;
                    break;
                }
            }

            for (auto&& child : m_children) {
                child->SetAnimTime(m_animTime);
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
            auto it = std::find_if(std::begin(animationClips), std::end(animationClips), [&name](auto& clip) { return clip->GetName() == name; });
            if (std::end(animationClips) != it) {
                m_currentAnimationClip = it->get();
                m_animTime = m_currentAnimationClip->GetStartTime();
                return true;
            }
        }
        return false;
    }

    void Group::DebugDraw() {
        Node::DebugDraw();
        ImGuiInspectMember("anim time", m_animTime);
        if (ImGui::TreeNode("Children")) {
            for (int i = 0; i < m_children.size(); ++i) {
                auto& child = m_children[i];
                auto const label = fmt::format("[{}]: {}", i, child->GetId());
                if (ImGui::TreeNode(label.c_str())) {
                    child->DebugDraw();
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }

    void Group::UpdateChildBounds() {
        for (auto&& child : m_children) {
            child->RecalculateBounds();
        }
    }
}
