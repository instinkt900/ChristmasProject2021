#pragma once

#include "events/event_listener.h"
#include "widget_types.h"
#include "widget_desc.h"

class WidgetTracks;

using WidgetRef = std::shared_ptr<class Widget>;

class Widget : public EventListener {
public:
    Widget();
    Widget(WidgetDesc const& desc);
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

    bool HasAnimation(std::string const& name);
    bool SetAnimation(std::string const& name);
    void StopAnimation();

    void DebugDraw();

    static WidgetRef CreateWidget(WidgetDesc const& desc);

protected:
    std::string m_id;
    bool m_visible = true;
    bool m_showRect = false;
    Widget* m_parent = nullptr;
    std::vector<WidgetRef> m_children;

    WidgetBounds m_layoutBounds;
    WidgetRect m_screenRect;

    std::unique_ptr<WidgetTracks> m_tracks;

    friend WidgetTracks;
};
