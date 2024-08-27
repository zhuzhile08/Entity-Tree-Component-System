/*************************
 * @file Archetype.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief ECS archetype and component memory allocator
 * 
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <LSD/Utility.h>
#include <LSD/UnorderedSparseMap.h>
#include <LSD/UnorderedSparseSet.h>

#include "Core.h"

#include <tuple>

namespace etcs {

namespace detail {

class Archetype {
private:
	class ComponentAllocator {
	private:
		class BasicMemory {
		public:
			virtual ~BasicMemory() { }

			virtual bool emptyComponent() const noexcept = 0;

			virtual void* emplace_back(void*) = 0;
			virtual void eraseComponent(std::size_t) = 0;

			virtual void* componentData(std::size_t) = 0;
			virtual const void* componentData(std::size_t) const = 0;

			virtual void* begin() = 0;
			virtual const void* begin() const = 0;

			virtual std::size_t count() const noexcept = 0;

			virtual unique_ptr_t<BasicMemory> copyType() const = 0;
		};

		template <class Ty> class Memory : public BasicMemory {
			using value_type = Ty;
			using memory = std::conditional_t<std::is_empty_v<value_type>, Ty, vector_t<Ty>>; // tiny memory optimization

			bool emptyComponent() const noexcept override {
				return std::is_empty_v<value_type>;
			}

			void* emplace_back(void* data) override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return &m_memory.emplace_back(std::move(*static_cast<Ty*>(data)));
			}
			void eraseComponent(std::size_t index) override {
				if constexpr (!std::is_empty_v<value_type>) {
					m_memory[index] = std::move(m_memory.back());
					m_memory.popBack();
				}
			}

			void* componentData(std::size_t index) override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return &m_memory[index];
			}
			const void* componentData(std::size_t index) const override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return &m_memory[index];
			}

			void* begin() override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return m_memory.begin().get();
			}
			const void* begin() const override {
				if constexpr (std::is_empty_v<value_type>) return &m_memory;
				else return m_memory.begin().get();
			}

			std::size_t count() const noexcept override {
				if constexpr (std::is_empty_v<value_type>) return 1;
				else return m_memory.size();
			}

			unique_ptr_t<BasicMemory> copyType() const override {
				return unique_ptr_t<Memory>::create();
			}
		
		private:
			memory m_memory;
		};

		ComponentAllocator() = default; // cursed

	public:
		ComponentAllocator(ComponentAllocator&&) = default;
		ComponentAllocator(const ComponentAllocator& other) : m_memory(other.m_memory->copyType()) { }

		template <class Ty> static ComponentAllocator create() {
			ComponentAllocator a;
			a.m_memory = unique_ptr_t<Memory<Ty>>::create();
			return a;
		}

		bool emptyComponent() {
			return m_memory->emptyComponent();
		}

		template <class Ty> void* emplace_back(Ty&& component) {
			Ty cmp = std::move(component);
			return m_memory->emplace_back(&cmp);
		}
		void* emplaceBackData(void* component) {
			return m_memory->emplace_back(component);
		}
		void eraseComponent(std::size_t index) {
			m_memory->eraseComponent(index);
		}

		template <class Ty> Ty* component(std::size_t index) {
			return static_cast<Ty*>(m_memory->componentData(index));
		}
		template <class Ty> const Ty* component(std::size_t index) const {
			return static_cast<Ty*>(m_memory->componentData(index));
		}
		void* componentData(std::size_t index) {
			return m_memory->componentData(index);
		}
		const void* componentData(std::size_t index) const {
			return m_memory->componentData(index);
		}

		std::size_t count() const noexcept {
			return m_memory->count();
		}

		template <class Ty> Ty* begin() noexcept {
			return static_cast<Ty*>(m_memory->begin());
		}
		template <class Ty> const Ty* begin() const noexcept {
			return static_cast<const Ty*>(m_memory->begin());
		}

	private:
		unique_ptr_t<BasicMemory> m_memory;
	};

	struct Edge { 
	public:
		Archetype* superset { };
		Archetype* subset { };
	};

	CUSTOM_HASHER(Hasher, const unique_ptr_t<Archetype>&, std::size_t, static_cast<std::size_t>, ->m_hash)
	CUSTOM_EQUAL(Equal, const unique_ptr_t<Archetype>&, std::size_t, ->m_hash)

	using component_alloc = ComponentAllocator;
	using components = lsd::UnorderedSparseMap<lsd::type_id, component_alloc>;

	using edges = lsd::UnorderedSparseMap<lsd::type_id, Edge>;
	using entities = lsd::UnorderedSparseSet<object_id>;
	
public:
	constexpr Archetype() = default;
	constexpr Archetype(Archetype&&) = default;

	template <class Ty, class... Args> Ty& insertEntityFromSub(object_id entityId, Archetype& subset, Args&&... args) {
		m_entities.emplace(entityId);

		auto& c = *static_cast<Ty*>(m_components.at(lsd::typeId<Ty>()).emplace_back(Ty(std::forward<Args>(args)...)));

		auto entityIndex = (subset.m_entities.find(entityId) - subset.m_entities.begin());

		for (auto& component : subset.m_components) {
			m_components.at(component.first).emplaceBackData(
				component.second.componentData(entityIndex)
			);
		}

		subset.eraseEntity(entityId);

		return c;
	}
	template <class Ty> void insertEntityFromSuper(object_id entityId, Archetype& superset) {
		m_entities.emplace(entityId);

		auto entityIndex = (superset.m_entities.find(entityId) - superset.m_entities.begin());

		auto id = lsd::typeId<Ty>();

		for (auto& component : superset.m_components)
			if (component.first != id) 
				m_components.at(component.first).emplaceBackData(component.second.componentData(entityIndex));

		superset.eraseEntity(entityId);
	}

	void eraseEntity(object_id entityId);

	template <class Ty> Ty& component(object_id entityId) {
		return *m_components.at(lsd::typeId<Ty>()).template component<Ty>(m_entities.at(entityId));
	}
	template <class Ty> const Ty& component(object_id entityId) const {
		return *m_components.at(lsd::typeId<Ty>()).template component<Ty>(m_entities.at(entityId));
	}

	template <class Ty> bool contains() const {
		return m_components.contains(lsd::typeId<Ty>());
	}

	template <class Ty> const Edge& edge() const {
		return m_edges.at(lsd::typeId<Ty>());
	}
	template <class Ty> Edge& edge() {
		return m_edges[lsd::typeId<Ty>()];
	}

	template <class EntityType, class... Types, class Callable> void each(World* world, Callable&& system) {
		for (auto& edge : m_edges) {
			auto superset = edge.second.superset;
			if (superset) superset->each<EntityType, Types...>(world, system);
		}

		auto iterators = std::make_tuple((m_components.at(lsd::typeId<std::remove_cv_t<std::remove_reference_t<Types>>>()).template begin<Types>())...);

		if constexpr (std::is_invocable_v<Callable, EntityType, Types...>) {
			for (auto id : m_entities) {
				auto e = EntityType(id, world);
				if (e.active()) system(e, (*std::get<Types*>(iterators)++)...);
			}
		} else {
			for (auto id : m_entities)
				if (EntityType(id, world).active()) system((*std::get<Types*>(iterators)++)...);
		}
	}

private:
	components m_components;
	entities m_entities;

	edges m_edges;
	std::size_t m_hash = 0;

	static std::size_t superHash(const Archetype& archetype, lsd::type_id typeId);
	static std::size_t subHash(const Archetype& archetype, lsd::type_id typeId);

	template <class Ty> static Archetype createSuper(Archetype& archetype, std::size_t hash) {
		Archetype a;
		a.m_hash = hash;

		{ // insert component in the proper ordered position
			auto compTypeId = lsd::typeId<Ty>();
			auto inserted = false;
			for (const auto& component : archetype.m_components) {
				if (component.first > compTypeId) {
					inserted = true;
					a.m_components.emplace(compTypeId, ComponentAllocator::create<Ty>());
				}

				a.m_components.emplace(component.first, component.second);
			}

			if (!inserted) a.m_components.emplace(compTypeId, ComponentAllocator::create<Ty>());
		}

		a.edge<Ty>().subset = &archetype;

		return a;
	}
	template <class Ty> static Archetype createSub(Archetype& archetype, std::size_t hash) {
		assert(!archetype.m_components.empty() && "etcs::ArchetypeManager::createSub(): Cannot create subset archetype of empty archetype!");

		Archetype a;
		a.m_hash = hash;

		{ // insert component in the proper ordered position
			auto compTypeId = lsd::typeId<Ty>();
			for (const auto& component : archetype.m_components)
				if (component.first != compTypeId) // insert component if it is not of type Ty
					a.m_components.emplace(component.first, component.second);
		}

		a.edge<Ty>().superset = &archetype;

		return a;
	}

	friend class Hasher;
	friend class Equal;
	friend class EntityManager;
	friend class ArchetypeManager;
};


class ArchetypeManager {
public:
	using archetype = unique_ptr_t<Archetype>;
	using archetypes = lsd::UnorderedSparseSet<archetype, Archetype::Hasher, Archetype::Equal>;

	ArchetypeManager(World* world) : m_world(world) {
		m_archetypes.emplace(archetype::create()); 
	}

	template <class Ty> [[nodiscard]] Archetype* addOrFindSuperset(Archetype* baseArchetype) {
		auto& edge = baseArchetype->template edge<Ty>();
		auto archetype = edge.superset;

		if (!archetype) {
			auto hash = Archetype::superHash(*baseArchetype, lsd::typeId<Ty>());
			auto it = m_archetypes.find(hash);

			if (it != m_archetypes.end())
				archetype = it->get();
			else
				archetype = m_archetypes.emplace(archetype::create(Archetype::createSuper<Ty>(*baseArchetype, hash))).first->get();
			
			edge.superset = archetype;
		}

		return archetype;
	}
	template <class Ty> [[nodiscard]] Archetype* addOrFindSubset(Archetype* baseArchetype) {
		auto& edge = baseArchetype->template edge<Ty>();
		auto archetype = edge.subset;

		if (!archetype) {
			auto hash = Archetype::subHash(*baseArchetype, lsd::typeId<Ty>());
			auto it = m_archetypes.find(hash);

			if (it != m_archetypes.end())
				archetype = it->get();
			else
				archetype = m_archetypes.emplace(archetype::create(Archetype::createSuper<Ty>(*baseArchetype, hash))).first->get();
			
			edge.subset = archetype;
		}

		return archetype;
	}

	[[nodiscard]] Archetype* systemArchetype(object_id systemId);

private:
	archetypes m_archetypes;
	World* m_world;

	friend class EntityManager;
	template <class...> friend class ::etcs::System;
};

} // namespace detail

} // namespace etcs
