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

#include "Archetype.h"

namespace etcs {

namespace detail {

// Core system data
class BasicSystemData {
public:
	constexpr BasicSystemData(object_id id) : m_id(id) { }
	virtual constexpr ~BasicSystemData() = default;

protected:
	object_id m_id;

	template <class... Types> [[nodiscard]] static consteval std::size_t generateHash() {
		auto container = array_t<std::uintptr_t, sizeof...(Types)> { reinterpret_cast<std::uintptr_t>(lsd::typeId<std::remove_volatile_t<std::remove_reference_t<Types>>>())... };

		std::sort(container.begin(), container.end());

		auto hash = container.size();

		for (const auto& component : container) {
			hash ^= lsd::implicitCast<std::size_t>(component + 0x9e3779b9 + (hash << 6) + (hash >> 2));
		}

		return hash;
	}
	
	friend class SystemManager;
};


// system data implementation
template <class... Types> class SystemData : BasicSystemData {
public:
	template <class Callable> SystemData(object_id id, Callable&& callable) : 
		BasicSystemData(id), m_function(callable) { }
	
private:
	function_t<void, std::remove_volatile_t<std::remove_reference_t<Types>>&...> m_function;

	[[nodiscard]] static consteval std::size_t generateHash() {
		return BasicSystemData::generateHash<std::remove_volatile_t<std::remove_reference_t<Types>>...>();
	}

	friend class SystemManager;
	friend class System;
};

// System manager
class SystemManager {
private:
	using system_handle = unique_ptr_t<BasicSystemData>;

	CUSTOM_HASHER(Hasher, const system_handle&, object_id, static_cast<std::size_t>, ->m_id)
	CUSTOM_EQUAL(Equal, const system_handle&, object_id, ->m_id)

public:
	constexpr SystemManager(ArchetypeManager* archetypes) noexcept : m_archetypes(archetypes) { }

	object_id uniqueId();

	template <class Callable, class... Types> object_id insert(Callable&& callable) {
		return m_lookup.emplace(new SystemData<Types...>(uniqueId(), std::forward<Callable>(callable)), BasicSystemData::generateHash<Types...>()).first->first.m_id;
	}
	void erase(object_id systemId);

	template <class... Types> [[nodiscard]] const SystemData<Types...>& system(object_id systemId, std::size_t& index) const {
		if (auto it = m_lookup.begin() + index; it < m_lookup.end() && it->first->m_id == systemId)
			return *static_cast<const SystemData<Types...>*>(it->first.get());
		else if (auto it = m_lookup.find(systemId); it != m_lookup.end()) {
			index = it - m_lookup.begin();

			return *static_cast<const SystemData<Types...>*>(it->first.get());
		}
		
		throw std::out_of_range("etcs::SystemManager::system(): System ID didn't exist!");
	}
	template <class... Types> [[nodiscard]] SystemData<Types...>& system(object_id systemId, std::size_t& index) {
		if (auto it = m_lookup.begin() + index; it < m_lookup.end() && it->first->m_id == systemId)
			return *static_cast<SystemData<Types...>*>(it->first.get());
		else if (auto it = m_lookup.find(systemId); it != m_lookup.end()) {
			index = it - m_lookup.begin();

			return *static_cast<SystemData<Types...>*>(it->first.get());
		}
		
		throw std::out_of_range("etcs::SystemManager::system(): System ID didn't exist!");
	}
	template <class... Types> void each(object_id systemId, std::size_t& index) {
		if (auto archetype = m_archetypes->systemArchetype(systemId); archetype) 
			archetype->template each<Entity, Types...>(m_archetypes->world(), system<Types...>(systemId, index).m_function);
	}

	[[nodiscard]] std::size_t hash(object_id systemId) const {
		return m_lookup.at(systemId);
	}
	[[nodiscard]] bool contains(object_id systemId) const {
		return m_lookup.contains(systemId);
	}

private:
	lsd::UnorderedSparseMap<system_handle, std::size_t, Hasher, Equal> m_lookup;
	vector_t<object_id> m_unused;

	ArchetypeManager* m_archetypes;
};

} // namespace detail

} // namespace etcs
