/*************************
 * @file WorldData.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Holds data of a world
 * 
 * @date 2024-05-19
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include "Core.h"
#include "ArchetypeManager.h"
#include "EntityManager.h"

#include "../Component.h"

namespace etcs {

namespace detail {

class WorldData {
private:
	using pointer_type = unique_ptr_t<WorldData>;

	CUSTOM_HASHER(Hasher, const pointer_type&, const string_t&, hash_t<string_t>(), ->m_name)
	CUSTOM_EQUAL(Equal, const pointer_type&, const string_t&, ->m_name)

public:
	WorldData(string_view_t name) : m_archetypes(), m_entities(this), m_name(name) { }

private:
	template <class Ty, class... Args> ComponentView<Ty> insertComponent(object_id entityId, std::size_t& index, Args&&... args) {
		auto& base = m_entities.archetype(entityId, index);
		if (base->contains<Ty>()) throw std::out_of_range("etcs::detail::WorldData::insertComponent(): A component was requested to be inserted into an entity which already has that component!");

		auto archetype = m_archetypes.addOrFindSuperset<Ty>(base);
		archetype->template insertEntityFromSub<Ty>(entityId, *base, std::forward<Args>(args)...);

		base = archetype;

		return ComponentView<Ty>(entityId, index, &m_entities);
	}
	template <class Ty> void eraseComponent(object_id entityId, std::size_t& index) {
		auto& base = m_entities.archetype(entityId, index);
		if (!base->contains<Ty>()) throw std::out_of_range("etcs::detail::WorldData::eraseComponent(): A component was requested to be erased from an entity which doesn't have that component!");

		detail::Archetype* archetype = m_archetypes.addOrFindSubset<Ty>(base);
		archetype->insertEntityFromSuper<Ty>(entityId, *base);

		base = archetype;
	}

	template <class Ty> bool containsComponent(object_id entityId, std::size_t& index) const {
		return m_entities.archetype(entityId, index)->contains<Ty>();
	}

	detail::ArchetypeManager m_archetypes;
	detail::EntityManager m_entities;

	string_t m_name;

	friend class Hasher;
	friend class Equal;

	friend class detail::EntityManager;
	friend class detail::WorldManager;
	friend class detail::BasicEntityQuery;
	friend class detail::BasicQueryIterator;
	friend class ::etcs::World;
	friend class ::etcs::Entity;
	friend class ::etcs::EntityRange;
};

} // namespace detail

} // namespace etcs
