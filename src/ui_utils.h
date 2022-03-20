#pragma once

#include "moth_ui/ui_fwd.h"

std::unique_ptr<moth_ui::Event> ToUIEvent(Event const& inEvent);
