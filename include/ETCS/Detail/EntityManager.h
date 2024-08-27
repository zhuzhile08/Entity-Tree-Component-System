/*************************
 * @file EntityManager.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity data implementation
 * 
 * @date 2024-8-24
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <LSD/UnorderedSparseSet.h>
#include <LSD/UnorderedSparseMap.h>

#include "Core.h"
 
namespace etcs {

namespace detail {

// Core entity data
class EntityData {
public:
	constexpr EntityData(object_id id, string_view_t name) : m_id(id), m_name(name) { }
	constexpr EntityData(object_id id, string_view_t name, EntityData* parent) : m_id(id), m_parent({ parent->m_id, parent->m_name }), m_name(name) {
		parent->m_children.emplace(EntityView { m_id, m_name });
	}

private:
	object_id m_id;
	EntityView m_parent = { };

	string_t m_name;
	lsd::UnorderedSparseSet<EntityView, EVHasher, EVEqual> m_children { };

	// bool m_active;

	friend class EntityManager;
	friend class ::etcs::Entity;
};


// Entity manager
class EntityManager {
private:
	CUSTOM_HASHER(Hasher, const EntityData&, std::size_t, static_cast<std::size_t>, .m_id)
	CUSTOM_EQUAL(Equal, const EntityData&, std::size_t, .m_id)

	constexpr EntityManager(World* world) noexcept : m_world(world) { }

	object_id uniqueId();

	object_id insert(string_view_t name);
	object_id insert(string_view_t name, object_id parentId);
	void erase(object_id id);

	void clear(object_id id);

	Archetype*& archetype(object_id entityId);
	const Archetype* archetype(object_id entityId) const;


	lsd::UnorderedSparseMap<EntityData, Archetype*, Hasher, Equal> m_lookup;
	vector_t<object_id> m_unused;

	World* m_world;

	friend class ::etcs::World;
};

} // namespace detail

} // namespace etcs
