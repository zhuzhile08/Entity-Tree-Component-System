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

// Core system data
class SystemData {
public:
	constexpr SystemData(object_id id) : m_id(id) { }

private:
	template <class... Types> [[nodiscard]] static std::size_t generateHash() {
		std::set<lsd::type_id> container({ lsd::typeId<std::remove_cv_t<std::remove_reference_t<Types>>>()... });

		auto hash = container.size();

		for (const auto& component : container) {
			hash ^= lsd::implicitCast<std::size_t>(reinterpret_cast<std::uintptr_t>(component) + 0x9e3779b9 + (hash << 6) + (hash >> 2));
		}

		return hash;
	}
	
	std::size_t m_id;

	friend class SystemManager;
	template <class...> friend class ::etcs::System;
};

// System manager
class SystemManager {
private:
	CUSTOM_HASHER(Hasher, const SystemData&, std::size_t, static_cast<std::size_t>, .m_id)
	CUSTOM_EQUAL(Equal, const SystemData&, std::size_t, .m_id)

	constexpr SystemManager(World* world) noexcept : m_world(world) { }

	object_id uniqueId();

	template <class... Types> object_id insert() {
		return m_lookup.emplace(uniqueId(), SystemData::generateHash<Types...>()).first->first.m_id;
	}
	void erase(object_id id);

	std::size_t hash(object_id entityId) const;


	lsd::UnorderedSparseMap<SystemData, std::size_t, Hasher, Equal> m_lookup;
	vector_t<object_id> m_unused;

	World* m_world;

	friend class ArchetypeManager;
	friend class ::etcs::World;
};

} // namespace detail

} // namespace etcs
