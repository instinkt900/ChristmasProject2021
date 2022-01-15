#include "game_pch.h"
#include "layout_entity.h"
#include "group.h"
#include "node_image.h"

LayoutEntity::LayoutEntity(nlohmann::json const& json)
    : m_id(json["id"])
    , m_bounds(json["bounds"]) {
}

std::unique_ptr<Node> LayoutEntity::Instantiate() {
    return std::make_unique<Node>(*this);
}

LayoutEntityGroup::LayoutEntityGroup(nlohmann::json const& json)
    : LayoutEntity(json) {
    if (json.contains(LABEL)) {
        auto data = json[LABEL];
        data.at("layout_path").get_to(m_layoutPath);
        auto subLayout = LoadLayout(m_layoutPath);
        for (auto&& child : subLayout->m_children) {
            m_children.push_back(child);
            child->SetParent(this);
        }
    }
}

std::unique_ptr<Node> LayoutEntityGroup::Instantiate() {
    return std::make_unique<Group>(*this);
}

LayoutEntityImage::LayoutEntityImage(nlohmann::json const& json)
    : LayoutEntity(json) {
    auto data = json[LABEL];
    data.at("texture_path").get_to(m_texturePath);
    data.at("source_rect").get_to(m_sourceRect);
}

std::unique_ptr<Node> LayoutEntityImage::Instantiate() {
    return std::make_unique<NodeImage>(*this);
}

std::unique_ptr<LayoutEntity> LoadEntity(nlohmann::json const& json) {
    std::unique_ptr<LayoutEntity> entity;

    if (json.contains(LayoutEntityImage::LABEL)) {
        entity = std::make_unique<LayoutEntityImage>(json);
    } else if (json.contains(LayoutEntityGroup::LABEL)) {
        entity = std::make_unique<LayoutEntityGroup>(json);
    } else {
        assert(false && "unknown entity type");
    }

    return entity;
}

std::unique_ptr<LayoutEntityGroup> LoadLayout(std::string const& layoutPath) {
    std::ifstream ifile(layoutPath);
    if (!ifile.is_open()) {
        return nullptr;
    }

    nlohmann::json json;
    ifile >> json;
    auto layout = std::make_unique<LayoutEntityGroup>(json);
    for (auto&& childJson : json["children"]) {
        auto child = LoadEntity(childJson);
        child->SetParent(layout.get());
        layout->GetChildren().push_back(std::move(child));
    }

    return layout;
}
