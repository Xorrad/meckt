#pragma once

#include "app/ui/ImGuiStyle.hpp"
#include "app/ui/Filters.hpp"

#include "app/ui/components/Combos.hpp"
#include "app/ui/components/ProvinceInput.hpp"
#include "app/ui/components/PositionInput.hpp"
#include "app/ui/components/ConfirmationModal.hpp"
#include "app/ui/components/NameInputs.hpp"

namespace Components {
    template <typename T>
    inline T Scaled(T value) {
        return value * static_cast<T>(Configuration::uiScale);
    }
}