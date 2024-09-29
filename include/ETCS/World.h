/*************************
 * @file World.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief World wrapper and manager functions to allow for a functional interface
 * 
 * @date 2024-09-24
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <LSD/UnorderedSparseSet.h>

#include "Detail/Core.h"
#include "Detail/WorldData.h"

#include "Entity.h"
#include "EntityQuery.h"
#include "EntityRange.h"
#include "Component.h"

namespace etcs {

void init();
void quit();

World world(string_view_t name = { });
World insertWorld(string_view_t name);

void eraseWorld(string_view_t name);
void eraseWorld(World world);

bool containsWorld(string_view_t name);
bool containsWorld(World world);

class World {
public:
	ETCS_DEFAULT_CONSTRUCTORS(World, constexpr)

	// entity functions

	Entity insertEntity(string_view_t name = "") {
		return m_data->m_entities.insert(name);
	}
	Entity insertEntity(string_view_t name, const Entity& parent) {
		return m_data->m_entities.insert(name, parent.m_id);
	}
	void eraseEntity(const Entity& entity) {
		m_data->m_entities.erase(entity.m_id);
	}
	void clearEntity(const Entity& entity) {
		m_data->m_entities.clear(entity.m_id);
	}

	bool containsEntity(const Entity& entity) {
		return m_data->m_entities.contains(entity.m_id);
	}


	// component functions

	template <class Ty, class... Args> ComponentView<Ty> insertComponent(const Entity& entity, Args&&... args) {
		return m_data->insertComponent<Ty>(entity.m_id, entity.m_index, std::forward<Args>(args)...);
	}
	template <class Ty> void eraseComponent(const Entity& entity) {
		m_data->eraseComponent<Ty>(entity.m_id, entity.m_index);
	}

	template <class Ty> bool containsComponent(const Entity& entity) const {
		return m_data->containsComponent<Ty>(entity.m_id, entity.m_index);
	}


	// iteration

	EntityRange entities() {
		return EntityRange(m_data);
	}
	template <class... Types> EntityQuery<Types...> query() {
		return EntityQuery<Types...>(m_data);
	}

	
	// world functions

	bool alive() {
		return containsWorld(m_data->m_name);
	}
	void destroy() {
		eraseWorld(m_data->m_name);
	}

	[[nodiscard]] string_view_t name() const {
		return m_data->m_name;
	}

private:
	detail::WorldData* m_data;

	World(detail::WorldData* data) : m_data(data) { }

	friend class detail::WorldManager;
	friend class detail::BasicEntityQuery;
	friend class detail::BasicQueryIterator;
	friend class EntityRange;
	friend class Entity;
};

} // namespace etcs
