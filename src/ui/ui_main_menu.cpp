#include "game_pch.h"
#include "ui_main_menu.h"
#include "ui_common.h"

DEFINE_WIDGET(main_menu, UIMainMenu)

UIMainMenu::UIMainMenu(std::shared_ptr<moth_ui::LayoutEntityGroup> entity)
    : Group(entity) {
    SetAnimation("in");
}
