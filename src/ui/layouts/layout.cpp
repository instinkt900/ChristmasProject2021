#include "game_pch.h"
#include "layout.h"
#include "layout_entity.h"
#include "layout_entity_image.h"
#include "layout_entity_group.h"
#include "animation_clip.h"

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
            std::vector<std::unique_ptr<AnimationClip>> rawAnimationClips;
            for (auto&& clipJson : json["clips"]) {
                rawAnimationClips.push_back(std::make_unique<AnimationClip>(clipJson));
            }
            std::sort(std::begin(rawAnimationClips), std::end(rawAnimationClips), [](auto& clip1, auto& clip2) {
                return clip1->GetStartFrame() < clip2->GetStartFrame();
            });
            auto& animationClips = layout->GetAnimationClips();
            float startTime = 0;
            for (auto&& clip : rawAnimationClips) {
                clip->SetStartTime(startTime);
                startTime = clip->GetEndTime();
                animationClips.push_back(std::move(clip));
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
