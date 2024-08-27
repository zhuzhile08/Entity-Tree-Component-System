/*************************
 * @file ETCS.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity Component System API
 * 
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include "Detail/Core.h"

#include "Entity.h"
#include "Component.h"
#include "System.h"
#include "World.h"
#include "Components/Transform.h"

namespace etcs {

void init();


[[nodiscard]] inline auto insertEntity(string_view_t name = "") {
	return detail::globalWorld->insertEntity(name);
}
[[nodiscard]] inline auto insertEntity(string_view_t name, Entity parent) {
	return detail::globalWorld->insertEntity(name, parent);
}

template <class... Types> [[nodiscard]] inline auto insertSystem() {
	return detail::globalWorld->insertSystem<Types...>();
}

} // namespace etcs
