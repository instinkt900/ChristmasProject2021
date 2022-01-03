#pragma once

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

class Widget {
public:
    Widget();
    virtual ~Widget();

    virtual bool OnEvent(SDL_Event const& event);
    virtual void Update(uint32_t ticks);
    virtual void Draw(SDL_Renderer& renderer);

    void SetRect(WidgetRect const& rect);
    WidgetRect const& GetScreenRect() const { return m_screenRect; }
    WidgetBounds& GetLayoutBounds() { return m_layoutBounds; }

    void RecalculateBounds();

    void AddChild(WidgetRef widget);
    void RemoveChild(WidgetRef widget);
    std::vector<WidgetRef> const& GetChildren() const { return m_children; }

    void SetParent(Widget* parent) { m_parent = parent; }
    Widget* GetParent() const { return m_parent; }

protected:
    Widget* m_parent = nullptr;
    std::vector<WidgetRef> m_children;

    WidgetBounds m_layoutBounds;
    WidgetRect m_screenRect;
};
