#pragma once

#include "layer.h"
#include "moth_ui/ui_fwd.h"

class UILayer : public Layer {
public:
    UILayer(std::filesystem::path const& layoutPath);
    virtual ~UILayer() = default;

    bool OnEvent(moth_ui::Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

    void OnAddedToStack(LayerStack* stack);
    void OnRemovedFromStack();

    void LoadLayout(std::filesystem::path const& path);
    std::shared_ptr<moth_ui::Group> GetUIRoot() { return m_root; }

protected:
    std::shared_ptr<moth_ui::Group> m_root;
};

