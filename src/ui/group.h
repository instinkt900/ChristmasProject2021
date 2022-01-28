#pragma once

#include "node.h"

namespace ui {

    class LayoutEntityGroup;
    struct AnimationClip;

    class Group : public Node {
    public:
        Group();
        Group(std::shared_ptr<LayoutEntityGroup> layoutEntityGroup);
        virtual ~Group();

        bool OnEvent(Event const& event) override;
        void Update(uint32_t ticks) override;
        void Draw(SDL_Renderer& renderer) override;

        void UpdateChildBounds() override;

        void AddChild(std::shared_ptr<Node> child);
        void RemoveChild(std::shared_ptr<Node> child);
        auto& GetChildren() { return m_children; }
        auto const& GetChildren() const { return m_children; }

        bool SetAnimation(std::string const& name) override;
        void SetAnimTime(float time) override;
        void SetAnimFrame(int frame) override;

        void DebugDraw() override;

    protected:
        std::vector<std::shared_ptr<Node>> m_children;
        AnimationClip* m_currentAnimationClip = nullptr;
        float m_animTime = 0;
    };
}
