#include "../../include/ETCS/Components/Transform.h"

#include "../../include/ETCS/Entity.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace etcs {

Transform::Transform(
	const glm::vec3& translation,
	const glm::vec3& axis,
	float angle,
	const glm::vec3& scale
) : m_translation(translation), m_orientation(glm::rotate(m_orientation, angle, axis)), m_scale(scale), m_dirty(true) { }


void Transform::translate(const glm::vec3& translation) {
	m_translation += translation;
	m_dirty = true;
}

void Transform::rotate(const glm::vec3& axis, float angle) {
	m_orientation = glm::rotate(m_orientation, angle, axis);
	m_dirty = true;
}

void Transform::rotate(const glm::vec3& euler) {
	m_orientation = m_orientation * glm::quat(euler);
	m_dirty = true;
}

void Transform::setOrientation(const glm::vec3& axis, float angle) {
	m_orientation = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), angle, axis);
	m_dirty = true;
}

void Transform::setRotation(const glm::vec3& euler) {
	m_orientation = glm::quat(euler);
	m_dirty = true;
}

void Transform::normalizeAndRotate(const glm::vec3& axis, float angle) {
	m_orientation = glm::rotate(m_orientation, angle, glm::normalize(axis));
	m_dirty = true;
}

void Transform::scale(const glm::vec3& scale) {
	m_scale *= scale;
	m_dirty = true;
}

void Transform::lookAt(const glm::vec3& target, const glm::vec3& up) {
	m_dirty = true;
	
	glm::vec3 direction = m_translation - target;
	float length = glm::length(direction);
	
	if (length < 0.0001) return;
	
	direction /= length;
	
	if (glm::abs(glm::dot(direction, up)) < 0.1)
		m_orientation = glm::normalize(glm::inverse(glm::quatLookAt(direction, this->up())));
	else
		m_orientation = glm::normalize(glm::inverse(glm::quatLookAt(direction, up)));
}

float Transform::axisRotation(const glm::vec3& axis) const {
	glm::vec3 similarAxisVector(0.0f);

	if (axis.x > axis.y && axis.x > axis.z && axis.x != 1) similarAxisVector.x = 1.0f;
	else if ((axis.y > axis.z && axis.y != 1) || axis.z == 1) similarAxisVector.y = 1.0f;
	else similarAxisVector.z = 1.0f;

	glm::vec3 orthonormal = glm::normalize(glm::cross(axis, similarAxisVector));

	auto transformed = glm::rotate(glm::inverse(m_orientation), orthonormal);
	auto flattened = glm::normalize(transformed - glm::dot(transformed, axis) * axis);

	auto result = glm::acos(glm::dot(orthonormal, flattened));

	if (glm::dot(glm::cross(orthonormal, flattened), axis) < 0) return result;
	else return result * -1;
}

glm::vec3 Transform::forward() const {
	return glm::normalize(glm::rotate(glm::inverse(m_orientation), { 0.0f, 0.0f, 1.0f }));
}

glm::vec3 Transform::left() const {
	return glm::normalize(glm::rotate(glm::inverse(m_orientation), { 1.0f, 0.0f, 0.0f }));
}

glm::vec3 Transform::up() const {
	return glm::normalize(glm::rotate(glm::inverse(m_orientation), { 0.0f, 1.0f, 0.0f }));
}


glm::vec3& Transform::localTranslation() {
	m_dirty = true;
	return m_translation;
}

glm::vec3& Transform::translation() {
	m_dirty = true;
	return m_translation;
}

glm::quat& Transform::localOrientation() {
	m_dirty = true;
	return m_orientation;
}

glm::quat& Transform::orientation() {
	m_dirty = true;
	return m_orientation;
}

glm::vec3 Transform::localRotation() const {
	return glm::eulerAngles(m_orientation);
}

glm::vec3 Transform::rotation() const {
	return glm::eulerAngles(m_orientation);
}

glm::vec3& Transform::localScale() {
	m_dirty = true;
	return m_scale;
}

glm::vec3& Transform::scale() {
	m_dirty = true;
	return m_scale;
}

glm::mat4 Transform::localTransform() {
	if (m_dirty) {
		m_localTransform = glm::scale(glm::translate(glm::toMat4((m_orientation = glm::normalize(m_orientation))), m_translation), m_scale);
		m_dirty = false;
	}
	return m_localTransform;
}

glm::mat4 Transform::transform() {
	if (m_dirty) {
		m_localTransform = glm::scale(glm::translate(glm::toMat4((m_orientation = glm::normalize(m_orientation))), m_translation), m_scale);
		m_dirty = false;
	}
	return m_localTransform;
}

glm::vec3 Transform::globalTranslation(const Entity& entity) const {
	auto local = glm::rotate(m_orientation, m_translation);

	if (entity.hasParent()) {
		auto parent = entity.parent();
		auto parentTransform = parent.component<Transform>().get();

		while (parent.alive()) {
			local = glm::rotate(parentTransform.m_orientation, local);

			if (parent.hasParent()) {
				parent = parent.parent();
				parentTransform = parent.component<Transform>().get();
			}
		}
	}
	
	return local;
}

glm::quat Transform::globalOrientation(const Entity& entity) const {
	auto parent = entity.parent();
	return m_orientation * (parent.alive() ? parent.component<Transform>().get().m_orientation : glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
}

glm::vec3 Transform::globalRotation(const Entity& entity) const {
	return glm::eulerAngles(globalOrientation(entity));
}

glm::vec3 Transform::globalScale(const Entity& entity) const {
	auto parent = entity.parent();
	return m_orientation * (parent.alive() ? parent.component<Transform>().get().globalScale(parent.parent()) : glm::vec3(1.0f));
}

glm::mat4 Transform::globalTransform(const Entity& entity) {
	auto parent = entity.parent();
	return localTransform() * (parent.alive() ? parent.component<Transform>().get().globalTransform(parent.parent()) : glm::mat4(1.0f));
}

} // namespace etcs
