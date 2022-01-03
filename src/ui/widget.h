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

    virtual bool OnEvent(Event const& event);
    virtual void Update(uint32_t ticks);
    virtual void Draw(SDL_Renderer& renderer);

    void SetId(std::string const& id) { m_id = id; }
    std::string const& GetId() const { return m_id; }

    void SetParent(Widget* parent) { m_parent = parent; }
    Widget* GetParent() const { return m_parent; }

    void AddChild(WidgetRef widget);
    void RemoveChild(WidgetRef widget);
    std::vector<WidgetRef> const& GetChildren() const { return m_children; }

    WidgetBounds& GetLayoutBounds() { return m_layoutBounds; }
    void SetScreenRect(WidgetRect const& rect);
    WidgetRect const& GetScreenRect() const { return m_screenRect; }
    
    void RecalculateBounds();

protected:
    std::string m_id;
    Widget* m_parent = nullptr;
    std::vector<WidgetRef> m_children;

    WidgetBounds m_layoutBounds;
    WidgetRect m_screenRect;
};
