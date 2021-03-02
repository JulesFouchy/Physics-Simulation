#include "PoulpeBody.h"

#include <Cool/App/Input.h>

glm::vec2 PoulpeBody::position_delta() const {
	return _is_held ? _curr_mouse_position - _prev_mouse_position : glm::vec2(0.f);
}

glm::vec2 PoulpeBody::position() const {
	return _position;
}

void PoulpeBody::update() {
	_position += position_delta();
	_prev_mouse_position = _curr_mouse_position;
}

void PoulpeBody::onMouseMoveEvent(double xPos, double yPos) {
	_prev_mouse_position = _curr_mouse_position;
	_curr_mouse_position = Cool::Input::ToNormalizedRatioSpace(xPos, yPos);
}

void PoulpeBody::onMouseButtonEvent(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_is_held = glm::length(_curr_mouse_position - _position) < _size;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		_is_held = false;
	}
}