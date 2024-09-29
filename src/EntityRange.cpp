#include "../include/ETCS/EntityRange.h"

#include "../include/ETCS/Detail/EntityManager.h"
#include "../include/ETCS/World.h"
#include "../include/ETCS/Entity.h"

namespace etcs {

// EntityIterator

Entity EntityIterator::operator*() {
	return m_entities->at(m_index);
}

Entity EntityIterator::operator->() {
	return m_entities->at(m_index);
}

Entity EntityIterator::entity() {
	return m_entities->at(m_index);
}

EntityIterator& EntityIterator::operator++() {
	++m_index;
	return *this;
}

EntityIterator EntityIterator::operator++(int) {
	EntityIterator temp = *this;
	++*this;
	return temp;
}

// EntityRange

EntityIterator EntityRange::begin() {
	return EntityIterator(&m_world->m_entities);
}

EntityIterator EntityRange::end() {
	return EntityIterator(&m_world->m_entities, m_world->m_entities.size());
}

} // namespace etcs
