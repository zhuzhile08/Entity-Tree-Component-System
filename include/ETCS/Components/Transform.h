/*************************
 * @file transform.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief a class that handels transformations
 * 
 * @date 2022-10-23
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "../Detail/Core.h"

#include "../Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace etcs {

class Transform : public BasicComponent {
public:
	Transform(
		const glm::vec3& translation = glm::vec3(0.0f), 
		const glm::quat& orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 
		const glm::vec3& scale = glm::vec3(1.0f)
	) : m_translation(translation), m_orientation(orientation), m_scale(scale), m_dirty(true) { }
	Transform(
		const glm::vec3& translation,
		const glm::vec3& axis,
		float angle,
		const glm::vec3& scale
	);
	

	void translate(const glm::vec3& translation);

	void rotate(const glm::vec3& axis, float angle);
	void rotate(const glm::vec3& euler);
	void setOrientation(const glm::vec3& axis, float angle);
	void setRotation(const glm::vec3& euler);
	void normalizeAndRotate(const glm::vec3& axis, float angle);
	
	void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 0.0f, 1.0f));

	void scale(const glm::vec3& scale);


	[[nodiscard]] glm::vec3 forward() const;
	[[nodiscard]] glm::vec3 left() const;
	[[nodiscard]] glm::vec3 up() const;
	[[nodiscard]] static glm::vec3 globalForward() {
		return { 0.0f, 1.0f, 0.0f };
	}
	[[nodiscard]] static glm::vec3 globalLeft() {
		return { 1.0f, 0.0f, 0.0f };
	}
	[[nodiscard]] static glm::vec3 globalUp() {
		return { 0.0f, 0.0f, 1.0f };
	}


	[[nodiscard]] glm::vec3& localTranslation();
	[[nodiscard]] const glm::vec3& localTranslation() const {
		return m_translation;
	}
	[[nodiscard]] glm::vec3& translation();
	[[nodiscard]] const glm::vec3& translation() const {
		return m_translation;
	}
	[[nodiscard]] glm::vec3 globalTranslation() const;

	[[nodiscard]] glm::quat& localOrientation();
	[[nodiscard]] const glm::quat& localOrientation() const {
		return m_orientation;
	}
	[[nodiscard]] glm::quat& orientation();
	[[nodiscard]] const glm::quat& orientation() const {
		return m_orientation;
	}
	[[nodiscard]] glm::quat globalOrientation() const;
	[[nodiscard]] glm::vec3 localRotation() const;
	[[nodiscard]] glm::vec3 rotation() const;
	[[nodiscard]] glm::vec3 globalRotation() const;

	[[nodiscard]] glm::vec3& localScale();
	[[nodiscard]] const glm::vec3& localScale() const {
		return m_scale;
	}
	[[nodiscard]] glm::vec3& scale();
	[[nodiscard]] const glm::vec3& scale() const {
		return m_scale;
	}
	[[nodiscard]] glm::vec3 globalScale() const;

	[[nodiscard]] glm::mat4 localTransform();
	[[nodiscard]] glm::mat4 transform();
	[[nodiscard]] glm::mat4 globalTransform();

private:
	glm::vec3 m_translation;
	glm::quat m_orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 m_scale;

	glm::mat4 m_localTransform = glm::mat4(1.0f);

	bool m_dirty = false;

	friend class Spatial;
};

} // namespace etcs
