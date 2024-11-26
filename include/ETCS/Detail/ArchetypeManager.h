/*************************
 * @file ArchetypeManager.h
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

			virtual void* emplaceBack(void*) = 0;
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

			void* emplaceBack(void* data) override {
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

		template <class Ty> void* emplaceBack(Ty&& component) {
			Ty cmp = std::move(component);
			return m_memory->emplaceBack(&cmp);
		}
		void* emplaceBackData(void* component) {
			return m_memory->emplaceBack(component);
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

	using component_alloc = ComponentAllocator;
	using components = lsd::UnorderedSparseMap<lsd::type_id, component_alloc>;

	using entities = lsd::UnorderedSparseSet<object_id>;
	
public:
	CUSTOM_HASHER(Hasher, const unique_ptr_t<Archetype>&, std::size_t, static_cast<std::size_t>, ->m_hash)
	CUSTOM_EQUAL(Equal, const unique_ptr_t<Archetype>&, std::size_t, ->m_hash)
	CUSTOM_HASHER(PtrHasher, const Archetype* const, std::size_t, static_cast<std::size_t>, ->m_hash)
	CUSTOM_EQUAL(PtrEqual, const Archetype* const, std::size_t, ->m_hash)


	constexpr Archetype() = default;
	constexpr Archetype(Archetype&&) = default;


	std::size_t superHash(lsd::type_id typeId);
	std::size_t subHash(lsd::type_id typeId);

	template <class Ty> Archetype createSuper(std::size_t hash) {
		Archetype a;
		a.m_hash = hash;

		{ // insert component in the proper ordered position
			auto compTypeId = lsd::typeId<Ty>();
			auto inserted = false;
			for (const auto& component : m_components) {
				if (component.first > compTypeId) {
					inserted = true;
					a.m_components.emplace(compTypeId, ComponentAllocator::create<Ty>());
				}

				a.m_components.emplace(component.first, component.second);
			}

			if (!inserted) a.m_components.emplace(compTypeId, ComponentAllocator::create<Ty>());
		}

		return a;
	}
	template <class Ty> Archetype createSub(std::size_t hash) {
		assert(!m_components.empty() && "etcs::Archetype::createSub(): Cannot create subset archetype of empty archetype!");

		Archetype a;
		a.m_hash = hash;

		{ // insert component in the proper ordered position
			auto compTypeId = lsd::typeId<Ty>();
			for (const auto& component : m_components)
				if (component.first != compTypeId) // insert component if it is not of type Ty
					a.m_components.emplace(component.first, component.second);
		}

		return a;
	}


	template <class Ty, class... Args> void insertEntityFromSub(object_id entityId, Archetype& subset, Args&&... args) {
		m_entities.emplace(entityId);

		m_components.at(lsd::typeId<Ty>()).emplaceBack(Ty(std::forward<Args>(args)...));

		auto entityIndex = (subset.m_entities.find(entityId) - subset.m_entities.begin());

		for (auto& component : subset.m_components) {
			m_components.at(component.first).emplaceBackData(
				component.second.componentData(entityIndex)
			);
		}

		subset.eraseEntity(entityId);
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

	void insertEntity(object_id entityId);
	void eraseEntity(object_id entityId);

	template <class Ty> [[nodiscard]] Ty& component(object_id entityId) {
		return *m_components.at(lsd::typeId<Ty>()).template component<Ty>(m_entities.find(entityId) - m_entities.begin());
	}
	template <class Ty> [[nodiscard]] const Ty& component(object_id entityId) const {
		return *m_components.at(lsd::typeId<Ty>()).template component<Ty>(m_entities.find(entityId) - m_entities.begin());
	}

	template <class Ty> [[nodiscard]] bool contains() const {
		return m_components.contains(lsd::typeId<Ty>());
	}

	[[nodiscard]] vector_t<lsd::type_id> typeIds() const;
	[[nodiscard]] std::size_t hash() const noexcept {
		return m_hash;
	}
	[[nodiscard]] bool empty() const noexcept {
		return m_entities.empty();
	}

private:
	components m_components;
	entities m_entities;

	std::size_t m_hash = 0;

	friend class Hasher;
	friend class Equal;
	friend class detail::BasicEntityQuery;
	friend class detail::BasicQueryIterator;
};


class ArchetypeManager {
public:
	using archetype_handle = unique_ptr_t<Archetype>;
	using archetype_array = lsd::UnorderedSparseSet<archetype_handle, Archetype::Hasher, Archetype::Equal>;
	using archetype_lookup = lsd::UnorderedSparseMap<lsd::type_id, vector_t<Archetype*>>;

	ArchetypeManager() { m_archetypes.emplace(archetype_handle::create()); }

	template <class Ty> [[nodiscard]] Archetype* addOrFindSuperset(Archetype* baseArchetype) {
		auto hash = baseArchetype->superHash(lsd::typeId<Ty>());
		auto archetype = m_archetypes.find(hash);

		if (archetype == m_archetypes.end()) {
			archetype = m_archetypes.emplace(archetype_handle::create(baseArchetype->createSuper<Ty>(hash))).first;

			for (auto types = (*archetype)->typeIds(); auto id : types)
				m_archetypeLookup[id].emplace_back(archetype->get());
		}

		return archetype->get();
	}
	template <class Ty> [[nodiscard]] Archetype* addOrFindSubset(Archetype* baseArchetype) {
		auto hash = baseArchetype->subHash(lsd::typeId<Ty>());
		auto archetype = m_archetypes.find(hash);

		if (archetype == m_archetypes.end()) {
			archetype = m_archetypes.emplace(archetype_handle::create(baseArchetype->createSub<Ty>(hash))).first;

			for (auto types = (*archetype)->typeIds(); auto id : types)
				m_archetypeLookup[id].emplace_back(archetype->get());
		}

		return archetype->get();
	}

	void querySupersets(vector_t<Archetype*>& archetypes, vector_t<lsd::type_id> types);

	[[nodiscard]] Archetype* baseArchetype() {
		return m_archetypes.front().get();
	}

private:
	archetype_array m_archetypes;
	archetype_lookup m_archetypeLookup;

	[[nodiscard]] static std::size_t generateHash(const vector_t<std::uintptr_t> types);
};

} // namespace detail

} // namespace etcs
