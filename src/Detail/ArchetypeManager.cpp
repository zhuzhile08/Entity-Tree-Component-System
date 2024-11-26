#include "../../include/ETCS/Detail/ArchetypeManager.h"

#include "../../include/ETCS/World.h"

namespace etcs {

namespace detail {

std::size_t Archetype::superHash(lsd::type_id typeId) {
	auto hash = m_components.size() + 1;

	auto inserted = false;

	for (const auto& [id, _] : m_components) {
		if (id > typeId) {
			inserted = true;
			hash ^= reinterpret_cast<std::uintptr_t>(typeId) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}

		hash ^= reinterpret_cast<std::uintptr_t>(id) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}
	if (!inserted) hash ^= reinterpret_cast<std::uintptr_t>(typeId) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

	return hash;
}

std::size_t Archetype::subHash(lsd::type_id typeId) {
	auto hash = m_components.size() - 1;

	for (const auto& [id, _] : m_components)
		if (id != typeId)
			hash ^= reinterpret_cast<std::uintptr_t>(id) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

	return hash;
}

void Archetype::insertEntity(object_id entityId) {
	m_entities.emplace(entityId);
}

void Archetype::eraseEntity(object_id entityId) {
	auto it = m_entities.find(entityId);

	if (it != m_entities.end()) {
		auto index = (it - m_entities.begin()); // not possible with std::unordered_map
		m_entities.erase(it);

		for (auto& component : m_components) component.second.eraseComponent(index);
	} else throw std::out_of_range("etscs::EnitityComponentSystem::Archetype::eraseEntity(): Tried to erase entity with nonexistant ID!");
}

vector_t<lsd::type_id> Archetype::typeIds() const {
	vector_t<lsd::type_id> res { };
	res.reserve(m_components.size());

	for (const auto& [id, _] : m_components) res.push_back(id);

	return res;
}

void ArchetypeManager::querySupersets(vector_t<Archetype*>& archetypes, vector_t<lsd::type_id> types) {
	auto baseArchetypeArray = m_archetypeLookup.find(types.front());
	if (baseArchetypeArray == m_archetypeLookup.end()) return;	

	vector_t<lsd::UnorderedSparseSet<Archetype*>> archetypeLookup;
	archetypeLookup.reserve(types.size() - 1);

	for (auto typeIt = ++types.begin(); typeIt != types.end(); typeIt++) {
		auto archetypeArray = m_archetypeLookup.find(*typeIt);

		if (archetypeArray == m_archetypeLookup.end()) return;
		
		if (archetypeArray->second.size() == 0) return;

		if (archetypeArray->second.size() < baseArchetypeArray->second.size()) {
			archetypeLookup.emplace_back(baseArchetypeArray->second.begin(), baseArchetypeArray->second.end());

			baseArchetypeArray = archetypeArray;
		} else archetypeLookup.emplace_back(archetypeArray->second.begin(), archetypeArray->second.end());
	}

	archetypes.reserve(baseArchetypeArray->second.size());

	for (auto archetype : baseArchetypeArray->second) {
		if (!archetype->empty()) {
			bool found = true;

			for (const auto& lookup : archetypeLookup) {
				if (!lookup.contains(archetype)) {
					found = false;
					break;
				}
			}

			if (found) archetypes.push_back(archetype);
		}
	}
}

} // namespace detail

} // namespace etcs
