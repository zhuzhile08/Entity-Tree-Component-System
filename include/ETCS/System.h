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

#include "Detail/World.h"
#include "Entity.h"

#include <set>
#include <variant>

namespace etcs {

class BasicSystem {
public:
	virtual ~BasicSystem() = default;

	virtual void run() = 0;
	virtual object_id id() const noexcept = 0;

protected:
	virtual std::size_t generateHash() const = 0;

	friend class detail::SystemManager;
};

template <class... Types> class System : public BasicSystem {
public:
	System(detail::World* world = detail::globalWorld) : 
		m_world(world) {
		m_world->insertSystem(this);
	}
	constexpr System(System&&) = default;
	~System() {
		m_world->eraseSystem(m_id);
	}

	void run() override {
		auto archetype = m_world->systemArchetype(m_id);
		if (archetype) archetype->template each<Types...>(m_callable);
	}
	void operator()() {
		auto archetype = m_world->systemArchetype(m_id);
		if (archetype) archetype->template each<Types...>(m_callable);
	}

	template <class Callable> void each(Callable&& callable) {
		m_callable = std::forward<Callable>(callable);
	}

	[[nodiscard]] object_id id() const noexcept override {
		return m_id;
	}

private:
	detail::World* m_world;
	std::size_t m_id;

	function_t<void, Types&...> m_callable;


	[[nodiscard]] std::size_t generateHash() const override {
		std::set<lsd::type_id> container({ lsd::typeId<std::remove_cv_t<Types>>()... });

		auto hash = container.size();

		for (const auto& component : container) {
			hash ^= reinterpret_cast<std::uintptr_t>(component) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}

		return hash;
	}
};

} // namespace etcs
