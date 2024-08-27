#include "../include/ETCS/World.h"

#include "../include/ETCS/Entity.h"

namespace etcs {

namespace detail {

World* globalWorld = nullptr;

} // namespace detail


Entity World::insertEntity(string_view_t name) {
	return Entity(m_entities.insert(name), this);
}
Entity World::insertEntity(string_view_t name, Entity parent) {
	return Entity(m_entities.insert(name, parent.m_id), this);
}
void World::eraseEntity(object_id entityId) {
	m_entities.erase(entityId);
}
void World::clearEntity(object_id entityId) {
	m_entities.clear(entityId);
}

bool World::containsEntity(object_id entityId) {
	return m_entities.m_lookup.find(entityId) != m_entities.m_lookup.end();
}

detail::EntityData* World::entityData(object_id entityId) {
	if (auto it = m_entities.m_lookup.find(entityId); it != m_entities.m_lookup.end()) return &it->first;
	else throw std::out_of_range("etcs::World::entityData(): Entity ID did not exist!");

	return nullptr;
}

void World::eraseSystem(object_id systemId) {
	m_systems.erase(systemId);
}

} // namespace etcs
