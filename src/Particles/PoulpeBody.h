#pragma once

class PoulpeBody {
public:
	PoulpeBody() = default;
	~PoulpeBody() = default;

	glm::vec2 position_delta() const;
	void update();

	void onMouseMoveEvent(double xPos, double yPos);

private:
	glm::vec2 _curr_position = glm::vec2(0.f);
	glm::vec2 _prev_position = glm::vec2(0.f);
};