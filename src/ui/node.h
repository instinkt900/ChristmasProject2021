#pragma once

#include "events/event_listener.h"
#include "layout_types.h"
#include "layout_entity.h"

class LayoutEntity;
class Group;

class Node : public EventListener {
public:
    Node();
    Node(std::shared_ptr<LayoutEntity> layoutEntity);
    virtual ~Node();

    virtual bool OnEvent(Event const& event) override;
    virtual void Update(uint32_t ticks);
    virtual void Draw(SDL_Renderer& renderer);

    void SetId(std::string const& id) { m_id = id; }
    std::string const& GetId() const { return m_id; }

    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    void SetParent(Group* parent) { m_parent = parent; }
    Group* GetParent() const { return m_parent; }

    LayoutRect& GetLayoutRect() { return m_layoutRect; }
    IntRect const& GetScreenRect() const { return m_screenRect; }
    void SetScreenRect(IntRect const& rect);
    virtual void UpdateChildBounds() {}

    void RecalculateBounds();
    bool IsInBounds(IntVec2 const& point) const;
    IntVec2 TranslatePosition(IntVec2 const& point) const;

    virtual bool SetAnimation(std::string const& name) { return false; }
    void ActivateClip(std::string const& name);
    void DeactivateClip();
    void SetAnimTime(float time);

    virtual void DebugDraw();

protected:
    std::shared_ptr<LayoutEntity> m_layout;

    Group* m_parent = nullptr;

    std::string m_id;
    LayoutRect m_layoutRect;

    bool m_visible = true;
    bool m_showRect = false;
    bool m_overrideScreenRect = false;
    IntRect m_screenRect;

    AnimationClip* m_currentAnimationClip = nullptr;
};
