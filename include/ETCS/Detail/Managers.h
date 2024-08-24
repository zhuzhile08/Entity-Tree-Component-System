/*************************
 * @file Manager.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief ECS System and Entity managing class
 * 
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <LSD/UnorderedSparseMap.h>

#include "Core.h"

namespace etcs {

namespace detail {

template <class Ty, class Mapped> class BasicManager {
protected:
	using value_type = Ty;
	using pointer = value_type*;
	using mapped_type = Mapped;

	CUSTOM_HASHER(Hasher, pointer, std::size_t, static_cast<std::size_t>, ->id())
	CUSTOM_EQUAL(Equal, pointer, std::size_t, ->id())

public:
	BasicManager(World* world) noexcept : m_world(world) { }

	object_id uniqueId() {
		if (m_unused.empty())
			return m_lookup.size();
		else {
			auto id = m_unused.back();
			m_unused.popBack();
			return id;
		}
	}

	virtual void insert(pointer) = 0;
	virtual void erase(object_id) = 0;

protected:
	lsd::UnorderedSparseMap<pointer, mapped_type, Hasher, Equal> m_lookup;
	vector_t<object_id> m_unused;

	World* m_world;
};

class EntityManager : public BasicManager<Entity, Archetype*> {
public:
	using basic_manager = BasicManager<Entity, Archetype*>;
	using pointer = basic_manager::pointer;

	EntityManager(World* world) noexcept : basic_manager(world) { }

	void insert(pointer entity) override;
	void erase(object_id id) override;

	void clear(object_id id);

	Archetype*& archetype(object_id entityId);
	Archetype* const& archetype(object_id entityId) const;
};

class SystemManager : public BasicManager<BasicSystem, std::size_t> {
public:
	using basic_manager = BasicManager<BasicSystem, std::size_t>;
	using pointer = basic_manager::pointer;

	SystemManager(World* world) noexcept : basic_manager(world) { }

	void insert(pointer object) override;
	void erase(object_id id) override;

	std::size_t hash(object_id entityId) const;

	void update();
};

} // namespace detail

} // namespace etcs
