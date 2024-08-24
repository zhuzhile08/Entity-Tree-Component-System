#include "../include/ETCS/ECS.h"

#include "../include/ETCS/Detail/Core.h"
#include "../include/ETCS/Entity.h"
#include "../include/ETCS/Component.h"
#include "../include/ETCS/System.h"

namespace etcs {

namespace detail {

World* globalWorld = nullptr;

void update() {
	globalWorld->update();
}

} // namespace detail

void init() {
	if (detail::globalWorld) throw std::logic_error("initECS(): The entity component system is already initialized!");
	else detail::globalWorld = new detail::World();
}

} // namespace etcs
