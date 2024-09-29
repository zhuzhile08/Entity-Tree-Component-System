#include "../include/ETCS/EntityQuery.h"

#include "../include/ETCS/World.h"
#include "../include/ETCS/Entity.h"

#include <limits>

namespace etcs {

namespace detail {

// BasicQueryIterator

Entity BasicQueryIterator::entity() {
	return Entity(*m_entityIterator, m_entityIndex, m_query->world());
}

BasicQueryIterator& BasicQueryIterator::operator++() {
	incrementIterator(); // first, increment the iterator once

	while (
		m_iterator != m_end && // if the archetype iterator is at the end, break
		!m_query->world()->m_entities.data((*m_entityIterator), m_entityIndex).m_active // check if the entity is active
	) incrementIterator();
	
	return *this;
}

void BasicQueryIterator::incrementIterator() {
	if (m_iterator != m_end && ++m_entityIterator == (*m_iterator)->m_entities.end() && ++m_iterator != m_end)
		m_entityIterator = (*m_iterator)->m_entities.begin();
}


// BasicEntityQuery

BasicEntityQuery::BasicEntityQuery(WorldData* world, std::size_t typeHash) : m_world(world) {
	loopAndAddArchetype(world->m_archetypes.archetype(typeHash));
}

void BasicEntityQuery::loopAndAddArchetype(Archetype* archetype) {
	if (archetype) {
		m_archetypes.push_back(archetype);

		for (const auto& edge : archetype->m_edges) loopAndAddArchetype(edge.second.superset);
	}
}

BasicQueryIterator BasicEntityQuery::begin() {
	if (m_archetypes.empty()) return BasicQueryIterator();
	else return BasicQueryIterator(this, m_archetypes.begin(), m_archetypes.end(), (*m_archetypes.begin())->m_entities.begin());
}

} // namespace detail

} // namespace etcs
