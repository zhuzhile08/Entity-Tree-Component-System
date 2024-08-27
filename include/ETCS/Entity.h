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

#include "World.h"

#include "LSD/UnorderedSparseSet.h"
 
namespace etcs {

class Entity {
private:
	using iterator = lsd::UnorderedSparseSet<detail::EntityView, detail::EVHasher, detail::EVEqual>::iterator;
	using const_iterator = lsd::UnorderedSparseSet<detail::EntityView, detail::EVHasher, detail::EVEqual>::const_iterator;

public:
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

	template <class Ty, class... Args> Entity& insertComponent(Args&&... args) {
		m_world->addComponent<Ty>(m_id, std::forward<Args>(args)...);
		return *this;
	}

	Entity insertChild(Entity child);
	Entity insertChild(string_view_t name);

	template <class Ty> Entity& erase() {
		m_world->eraseComponent<Ty>();
		return *this;
	}
	template <class Ty> const Entity& erase() const {
		m_world->eraseComponent<Ty>();
		return *this;
	}

	Entity& erase(const_iterator pos);
	Entity& erase(const_iterator first, const_iterator last);
	Entity& erase(string_view_t name);

	Entity& clearComponents();
	const Entity& clearComponents() const;

	Entity& clearChildren();

	Entity& rename(string_view_t name);

	iterator find(string_view_t name);
	const_iterator find(string_view_t name) const;

	template <class Ty> bool contains() const {
		return m_world->containsComponent<Ty>(m_id);
	}

	bool contains(string_view_t name) const;
	bool hasParent() const;

	template <class Ty> [[nodiscard]] Ty& component() {
		return m_world->component<Ty>(m_id);
	}
	template <class Ty> [[nodiscard]] const Ty& component() const {
		return m_world->component<Ty>(m_id);
	}

	[[nodiscard]] Entity at(string_view_t name) const;
	[[nodiscard]] Entity operator[](string_view_t name) const;

	[[nodiscard]] bool alive() const;

	[[nodiscard]] bool hasComponents() const;
	[[nodiscard]] bool hasChildren() const;

	[[nodiscard]] std::size_t size() const;
	[[nodiscard]] string_view_t name() const;
	[[nodiscard]] Entity parent() const;
	[[nodiscard]] constexpr object_id id() const noexcept { 
		return m_id;
	}

private:
	object_id m_id;
	World* m_world;

	constexpr Entity(object_id id, World* world = detail::globalWorld) : m_id(id), m_world(world) { }

	friend class World;
};

} // namespace etcs
