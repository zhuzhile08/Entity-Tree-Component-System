#include "../../include/ETCS/Detail/SystemManager.h"

#include "../../include/ETCS/System.h"

namespace etcs {

namespace detail {

object_id SystemManager::uniqueId() {
	if (m_unused.empty()) {
		if (m_lookup.size() == nullId) throw std::runtime_error("etcs::detail::SystemManager::uniqueId(): Current system count exceeded max system count!");
		else return m_lookup.size();
	} else {
		auto id = m_unused.back();
		m_unused.popBack();
		return id;
	}
}

void SystemManager::erase(object_id systemId) {
	m_unused.push_back(systemId);
	m_lookup.erase(systemId);
}

std::size_t SystemManager::hash(object_id systemId) const {
	return m_lookup.at(systemId);
}

} // namespace detail

} // namespace etcs
