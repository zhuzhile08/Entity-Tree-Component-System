/*************************
 * @file Entity.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Entity class
 * 
 * @date 2022-24-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <LSD/Node.h>

#include "Detail/Core.h"
#include "Detail/World.h"
 
namespace etcs {

class Entity : public lsd::Node<Entity> {
public:
	Entity(string_view_t name = "Entity", detail::World* world = detail::globalWorld) : 
		lsd::Node<Entity>(name), 
		m_world(world) {
		m_world->insertEntity(this);
	}
	Entity(const Entity&) = delete;
	Entity(Entity&&) = default;
	~Entity() {
		m_world->eraseEntity(m_id);
	}

	Entity& operator=(const Entity&) = delete;
	Entity& operator=(Entity&&) = default;

	template <class Ty, class... Args> Entity& addComponent(Args&&... args) noexcept {
		m_world->addComponent<Ty>(m_id, std::forward<Args>(args)...);
		return *this;
	}
	template <class Ty> Entity& removeComponent() noexcept {
		m_world->removeComponent<Ty>();
		return *this;
	}
	template <class Ty> const Entity& removeComponent() const noexcept {
		m_world->removeComponent<Ty>();
		return *this;
	}
	Entity& removeAll() noexcept {
		m_world->clearEntity(m_id);
		return *this;
	}
	const Entity& removeAll() const noexcept {
		m_world->clearEntity(m_id);
		return *this;
	}

	template <class Ty> [[nodiscard]] Ty& component() noexcept {
		return m_world->component<Ty>(m_id);
	}
	template <class Ty> [[nodiscard]] const Ty& component() const {
		return m_world->component<Ty>(m_id);
	}

	template <class Ty> [[nodiscard]] bool containsComponent() const {
		return m_world->containsComponent<Ty>(m_id);
		return false;
	}

	[[nodiscard]] object_id id() const noexcept {
		return m_id;
	}

private:
	object_id m_id;
	detail::World* m_world;

	friend class detail::EntityManager;
};

} // namespace etcs
