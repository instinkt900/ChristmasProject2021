#pragma once

namespace ui {
    class LayoutEntityGroup;

    std::shared_ptr<LayoutEntityGroup> LoadLayout(std::string const& layoutPath);
}
