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
#include "Component.h"

namespace etcs {

class World {
public:
	World() : m_archetypes(this), m_entities(this), m_systems(&this->m_archetypes) { }

	// entity functions

	Entity insertEntity(string_view_t name = "");
	Entity insertEntity(string_view_t name, Entity parent);
	void eraseEntity(object_id entityId);
	void clearEntity(object_id entityId);

	bool containsEntity(object_id entityId) const;

	// component functions

	template <class Ty, class... Args> ComponentView<std::remove_volatile_t<std::remove_reference_t<Ty>>> insertComponent(object_id entityId, std::size_t& index, Args&&... args) {
		auto& base = m_entities.archetype(entityId, index);

		auto archetype = m_archetypes.addOrFindSuperset<Ty>(base);
		archetype->template insertEntityFromSub<std::remove_volatile_t<std::remove_reference_t<Ty>>>(entityId, *std::exchange(base, archetype), std::forward<Args>(args)...);

		return ComponentView<std::remove_volatile_t<std::remove_reference_t<Ty>>>(entityId, index, &m_entities);
	}
	template <class Ty> void eraseComponent(object_id entityId, std::size_t& index) {
		auto& base = m_entities.archetype(entityId, index);
		detail::Archetype* archetype = addOrFindSubset<std::remove_volatile_t<std::remove_reference_t<Ty>>>(base);

		archetype->insertEntityFromSuper<std::remove_volatile_t<std::remove_reference_t<Ty>>>(entityId, *std::exchange(base, archetype));
	}

	template <class Ty> bool containsComponent(object_id entityId, std::size_t& index) const {
		return m_entities.archetype(entityId, index)->contains<Ty>();
	}


	// systems functions

	template <class Callable, class... Types> System<Types...> insertSystem(Callable&& callable) {
		return System<Types...>(m_systems.insert<Types...>(std::forward<Callable>(callable)), &m_archetypes);
	}
	void eraseSystem(object_id systemId);

private:
	detail::ArchetypeManager m_archetypes;
	detail::EntityManager m_entities;
	detail::SystemManager m_systems;


	detail::EntityData& entityData(object_id entityId, std::size_t& index);
	const detail::EntityData& entityData(object_id entityId, std::size_t& index) const;


	friend class detail::EntityManager;
	friend class detail::SystemManager;
	friend class detail::ArchetypeManager;
	friend class Entity;
	template <class...> friend class System;
};

} // namespace etcs
