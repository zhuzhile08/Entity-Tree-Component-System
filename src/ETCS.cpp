#include "../include/ETCS/ETCS.h"

#include "../include/ETCS/World.h"

namespace etcs {

void init() {
	if (detail::globalWorld) throw std::logic_error("initECS(): The entity component system is already initialized!");
	else detail::globalWorld = new World();
}

void quit() {
	if (detail::globalWorld) delete detail::globalWorld;
}

} // namespace etcs
