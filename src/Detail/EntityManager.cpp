#include "../../include/ETCS/Detail/EntityManager.h"

#include "../../include/ETCS/World.h"
#include "../../include/ETCS/Entity.h"

#include <stdexcept>

namespace etcs {

namespace detail {

object_id EntityManager::uniqueId() {
	if (m_unused.empty()) {
		if (m_lookup.size() == nullId) throw std::runtime_error("etcs::detail::EntityManager::uniqueId(): Current entity count exceeded max entity count!");
		else return m_lookup.size();
	} else {
		auto id = m_unused.back();
		m_unused.popBack();
		return id;
	}
}

Entity EntityManager::insert(string_view_t name) {
	auto archetype = m_world->m_archetypes.defaultArchetype();

	auto res = m_lookup.emplace(EntityData(uniqueId(), name), archetype).first;
	archetype->m_entities.emplace(res->first.m_id);

	return Entity(res->first.m_id, res - m_lookup.begin(), m_world);
}
Entity EntityManager::insert(string_view_t name, object_id parentId) {
	if (auto parent = m_lookup.find(parentId); parent != m_lookup.end()) {
		if (auto it = parent->first.m_children.find(name); it == parent->first.m_children.end()) {
			auto archetype = m_world->m_archetypes.defaultArchetype();

			auto eIt = m_lookup.emplace(EntityData(uniqueId(), name, &parent->first), archetype).first;
			archetype->m_entities.emplace(eIt->first.m_id);

			m_lookup.find(parentId)->first.m_children.emplace(EntityView { eIt->first.m_id, eIt->first.m_name }); // find parent again because of memory invalidation

			return Entity(eIt->first.m_id, eIt - m_lookup.begin(), m_world);
		} else return Entity(it->id, std::numeric_limits<std::size_t>::max(), m_world);
	} else throw std::out_of_range("etcs::detail::EntityManager::insert(): Parent ID does not exist!");

	return Entity(nullId, std::numeric_limits<std::size_t>::max(), nullptr);
}

void EntityManager::erase(object_id id) {
	auto e = m_lookup.find(id);

	if (e == m_lookup.end()) throw std::out_of_range("etcs::detail::EntityManager::erase(): Entity ID does not exist!");

	for (auto it = e->first.m_children.begin(); it != e->first.m_children.end(); it++) {
		m_lookup.find(it->id)->first.m_parent = e->first.m_parent;
	}

	m_unused.push_back(id);
	m_lookup.erase(id);
}

void EntityManager::clear(object_id id) {
	auto& archetype = m_lookup.at(id);
	archetype->eraseEntity(id);
	archetype = m_world->m_archetypes.defaultArchetype();
}

detail::EntityData& EntityManager::data(object_id id, std::size_t& index) {
	if (m_lookup.size() > index) if (auto& e = (m_lookup.begin() + index)->first; e.m_id == id) return e;
	
	if (auto it = m_lookup.find(id); it != m_lookup.end()) {
		index = it - m_lookup.begin();
		return it->first;
	} else throw std::out_of_range("etcs::detail::EntityManager::data(): Entity ID did not exist!");

	return m_lookup.begin()->first;
}

const detail::EntityData& EntityManager::data(object_id id, std::size_t& index) const {
	if (m_lookup.size() > index) if (auto& e = (m_lookup.begin() + index)->first; e.m_id == id) return e;
	
	if (auto it = m_lookup.find(id); it != m_lookup.end()) {
		index = it - m_lookup.begin();
		return it->first;
	} else throw std::out_of_range("etcs::detail::EntityManager::data(): Entity ID did not exist!");

	return m_lookup.begin()->first;
}

bool EntityManager::contains(object_id id) const {
	return m_lookup.contains(id);
}

Archetype*& EntityManager::archetype(object_id id, std::size_t& index) {
	if (m_lookup.size() > index) if (auto it = m_lookup.begin() + index; it->first.m_id == id) return it->second;

	if (auto it = m_lookup.find(id); it != m_lookup.end()) {
		index = it - m_lookup.begin();
		return it->second;
	} else throw std::out_of_range("etcs::detail::EntityManager::archetype(): Entity ID did not exist!");

	return m_lookup.begin()->second;
}

const Archetype* EntityManager::archetype(object_id id, std::size_t& index) const {
	if (m_lookup.size() > index) if (auto it = m_lookup.begin() + index; it->first.m_id == id) return it->second;

	if (auto it = m_lookup.find(id); it != m_lookup.end()) {
		index = it - m_lookup.begin();
		return it->second;
	} else throw std::out_of_range("etcs::detail::EntityManager::archetype(): Entity ID did not exist!");

	return m_lookup.begin()->second;
}

Entity EntityManager::find(object_id entityId) const {
	if (auto it = m_lookup.find(entityId); it != m_lookup.end()) return Entity(it->first.m_id, it - m_lookup.begin(), m_world);
	else throw std::out_of_range("etcs::detail::EntityManager::data(): Entity ID did not exist!");

	return Entity(nullId, std::numeric_limits<std::size_t>::max(), nullptr);
}

} // namespace detail

} // namespace etcs
