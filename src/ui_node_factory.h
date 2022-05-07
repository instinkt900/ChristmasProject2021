#pragma once

#include "moth_ui/node_factory.h"

class UINodeFactory : public moth_ui::NodeFactory {
public:
    virtual ~UINodeFactory() = default;

    static UINodeFactory& Get() {
        if (s_instance == nullptr) {
            s_instance = std::unique_ptr<UINodeFactory>(new UINodeFactory);
        }
        return *s_instance;
    }

private:
    static std::unique_ptr<UINodeFactory> s_instance;

    UINodeFactory();
};
