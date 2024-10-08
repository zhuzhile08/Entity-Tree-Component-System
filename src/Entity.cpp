#include "../include/ETCS/Entity.h"

#include "../include/ETCS/ETCS.h"
 
namespace etcs {

void Entity::destroy() noexcept {
	m_world->m_entities.erase(m_id);
	m_id = nullId;
}


Entity::iterator Entity::begin() {
	return m_world->m_entities.data(m_id, m_index).m_children.begin();
}
Entity::const_iterator Entity::begin() const {
	return m_world->m_entities.data(m_id, m_index).m_children.cbegin();
}
Entity::const_iterator Entity::cbegin() const {
	return m_world->m_entities.data(m_id, m_index).m_children.cbegin();
}

Entity::iterator Entity::end() {
	return m_world->m_entities.data(m_id, m_index).m_children.end();
}
Entity::const_iterator Entity::end() const {
	return m_world->m_entities.data(m_id, m_index).m_children.cend();
}
Entity::const_iterator Entity::cend() const {
	return m_world->m_entities.data(m_id, m_index).m_children.cend();
}

Entity Entity::insertChild(const Entity& child) const {
	auto& data = m_world->m_entities.data(m_id, m_index);
	auto& childData = m_world->m_entities.data(child.m_id, child.m_index);
	
	childData.m_parent = { data.m_id, data.m_name };
	data.m_children.emplace(detail::EntityView { childData.m_id, childData.m_name });

	return child;
}
Entity Entity::insertChild(string_view_t name) const {
	return m_world->m_entities.insert(name, m_id);
}

Entity& Entity::erase(const_iterator pos) { 
	m_world->m_entities.data(m_id, m_index).m_children.erase(pos); 
	return *this;
}
Entity& Entity::erase(const_iterator first, const_iterator last) { 
	m_world->m_entities.data(m_id, m_index).m_children.erase(first, last); 
	return *this;
}
Entity& Entity::erase(string_view_t name) { 
	m_world->m_entities.data(m_id, m_index).m_children.erase(name); 
	return *this;
}

Entity& Entity::clearComponents() {
	m_world->m_entities.clear(m_id);
	return *this;
}
const Entity& Entity::clearComponents() const {
	m_world->m_entities.clear(m_id);
	return *this;
}

Entity& Entity::clearChildren() { 
	m_world->m_entities.data(m_id, m_index).m_children.clear();
	return *this;
}

Entity& Entity::rename(string_view_t name) {
	std::size_t id = 0;

	auto& data = m_world->m_entities.data(m_id, m_index);
	auto& parentData = m_world->m_entities.data(data.m_parent.id, id);

	parentData.m_children.erase(data.m_name);
	data.m_name = name;
	parentData.m_children.emplace(detail::EntityView { data.m_id, data.m_name });

	return *this;
}

Entity& Entity::enable() {
	m_world->m_entities.data(m_id, m_index).m_active = true;
	return *this;
}
Entity& Entity::disable() {
	m_world->m_entities.data(m_id, m_index).m_active = false;
	return *this;
}

Entity::iterator Entity::find(string_view_t name) { 
	return m_world->m_entities.data(m_id, m_index).m_children.find(name); 
}
Entity::const_iterator Entity::find(string_view_t name) const { 
	return m_world->m_entities.data(m_id, m_index).m_children.find(name); 
}

bool Entity::contains(string_view_t name) const { 
	return m_world->m_entities.data(m_id, m_index).m_children.contains(name); 
}
bool Entity::hasParent() const {
	return m_world->m_entities.contains(m_world->m_entities.data(m_id, m_index).m_parent.id);
}

Entity Entity::at(string_view_t name) const {
	std::size_t beg = 0, cur = 0, id = 0;
	auto p = m_id;

	while ((cur = name.find("::", beg)) < name.size()) {
		p = m_world->m_entities.data(p, id).m_children.at(name.substr(beg, cur - beg)).id;
		beg = cur + 2;
	}

	return Entity(m_world->m_entities.data(p, id).m_children.at(name.substr(beg)).id, 0, m_world);
}
Entity Entity::operator[](string_view_t name) const {
	std::size_t beg = 0, cur = 0, id = 0;
	auto p = m_id;

	while ((cur = name.find("::", beg)) < name.size()) {
		p = m_world->m_entities.data(p, id).m_children.at(name.substr(beg, cur - beg)).id;
		beg = cur + 2;
	}

	return Entity(m_world->m_entities.data(p, id).m_children.at(name.substr(beg)).id, 0, m_world);
}

bool Entity::alive() const {
	return m_world->m_entities.contains(m_id);
}
bool Entity::active() const {
	return m_world->m_entities.data(m_id, m_index).m_active;
}

bool Entity::hasComponents() const { 
	return m_world->m_entities.data(m_id, m_index).m_children.empty(); 
}
bool Entity::hasChildren() const { 
	return m_world->m_entities.data(m_id, m_index).m_children.empty(); 
}

std::size_t Entity::size() const {
	return m_world->m_entities.data(m_id, m_index).m_children.size();
}
string_view_t Entity::name() const {
	return m_world->m_entities.data(m_id, m_index).m_name;
}
Entity Entity::parent() const {
	return Entity(m_world->m_entities.data(m_id, m_index).m_parent.id, 0, m_world);
}

World Entity::world() {
	return World(m_world);
}

} // namespace etcs
