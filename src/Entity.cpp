#include "../include/ETCS/Entity.h"

#include "../include/ETCS/ETCS.h"
 
namespace etcs {

void Entity::destroy() noexcept {
	m_world->eraseEntity(m_id);
	m_id = nullId;
}


Entity::iterator Entity::begin() {
	return m_world->entityData(m_id, m_index).m_children.begin();
}
Entity::const_iterator Entity::begin() const {
	return m_world->cEntityData(m_id, m_index).m_children.cbegin();
}
Entity::const_iterator Entity::cbegin() {
	return m_world->entityData(m_id, m_index).m_children.cbegin();
}

Entity::iterator Entity::end() {
	return m_world->entityData(m_id, m_index).m_children.end();
}
Entity::const_iterator Entity::end() const {
	return m_world->cEntityData(m_id, m_index).m_children.cend();
}
Entity::const_iterator Entity::cend() {
	return m_world->entityData(m_id, m_index).m_children.cend();
}

Entity Entity::insertChild(const Entity& child) {
	auto& data = m_world->entityData(m_id, m_index);
	auto& childData = m_world->cEntityData(child.m_id, child.m_index);
	
	childData.m_parent = { data.m_id, data.m_name };
	data.m_children.emplace(detail::EntityView { childData.m_id, childData.m_name });

	return child;
}
Entity Entity::insertChild(string_view_t name) {
	return insertEntity(name, *this);
}

Entity& Entity::erase(const_iterator pos) { 
	m_world->entityData(m_id, m_index).m_children.erase(pos); 
	return *this;
}
Entity& Entity::erase(const_iterator first, const_iterator last) { 
	m_world->entityData(m_id, m_index).m_children.erase(first, last); 
	return *this;
}
Entity& Entity::erase(string_view_t name) { 
	m_world->entityData(m_id, m_index).m_children.erase(name); 
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
	m_world->entityData(m_id, m_index).m_children.clear();
	return *this;
}

Entity& Entity::rename(string_view_t name) {
	auto& data = m_world->entityData(m_id, m_index);
	auto& parentData = m_world->cEntityData(data.m_parent.id, 0);

	parentData.m_children.erase(data.m_name);
	data.m_name = name;
	parentData.m_children.emplace(detail::EntityView { data.m_id, data.m_name });

	return *this;
}

Entity& Entity::enable() {
	m_world->entityData(m_id, m_index).m_active = true;
	return *this;
}
Entity& Entity::disable() {
	m_world->entityData(m_id, m_index).m_active = false;
	return *this;
}

Entity::iterator Entity::find(string_view_t name) { 
	return m_world->entityData(m_id, m_index).m_children.find(name); 
}
Entity::const_iterator Entity::find(string_view_t name) const { 
	return m_world->cEntityData(m_id, m_index).m_children.find(name); 
}

bool Entity::contains(string_view_t name) { 
	return m_world->entityData(m_id, m_index).m_children.contains(name); 
}
bool Entity::hasParent() const {
	return m_world->containsEntity(m_world->cEntityData(m_id, m_index).m_parent.id);
}

Entity Entity::at(string_view_t name) {
	std::size_t beg = 0, cur = 0;
	auto p = m_id;

	while ((cur = name.find("::", beg)) < name.size()) {
		p = m_world->cEntityData(p, 0).m_children.at(name.substr(beg, cur - beg)).id;
		beg = cur + 2;
	}

	return Entity(m_world->cEntityData(p, 0).m_children.at(name.substr(beg)).id, 0, m_world);
}
Entity Entity::operator[](string_view_t name) {
	std::size_t beg = 0, cur = 0;
	auto p = m_id;

	while ((cur = name.find("::", beg)) < name.size()) {
		p = m_world->cEntityData(p, 0).m_children.at(name.substr(beg, cur - beg)).id;
		beg = cur + 2;
	}

	return Entity(m_world->cEntityData(p, 0).m_children.at(name.substr(beg)).id, 0, m_world);
}

bool Entity::alive() const {
	return m_world->containsEntity(m_id);
}
bool Entity::active() {
	return m_world->entityData(m_id, m_index).m_active;
}

bool Entity::hasComponents() { 
	return m_world->entityData(m_id, m_index).m_children.empty(); 
}
bool Entity::hasChildren() { 
	return m_world->entityData(m_id, m_index).m_children.empty(); 
}

std::size_t Entity::size() {
	return m_world->entityData(m_id, m_index).m_children.size();
}
string_view_t Entity::name() {
	return m_world->entityData(m_id, m_index).m_name;
}
Entity Entity::parent() {
	return Entity(m_world->entityData(m_id, m_index).m_parent.id, 0, m_world);
}

} // namespace etcs
