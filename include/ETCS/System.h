/*************************
 * @file System.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Systems implementation for the ECS
 * 
 * @date 2024-02-09
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include "Detail/Core.h"
#include "Detail/Archetype.h"

#include <set>
#include <variant>

namespace etcs {

template <class... Types> class System {
public:
	constexpr System() = default;
	constexpr System(const System&) = default;
	constexpr System(System&&) = default;

	System& operator=(const System&) = default;
	System& operator=(System&&) = default;

	void destroy() noexcept {
		m_systems->erase(m_id);

		*this = System();
	}
	
	constexpr void swap(Entity& other) noexcept { 
		auto t = *this; 
		*this = other;
		other = t;
	}

	void each() {
		m_systems->each<Types...>(m_id, m_index);
	}

	[[nodiscard]] decltype(auto) function() const {
		return (m_systems->system<Types...>(m_id, m_index).m_system);
	}

	[[nodiscard]] object_id id() const {
		return m_id;
	}
	[[nodiscard]] bool alive() const {
		return m_systems->contains(m_id);
	}

private:
	object_id m_id = nullId;
	mutable std::size_t m_index = -1;

	detail::SystemManager* m_systems = nullptr;

	constexpr System(object_id id, std::size_t index, detail::SystemManager* systems) : m_id(id), m_index(index), m_systems(systems) { }

	friend class World;
};


} // namespace etcs
