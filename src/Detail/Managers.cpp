#include "../../include/ETCS/Detail/Managers.h"

#include "../../include/ETCS/Entity.h"
#include "../../include/ETCS/System.h"
#include "../../include/ETCS/Detail/Archetype.h"
#include "../../include/ETCS/Detail/World.h"

namespace etcs {

namespace detail {

void EntityManager::insert(pointer entity) {
	auto& archetype = m_world->m_archetypes.front();

	entity->m_id = uniqueId();
	m_lookup.emplace(entity, archetype);
	archetype->m_entities.emplace(entity->m_id);
}

void EntityManager::erase(object_id id) {
	m_unused.push_back(id);
	m_lookup.erase(id);
}

void EntityManager::clear(object_id id) {
	auto& archetype = m_lookup.at(id);
	archetype->eraseEntity(id);
	archetype = m_world->m_archetypes.front();
}

Archetype*& EntityManager::archetype(object_id entityId) {
	return m_lookup.at(entityId);
}

Archetype* const& EntityManager::archetype(object_id entityId) const {
	return m_lookup.at(entityId);
}


void SystemManager::insert(pointer object) {
	m_lookup.emplace(object, object->generateHash());
}

void SystemManager::erase(object_id systemId) {
	m_unused.push_back(systemId);
	m_lookup.erase(systemId);
}

std::size_t SystemManager::hash(object_id systemId) const {
	return m_lookup.at(systemId);
}

void SystemManager::update() {
	for (auto& system : m_lookup) system.first->run();
}

} // namespace detail

} // namespace etcs
