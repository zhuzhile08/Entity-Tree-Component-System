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
	constexpr System(const System&) = default;
	constexpr System(System&&) = default;

	System& operator=(const System&) = default;
	System& operator=(System&&) = default;

	template <class Callable> void each(Callable&& callable) {
		if (auto archetype = m_archetypes->systemArchetype(m_id); archetype) archetype->template each<Types...>(std::forward<Callable>(callable));
	}

	[[nodiscard]] object_id id() const noexcept {
		return m_id;
	}

private:
	object_id m_id;
	detail::ArchetypeManager* m_archetypes;

	constexpr System(object_id id, detail::ArchetypeManager* archetypes) : m_id(id), m_archetypes(archetypes) { }

	friend class World;
};

} // namespace etcs
