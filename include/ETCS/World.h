/*************************
 * @file World.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Includes all headers of the world class
 * 
 * @date 2024-05-19
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <LSD/UnorderedSparseSet.h>

#include "Detail/Core.h"
#include "Detail/Archetype.h"
#include "Detail/EntityManager.h"
#include "Detail/SystemManager.h"
#include "System.h"

namespace etcs {

class World {
public:
	World() : m_archetypes(this), m_entities(this), m_systems(this) { }

	// entity functions

	Entity insertEntity(string_view_t name = "");
	Entity insertEntity(string_view_t name, Entity parent);
	void eraseEntity(object_id entityId);
	void clearEntity(object_id entityId);

	bool containsEntity(object_id entityId) const;

	// component functions

	template <class Ty, class... Args> Ty& addComponent(object_id entityId, Args&&... args) {
		auto& base = m_entities.archetype(entityId);
		detail::Archetype* archetype = m_archetypes.addOrFindSuperset<Ty>(base);

		return archetype->insertEntityFromSub<Ty>(entityId, *std::exchange(base, archetype), std::forward<Args>(args)...);
	}
	template <class Ty> void eraseComponent(object_id entityId) {
		auto& base = m_entities.archetype(entityId);
		detail::Archetype* archetype = addOrFindSubset<Ty>(base);

		archetype->insertEntityFromSuper<Ty>(entityId, *std::exchange(base, archetype));
	}

	template <class Ty> bool containsComponent(object_id entityId) const {
		return m_entities.archetype(entityId)->contains<Ty>();
	}

	template <class Ty> Ty& component(object_id entityId) {
		return m_entities.archetype(entityId)->component<Ty>(entityId);
	}
	template <class Ty> const Ty& component(object_id entityId) const {
		return m_entities.archetype(entityId)->component<Ty>(entityId);
	}


	// systems functions

	template <class... Types> System<Types...> insertSystem() {
		return System<Types...>(m_systems.insert<Types...>(), &m_archetypes);
	}
	void eraseSystem(object_id systemId);

private:
	detail::ArchetypeManager m_archetypes;
	detail::EntityManager m_entities;
	detail::SystemManager m_systems;


	detail::EntityData& entityData(object_id entityId, std::size_t index);
	const detail::EntityData& entityData(object_id entityId, std::size_t index) const;


	friend class detail::EntityManager;
	friend class detail::SystemManager;
	friend class detail::ArchetypeManager;
	friend class Entity;
	template <class...> friend class System;
};

} // namespace etcs
