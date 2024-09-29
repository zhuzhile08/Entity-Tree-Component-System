#include "../include/ETCS/World.h"

#include <LSD/UnorderedSparseSet.h>

#include "../include/ETCS/Detail/WorldData.h"

namespace etcs {

namespace detail {

class WorldManager {
public:
	WorldManager() {
		m_worlds.emplace(new WorldData({ }));
	}

	World world(string_view_t name) const {
		return World(m_worlds.at(name).get());
	}

	World insert(string_view_t name) {
		if (m_worlds.contains(name)) return World(m_worlds.at(name).get());
		else return World((*m_worlds.emplace(new WorldData(name)).first).get());
	}

	void erase(string_view_t name) {
		if (name == string_view_t { }) throw std::logic_error("etcs::eraseWorld(): Can't erase a world with an empty name, which is the default world!");
		else m_worlds.erase(name);
	}

	bool contains(string_view_t name) const {
		return m_worlds.contains(name);
	}

private:
	lsd::UnorderedSparseSet<unique_ptr_t<WorldData>, WorldData::Hasher, WorldData::Equal> m_worlds;
};

static WorldManager* globalWorldManager;

} // namespace detail

void init() {
	if (detail::globalWorldManager) throw std::logic_error("etcs::init(): The ETCS library is already initialized!");
	else detail::globalWorldManager = new detail::WorldManager();
}

void quit() {
	if (detail::globalWorldManager) delete detail::globalWorldManager;
}

World world(string_view_t name) {
	return detail::globalWorldManager->world(name);
}

World insertWorld(string_view_t name) {
	return detail::globalWorldManager->insert(name);
}

void eraseWorld(string_view_t name) {
	detail::globalWorldManager->erase(name);
}

void eraseWorld(World world) {
	detail::globalWorldManager->erase(world.name());
}

bool containsWorld(string_view_t name) {
	return detail::globalWorldManager->contains(name);
}

bool containsWorld(World world) {
	return detail::globalWorldManager->contains(world.name());
}

} // namespace etcs
