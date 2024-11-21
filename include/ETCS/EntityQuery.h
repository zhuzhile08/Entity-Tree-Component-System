/*************************
 * @file EntityIterator.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Pure archetype iterator
 * 
 * @date 2024-09-12
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include "Detail/Core.h"
#include "Detail/ArchetypeManager.h"
#include "Entity.h"

#include <tuple>

namespace etcs {

namespace detail {

class BasicQueryEndIterator { };

class BasicQueryIterator {
private:
	using archetype_it = vector_t<Archetype*>::iterator;
	using archetype_const_it = vector_t<Archetype*>::const_iterator;
	using entity_it = Archetype::entities::iterator;

public:
	ETCS_DEFAULT_CONSTRUCTORS(BasicQueryIterator, constexpr)

	BasicQueryIterator& operator++();

	Entity entity();
	template <class Ty> Ty& component() {
		return *(*m_iterator)->m_components.at(lsd::typeId<Ty>()).template component<Ty>(m_entityIterator - (*m_iterator)->m_entities.begin());
	}
	template <class Ty> const Ty& component() const {
		return *(*m_iterator)->m_components.at(lsd::typeId<Ty>()).template component<Ty>(m_entityIterator - (*m_iterator)->m_entities.begin());
	}

	friend constexpr bool operator==(const BasicQueryIterator& first, const BasicQueryIterator& second) noexcept {
		return first.m_iterator == second.m_iterator && first.m_entityIterator == second.m_entityIterator;
	}
	friend constexpr bool operator==(const BasicQueryIterator& first, BasicQueryEndIterator) noexcept {
		return first.m_iterator == first.m_end;
	}

private:
	archetype_it m_iterator = { };
	archetype_const_it m_end = { };

	entity_it m_entityIterator = { };

	std::size_t m_entityIndex = { };

	BasicEntityQuery* m_query = { };

	BasicQueryIterator(BasicEntityQuery* query, archetype_it iterator, archetype_const_it end, entity_it entityIterator);

	void incrementIterator();
	void skipInvalid();

	friend class BasicEntityQuery;
};

class BasicEntityQuery {
public:
	ETCS_DEFAULT_CONSTRUCTORS(BasicEntityQuery, constexpr)

	BasicQueryIterator begin();
	BasicQueryEndIterator end() {
		return BasicQueryEndIterator { };
	}

	WorldData* world() noexcept {
		return m_world;
	}

private:
	vector_t<Archetype*> m_archetypes;
	WorldData* m_world = { };

	BasicEntityQuery(WorldData* world, std::size_t typeHash);

	void loopAndAddArchetype(Archetype* archetype);

	template <class... Types> [[nodiscard]] static std::size_t generateHash() {
		auto container = array_t<std::uintptr_t, sizeof...(Types)> { reinterpret_cast<std::uintptr_t>(lsd::typeId<Types>())... };

		std::sort(container.begin(), container.end());

		auto hash = container.size();

		for (const auto& component : container) {
			hash ^= lsd::implicitCast<std::size_t>(component + 0x9e3779b9 + (hash << 6) + (hash >> 2));
		}

		return hash;
	}

	template <class, class...> friend class ::etcs::EntityQuery;
};

} // namespace detail


template <class Type, class... Types> class QueryIterator {
public:
	using value_type = std::conditional_t<
		std::is_same_v<Entity, std::remove_const_t<Type>>, 
		std::tuple<Type, Types&...>, 
		std::tuple<Type&, Types&...>
	>;

	ETCS_DEFAULT_CONSTRUCTORS(QueryIterator, constexpr)

	Entity operator->() {
		return m_iterator.entity();
	}
	Entity entity() {
		return m_iterator.entity();
	}

	value_type operator*() {
		if constexpr (std::is_same_v<Entity, std::remove_const_t<Type>>) 
			return value_type { m_iterator.entity(), m_iterator.component<std::remove_const_t<Types>>()... };
		else return value_type { m_iterator.component<std::remove_const_t<Type>>(), m_iterator.component<std::remove_const_t<Types>>()... };
	}

	QueryIterator& operator++() {
		++m_iterator;
		return *this;
	}
	QueryIterator operator++(int) {
		auto temp = *this;
		++m_iterator;
		return temp;
	}

	friend constexpr bool operator==(const QueryIterator& first, const QueryIterator& second) noexcept {
		return first.m_iterator == second.m_iterator;
	}
	friend constexpr bool operator==(const QueryIterator& first, const detail::BasicQueryEndIterator& second) noexcept {
		return first.m_iterator == second;
	}

private:
	detail::BasicQueryIterator m_iterator;

	QueryIterator(detail::BasicQueryIterator&& iterator) : m_iterator(iterator) { }

	template <class, class...> friend class EntityQuery;
};

template <class Type, class... Types> class EntityQuery {
public:
	using iterator_type = QueryIterator<Type, Types...>;

	ETCS_DEFAULT_CONSTRUCTORS(EntityQuery, constexpr)

	iterator_type begin() {
		return iterator_type(m_entityQuery.begin());
	}
	detail::BasicQueryEndIterator end() {
		return detail::BasicQueryEndIterator();
	}

private:
	detail::BasicEntityQuery m_entityQuery;

	EntityQuery(detail::WorldData* world) requires(!std::is_same_v<Entity, std::remove_const_t<Type>>) : 
		m_entityQuery(world, detail::BasicEntityQuery::generateHash<std::remove_const_t<Type>, std::remove_const_t<Types>...>()) { }
	EntityQuery(detail::WorldData* world) requires(std::is_same_v<Entity, std::remove_const_t<Type>>) : 
		m_entityQuery(world, detail::BasicEntityQuery::generateHash<std::remove_const_t<Types>...>()) { }

	friend class World;
};

} // namespace etcs
