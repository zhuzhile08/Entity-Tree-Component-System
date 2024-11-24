#include "../include/ETCS/EntityQuery.h"

#include "../include/ETCS/World.h"
#include "../include/ETCS/Entity.h"

#include <limits>

namespace etcs {

namespace detail {

// BasicQueryIterator

BasicQueryIterator::BasicQueryIterator(BasicEntityQuery* query, archetype_it iterator, archetype_const_it end, entity_it entityIterator) : 
	m_iterator(iterator), m_end(end), m_entityIterator(entityIterator), m_query(query) {
	skipInvalid();
}

Entity BasicQueryIterator::entity() {
	return Entity(*m_entityIterator, m_entityIndex, m_query->world());
}

BasicQueryIterator& BasicQueryIterator::operator++() {
	incrementIterator();
	skipInvalid();
	
	return *this;
}

void BasicQueryIterator::incrementIterator() {
	if (m_iterator != m_end && ++m_entityIterator >= (*m_iterator)->m_entities.end() && ++m_iterator != m_end)
		m_entityIterator = (*m_iterator)->m_entities.begin();
}

void BasicQueryIterator::skipInvalid() {
	while (
		m_iterator != m_end && // if the archetype iterator is at the end, don't increment
		(
			(*m_iterator)->m_entities.empty() || // if the archetype is empty, don't increment
			!m_query->world()->m_entities.data((*m_entityIterator), m_entityIndex).m_active // check if the entity is active
		)
	) incrementIterator();
}


// BasicEntityQuery

BasicEntityQuery::BasicEntityQuery(WorldData* world, const vector_t<lsd::type_id>& typeIds) : m_world(world) {
	world->m_archetypes.querySupersets(m_archetypes, typeIds);
}

BasicQueryIterator BasicEntityQuery::begin() {
	if (m_archetypes.empty()) return BasicQueryIterator();
	else return BasicQueryIterator(this, m_archetypes.begin(), m_archetypes.end(), (*m_archetypes.begin())->m_entities.begin());
}

} // namespace detail

} // namespace etcs
