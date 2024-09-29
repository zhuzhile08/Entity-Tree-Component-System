#include "../../include/ETCS/Detail/ArchetypeManager.h"

#include "../../include/ETCS/World.h"

namespace etcs {

namespace detail {

std::size_t Archetype::superHash(const Archetype& archetype, lsd::type_id typeId) {
	auto hash = archetype.m_components.size() + 1;

	auto inserted = false;

	for (const auto& component : archetype.m_components) {
		if (component.first > typeId) {
			inserted = true;
			hash ^= reinterpret_cast<std::uintptr_t>(typeId) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}

		hash ^= reinterpret_cast<std::uintptr_t>(component.first) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}
	if (!inserted) hash ^= reinterpret_cast<std::uintptr_t>(typeId) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

	return hash;
}

std::size_t Archetype::subHash(const Archetype& archetype, lsd::type_id typeId) {
	auto hash = archetype.m_components.size() - 1;

	for (const auto& component : archetype.m_components)
		if (component.first != typeId)
			hash ^= reinterpret_cast<std::uintptr_t>(component.first) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

	return hash;
}

void Archetype::eraseEntity(object_id entityId) {
	auto it = m_entities.find(entityId);

	if (it != m_entities.end()) {
		auto index = (it - m_entities.begin()); // not possible with std::unordered_map
		m_entities.erase(it);

		for (auto& component : m_components) component.second.eraseComponent(index);
	} else throw std::out_of_range("etcs::EnitityComponentSystem::Archetype::eraseEntity(): Tried to erase entity with nonexistant ID!");
}


[[nodiscard]] Archetype* ArchetypeManager::archetype(std::size_t hash) {
	auto archetype = m_archetypes.find(hash);

	if (archetype != m_archetypes.end()) return archetype->get();
	else return nullptr;
}

} // namespace detail

} // namespace etcs
