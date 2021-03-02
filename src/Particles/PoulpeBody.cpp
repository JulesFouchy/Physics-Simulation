#include "PoulpeBody.h"

#include <Cool/App/Input.h>

glm::vec2 PoulpeBody::position_delta() const {
	return _curr_position - _prev_position;
}

void PoulpeBody::update() {
	_prev_position = _curr_position;
}

void PoulpeBody::onMouseMoveEvent(double xPos, double yPos) {
	_prev_position = _curr_position;
	_curr_position = Cool::Input::ToNormalizedRatioSpace(xPos, yPos);
}