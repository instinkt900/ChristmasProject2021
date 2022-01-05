#pragma once

#include "events/event_listener.h"

struct WidgetVertex {
    FloatVec2 anchor;
    IntVec2 offset;
};

struct WidgetBounds {
    WidgetVertex topLeft{ 0.0f, 0.0f, 0, 0 };
    WidgetVertex bottomRight{ 1.0f, 1.0f, 0, 0 };
};

struct WidgetRect {
    IntVec2 topLeft{ 0, 0 };
    IntVec2 bottomRight{ 0, 0 };
};

using WidgetRef = std::shared_ptr<class Widget>;

class Widget : public EventListener {
public:
    Widget();
    virtual ~Widget();

    virtual bool OnEvent(Event const& event) override;
    virtual void Update(uint32_t ticks);
    virtual void Draw(SDL_Renderer& renderer);

    void SetId(std::string const& id) { m_id = id; }
    std::string const& GetId() const { return m_id; }

    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    void SetParent(Widget* parent) { m_parent = parent; }
    Widget* GetParent() const { return m_parent; }

    void AddChild(WidgetRef widget);
    void RemoveChild(WidgetRef widget);
    std::vector<WidgetRef> const& GetChildren() const { return m_children; }

    WidgetBounds& GetLayoutBounds() { return m_layoutBounds; }
    void SetScreenRect(WidgetRect const& rect);
    WidgetRect const& GetScreenRect() const { return m_screenRect; }
    
    void RecalculateBounds(bool propagate);
    bool IsInBounds(IntVec2 const& point) const;
    IntVec2 TranslatePosition(IntVec2 const& point) const;

protected:
    std::string m_id;
    bool m_visible = true;
    Widget* m_parent = nullptr;
    std::vector<WidgetRef> m_children;

    WidgetBounds m_layoutBounds;
    WidgetRect m_screenRect;
};
