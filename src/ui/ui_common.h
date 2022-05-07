#pragma once

#include "../ui_node_factory.h"
#include "moth_ui/layout/layout_entity_group.h"

#define DEFINE_WIDGET(name, widgetClass) static auto widgetClass##name = UINodeFactory::Get().RegisterClass(#name, [](std::shared_ptr<moth_ui::LayoutEntity> entity) { return std::make_unique<widgetClass>(std::static_pointer_cast<moth_ui::LayoutEntityGroup>(entity)); });
