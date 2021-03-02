#pragma once

class PoulpeBody {
public:
	PoulpeBody() = default;
	~PoulpeBody() = default;

	glm::vec2 position_delta() const;
	glm::vec2 position() const;
	inline float size() const { return _size; }
	void update();

	void onMouseMoveEvent(double xPos, double yPos);
	void onMouseButtonEvent(int button, int action, int mods);

private:
	glm::vec2 _curr_mouse_position = glm::vec2(0.f);
	glm::vec2 _prev_mouse_position = glm::vec2(0.f);
	glm::vec2 _position = glm::vec2(0.f);
	float _size = 0.8f;
	bool _is_held = false;
};