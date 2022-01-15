#include "game_pch.h"
#include "group.h"
#include "events/event_dispatch.h"
#include "debug/inspectors.h"
#include "inspectors.h"
#include "layout_entity.h"

Group::Group() {
}

Group::Group(LayoutEntityGroup const& layoutEntityGroup)
    : Node(layoutEntityGroup) {
    for (auto&& childEntity : layoutEntityGroup.GetChildren()) {
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
