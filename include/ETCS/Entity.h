/*************************
 * @file Entity.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity handle
 * 
 * @date 2022-24-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include "Detail/Core.h"

#include "Component.h"
#include "World.h"

#include <LSD/UnorderedSparseSet.h>
 
namespace etcs {

class Entity {
private:
	using iterator = lsd::UnorderedSparseSet<detail::EntityView, detail::EVHasher, detail::EVEqual>::iterator;
	using const_iterator = lsd::UnorderedSparseSet<detail::EntityView, detail::EVHasher, detail::EVEqual>::const_iterator;

public:
	constexpr Entity() = default;
	constexpr Entity(const detail::EntityView& view, Entity& parent) : m_id(view.id), m_world(parent.m_world) { }
	constexpr Entity(const Entity&) = default;
	constexpr Entity(Entity&&) = default;

	constexpr Entity& operator=(const Entity&) = default;
	constexpr Entity& operator=(Entity&&) = default;

	void destroy() noexcept;
	
	constexpr void swap(Entity& other) noexcept { 
		auto t = *this; 
		*this = other;
		other = t;
	}

	iterator begin();
	const_iterator begin() const;
	const_iterator cbegin() const;

	iterator end();
	const_iterator end() const;
	const_iterator cend() const;

	template <class Ty, class... Args> ComponentView<Ty> insertComponent(Args&&... args) const {
		return m_world->insertComponent<Ty>(m_id, m_index, std::forward<Args>(args)...);
	}

	Entity insertChild(const Entity& child) const;
	Entity insertChild(string_view_t name) const;

	template <class Ty> Entity& erase() {
		m_world->eraseComponent<Ty>(m_id, m_index);
		return *this;
	}

	Entity& erase(const_iterator pos);
	Entity& erase(const_iterator first, const_iterator last);
	Entity& erase(string_view_t name);

	Entity& clearComponents();
	const Entity& clearComponents() const;

	Entity& clearChildren();

	Entity& rename(string_view_t name);
	
	Entity& enable();
	Entity& disable();

	iterator find(string_view_t name);
	const_iterator find(string_view_t name) const;

	template <class Ty> bool contains() const {
		return m_world->containsComponent<Ty>(m_id, m_index);
	}

	bool contains(string_view_t name) const;
	bool hasParent() const;

	template <class Ty> [[nodiscard]] ComponentView<Ty> component() const {
		return ComponentView<Ty>(m_id, m_index, &m_world->m_entities);
	}

	[[nodiscard]] Entity at(string_view_t name) const;
	[[nodiscard]] Entity operator[](string_view_t name) const;

	[[nodiscard]] bool alive() const;
	[[nodiscard]] bool active() const;

	[[nodiscard]] bool hasComponents() const;
	[[nodiscard]] bool hasChildren() const;

	[[nodiscard]] std::size_t size() const;
	[[nodiscard]] string_view_t name() const;
	[[nodiscard]] Entity parent() const;
	[[nodiscard]] constexpr object_id id() const noexcept { 
		return m_id;
	}
	[[nodiscard]] constexpr World* world() const noexcept {	
		return m_world;
	}
	[[nodiscard]] constexpr const World* world() noexcept {
		return m_world;
	}

private:
	object_id m_id = nullId;
	mutable std::size_t m_index = -1; // only for caching the current index

	World* m_world = nullptr;

	constexpr Entity(object_id id, std::size_t index, World* world = detail::globalWorld) : m_id(id), m_index(index), m_world(world) { }

	friend class detail::EntityManager;
	friend class detail::Archetype;
};

} // namespace etcs
