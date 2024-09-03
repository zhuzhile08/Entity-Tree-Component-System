#include "../include/ETCS/World.h"

#include "../include/ETCS/Entity.h"

namespace etcs {

namespace detail {

World* globalWorld = nullptr;

} // namespace detail


Entity World::insertEntity(string_view_t name) {
	return m_entities.insert(name);
}
Entity World::insertEntity(string_view_t name, Entity parent) {
	return m_entities.insert(name, parent.id());
}
void World::eraseEntity(object_id entityId) {
	m_entities.erase(entityId);
}
void World::clearEntity(object_id entityId) {
	m_entities.clear(entityId);
}

bool World::containsEntity(object_id entityId) const {
	return m_entities.contains(entityId);
}

detail::EntityData& World::entityData(object_id entityId, std::size_t& index) {
	return m_entities.data(entityId, index);
}

const detail::EntityData& World::entityData(object_id entityId, std::size_t& index) const {
	return m_entities.data(entityId, index);
}

void World::eraseSystem(object_id systemId) {
	m_systems.erase(systemId);
}

} // namespace etcs
