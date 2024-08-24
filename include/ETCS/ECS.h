/*************************
 * @file ECS.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity Component System API
 * 
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include "Entity.h"
#include "Component.h"
#include "System.h"

namespace etcs {

void init();

namespace detail {

void update();

} // namespace detail

} // namespace etcs
