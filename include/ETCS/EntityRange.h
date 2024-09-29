/*************************
 * @file EntityIterator.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Pure archetype iterator
 * 
 * @date 2024-09-12
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include "Detail/Core.h"

namespace etcs {

class EntityIterator {
public:
	ETCS_DEFAULT_CONSTRUCTORS(EntityIterator, constexpr)

	Entity operator*();
	Entity operator->();
	Entity entity();

	EntityIterator& operator++();
	EntityIterator operator++(int);

	friend constexpr bool operator==(const EntityIterator& first, const EntityIterator& second) noexcept {
		return first.m_index == second.m_index;
	}

protected:
	std::size_t m_index = { };
	
	detail::EntityManager* m_entities;

	constexpr EntityIterator(detail::EntityManager* entities, std::size_t index = { }) : m_index(index), m_entities(entities) { }

	friend class EntityRange;
};


class EntityRange {
public:
	ETCS_DEFAULT_CONSTRUCTORS(EntityRange, constexpr)

	EntityIterator begin();
	EntityIterator end();

private:
	detail::WorldData* m_world;

	constexpr EntityRange(detail::WorldData* world) : m_world(world) { }

	friend class World;
};

} // namespace etcs
