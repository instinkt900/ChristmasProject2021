#include "game_pch.h"
#include "layout.h"
#include "layout_entity.h"
#include "layout_entity_image.h"
#include "layout_entity_group.h"
#include "animation_clip_info.h"

namespace ui {
    std::unique_ptr<LayoutEntity> LoadEntity(nlohmann::json const& json, LayoutEntityGroup* parent) {
        std::unique_ptr<LayoutEntity> entity;

        if (json.contains(LayoutEntityImage::LABEL)) {
            entity = std::make_unique<LayoutEntityImage>(json, parent);
        } else if (json.contains(LayoutEntityGroup::LABEL)) {
            entity = std::make_unique<LayoutEntityGroup>(json, parent);
        } else {
            assert(false && "unknown entity type");
        }

        return entity;
    }

    std::shared_ptr<LayoutEntityGroup> LoadLayout(std::string const& layoutPath) {
        std::ifstream ifile(layoutPath);
        if (!ifile.is_open()) {
            return nullptr;
        }

        nlohmann::json json;
        ifile >> json;
        auto layout = std::make_unique<LayoutEntityGroup>(json, nullptr);

        if (json.contains("clips")) {
            auto& animationClips = layout->GetAnimationClips();
            float startTime = 0;
            for (auto&& clipJson : json["clips"]) {
                auto clip = std::make_shared<AnimationClipInfo>(clipJson, startTime);
                startTime = clip->GetEndTime();
                animationClips.insert(std::make_pair(clip->GetName(), clip));
            }
        }

        if (json.contains("children")) {
            for (auto&& childJson : json["children"]) {
                auto child = LoadEntity(childJson, layout.get());
                layout->GetChildren().push_back(std::move(child));
            }
        }

        return layout;
    }
}
