#pragma once

#include <Cool/App/IApp.h>
#include <Cool/App/OpenGLWindow.h>
#include <Cool/Renderer_Fullscreen/Renderer_Fullscreen.h>
#include <Cool/Exporter/Exporter.h>
#include <Cool/Camera/Camera.h>

#include "Particles/ParticleSystem.h"

using namespace Cool;

class App : public Cool::IApp {
public:
	App(OpenGLWindow& mainWindow);
	~App();

	void render();

	void update() override;
	void ImGuiWindows() override;
	void ImGuiMenus() override;

	void onKeyboardEvent(int key, int scancode, int action, int mods) override;
	void onMouseButtonEvent(int button, int action, int mods) override;
	void onScrollEvent(double xOffset, double yOffset) override;
	void onMouseMoveEvent(double xPos, double yPos) override;

private:
	OpenGLWindow& m_mainWindow;
	glm::vec3 m_bgColor = glm::vec3(0.478f, 0.674f, 0.792f);
	Renderer_Fullscreen m_renderer;
	Exporter _exporter;
	ParticleSystem m_particle_system;
	Camera _camera;
#ifndef NDEBUG
	bool m_bShow_Debug = true;
	bool m_bShow_ImGuiDemo = false;
#endif

private:
	//Serialization
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("Background Color", m_bgColor),
			cereal::make_nvp("Particle System", m_particle_system)
		);
	}
};