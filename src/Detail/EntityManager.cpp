#include "../../include/ETCS/Detail/EntityManager.h"

#include "../../include/ETCS/World.h"

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

object_id EntityManager::insert(string_view_t name) {
	auto& archetype = m_world->m_archetypes.m_archetypes.front();

	auto res = m_lookup.emplace(EntityData(uniqueId(), name), archetype).first->first.m_id;
	archetype->m_entities.emplace(res);

	return res;
}
object_id EntityManager::insert(string_view_t name, object_id parentId) {
	if (auto parent = m_lookup.find(parentId); parent != m_lookup.end()) {
		auto& archetype = m_world->m_archetypes.m_archetypes.front();
		auto id = uniqueId();

		if (auto [it, success] = m_lookup.emplace(EntityData(id, name, &parent->first), archetype); success) {
			archetype->m_entities.emplace(it->first.m_id);
			m_lookup.find(parentId)->first.m_children.emplace(EntityView { it->first.m_id, it->first.m_name }); // find parent again because of memory invalidation

			return it->first.m_id;
		} else {
			m_unused.push_back(id);
			return it->first.m_id;
		}
	} else throw std::out_of_range("etcs::detail::EntityManager::insert(): Parent ID does not exist!");

	return nullId;
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
	archetype = m_world->m_archetypes.m_archetypes.front();
}

Archetype*& EntityManager::archetype(object_id entityId) {
	return m_lookup.at(entityId);
}

const Archetype* EntityManager::archetype(object_id entityId) const {
	return m_lookup.at(entityId);
}

} // namespace detail

} // namespace etcs
