/*************************
 * @file Component.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Component and component manager implementation
 * 
 * @date 2024-02-11
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include "Detail/Core.h"

#include "Detail/ArchetypeManager.h"
#include "Detail/EntityManager.h"

namespace etcs {

template <class Ty> class ComponentView {
public:
	using value_type = Ty;
	using const_value = const Ty;
	using reference = value_type&;
	using const_reference = const_value&;

	constexpr ComponentView() = default;
	constexpr ComponentView(const ComponentView&) = default;
	constexpr ComponentView(ComponentView&&) = default;

	constexpr ComponentView& operator=(const ComponentView&) = default;
	constexpr ComponentView& operator=(ComponentView&&) = default;

	[[nodiscard]] reference get() {
		return m_entities->archetype(m_id, m_index)->template component<value_type>(m_id);
	}
	[[nodiscard]] const_reference get() const {
		return m_entities->archetype(m_id, m_index)->template component<value_type>(m_id);
	}

	[[nodiscard]] object_id entityId() const {
		return m_id;
	}
	[[nodiscard]] bool valid() const {
		return m_entities && m_entities->contains(m_id);
	}

private:
	object_id m_id { };
	std::size_t m_index { };

	detail::EntityManager* m_entities { };

	constexpr ComponentView(object_id id, std::size_t index, detail::EntityManager* entities) : m_id(id), m_index(index), m_entities(entities) { }

	friend class detail::WorldData;
	friend class Entity;
};

} // namespace etcs
