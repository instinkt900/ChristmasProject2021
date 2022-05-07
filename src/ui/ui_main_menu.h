#pragma once

#include "moth_ui/group.h"

class UIMainMenu : public moth_ui::Group {
public:
    UIMainMenu(std::shared_ptr<moth_ui::LayoutEntityGroup> entity);
    virtual ~UIMainMenu() = default;
};
