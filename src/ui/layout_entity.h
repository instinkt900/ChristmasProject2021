#pragma once

#include "layout_types.h"

class LayoutEntityGroup;
class Node;

class LayoutEntity {
public:
    explicit LayoutEntity(nlohmann::json const& json);

    std::string GetId() const { return m_id; }
    LayoutRect const& GetBounds() const { return m_bounds; }

    void SetParent(LayoutEntity* parent) { m_parent = parent; }
    LayoutEntity* GetParent() const { return m_parent; }

    virtual std::unique_ptr<Node> Instantiate();

protected:
    std::string m_id;
    LayoutRect m_bounds;
    LayoutEntity* m_parent = nullptr;
};

class LayoutEntityGroup : public LayoutEntity {
public:
    explicit LayoutEntityGroup(nlohmann::json const& json);

    auto& GetChildren() { return m_children; }
    auto const& GetChildren() const { return m_children; }

    std::unique_ptr<Node> Instantiate() override;

    static constexpr char const* LABEL = "layout_data";

protected:
    std::string m_layoutPath;
    std::vector<std::shared_ptr<LayoutEntity>> m_children;
};

class LayoutEntityImage : public LayoutEntity {
public:
    explicit LayoutEntityImage(nlohmann::json const& json);

    std::string const& GetTexturePath() const { return m_texturePath; }
    IntRect const& GetSourceRect() const { return m_sourceRect; }

    std::unique_ptr<Node> Instantiate() override;

    static constexpr char const* LABEL = "image_data";

protected:
    std::string m_texturePath;
    IntRect m_sourceRect;
};

std::unique_ptr<LayoutEntityGroup> LoadLayout(std::string const& layoutPath);
