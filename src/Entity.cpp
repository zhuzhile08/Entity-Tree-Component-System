#include "../include/ETCS/Entity.h"

#include "../include/ETCS/ETCS.h"
 
namespace etcs {

void Entity::destroy() noexcept {
	m_world->eraseEntity(m_id);
	m_id = nullId;
}


Entity::iterator Entity::begin() {
	return m_world->entityData(m_id)->m_children.begin();
}
Entity::const_iterator Entity::begin() const {
	return m_world->entityData(m_id)->m_children.cbegin();
}
Entity::const_iterator Entity::cbegin() const {
	return m_world->entityData(m_id)->m_children.cbegin();
}

Entity::iterator Entity::end() {
	return m_world->entityData(m_id)->m_children.end();
}
Entity::const_iterator Entity::end() const {
	return m_world->entityData(m_id)->m_children.cend();
}
Entity::const_iterator Entity::cend() const {
	return m_world->entityData(m_id)->m_children.cend();
}

Entity Entity::insertChild(Entity child) {
	auto data = m_world->entityData(m_id);
	auto childData = m_world->entityData(child.m_id);
	
	childData->m_parent = { data->m_id, data->m_name };
	data->m_children.emplace(detail::EntityView { childData->m_id, childData->m_name });

	return child;
}
Entity Entity::insertChild(string_view_t name) {
	return insertEntity(name, *this);
}

Entity& Entity::erase(const_iterator pos) { 
	m_world->entityData(m_id)->m_children.erase(pos); 
	return *this;
}
Entity& Entity::erase(const_iterator first, const_iterator last) { 
	m_world->entityData(m_id)->m_children.erase(first, last); 
	return *this;
}
Entity& Entity::erase(string_view_t name) { 
	m_world->entityData(m_id)->m_children.erase(name); 
	return *this;
}

Entity& Entity::clearComponents() {
	m_world->clearEntity(m_id);
	return *this;
}
const Entity& Entity::clearComponents() const {
	m_world->clearEntity(m_id);
	return *this;
}

Entity& Entity::clearChildren() { 
	m_world->entityData(m_id)->m_children.clear();
	return *this;
}

Entity& Entity::rename(string_view_t name) {
	auto data = m_world->entityData(m_id);
	auto parentData = m_world->entityData(data->m_parent.id);

	parentData->m_children.erase(data->m_name);
	data->m_name = name;
	parentData->m_children.emplace(detail::EntityView { data->m_id, data->m_name });

	return *this;
}

Entity::iterator Entity::find(string_view_t name) { 
	return m_world->entityData(m_id)->m_children.find(name); 
}
Entity::const_iterator Entity::find(string_view_t name) const { 
	return m_world->entityData(m_id)->m_children.find(name); 
}

bool Entity::contains(string_view_t name) const { 
	return m_world->entityData(m_id)->m_children.contains(name); 
}
bool Entity::hasParent() const {
	return m_world->containsEntity(m_world->entityData(m_id)->m_parent.id);
}

Entity Entity::at(string_view_t name) const {
	std::size_t beg = 0, cur = 0;
	auto p = m_id;

	while ((cur = name.find("::", beg)) < name.size()) {
		p = m_world->entityData(p)->m_children.at(name.substr(beg, cur - beg)).id;
		beg = cur + 2;
	}

	return Entity(m_world->entityData(p)->m_children.at(name.substr(beg)).id, m_world);
}
Entity Entity::operator[](string_view_t name) const {
	std::size_t beg = 0, cur = 0;
	auto p = m_id;

	while ((cur = name.find("::", beg)) < name.size()) {
		p = m_world->entityData(p)->m_children.at(name.substr(beg, cur - beg)).id;
		beg = cur + 2;
	}

	return Entity(m_world->entityData(p)->m_children.at(name.substr(beg)).id, m_world);
}

bool Entity::alive() const {
	return m_world->containsEntity(m_id);
}

bool Entity::hasComponents() const { 
	return m_world->entityData(m_id)->m_children.empty(); 
}
bool Entity::hasChildren() const { 
	return m_world->entityData(m_id)->m_children.empty(); 
}

std::size_t Entity::size() const {
	return m_world->entityData(m_id)->m_children.size();
}
string_view_t Entity::name() const {
	return m_world->entityData(m_id)->m_name;
}
Entity Entity::parent() const {
	return Entity(m_world->entityData(m_id)->m_parent.id, m_world);
}

} // namespace etcs
