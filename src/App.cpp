#include "App.h"

#include <Cool/App/RenderState.h>
#include <Cool/App/Input.h>
#include <Cool/Serialization/JsonFile.h>
#include <Cool/File/File.h>
#include <Cool/Time/Time.h>
#include <Cool/Params/ParamsHistory.h>

App::App(OpenGLWindow& mainWindow)
	: m_mainWindow(mainWindow)
{
	Serialization::FromJSON(*this, (File::RootDir + "/last-session-cache.json").c_str());
	// RenderState::SubscribeToSizeChanges([]() { Log::Info("The size of the rendering area has changed. Look, you can subscribe to this event !"); });
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Please note that the blending is WRONG for the alpha channel (but it doesn't matter in most cases) The correct call would be glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE) a.k.a. newAlpha = srcAlpha + dstAlpha - srcAlpha*dstAlpha
}

App::~App() {
	Serialization::ToJSON(*this, (File::RootDir + "/last-session-cache.json").c_str(), "App");
}

void App::update() {
	render();
	Time::Update();
	m_particle_system.update();
	_camera.update();
	_exporter.update(m_renderer.renderBuffer());
}

void App::render() {
	m_renderer.begin();
	m_particle_system.render(_camera.viewMatrix(), _camera.projMatrix());
	m_renderer.end();
}

void App::ImGuiWindows() {
	// Exporter
	if (_exporter.ImGui_window_export_image_sequence() && _reset_on_export) {
		m_particle_system.reset_pos_and_vel();
	}
	if (!RenderState::IsExporting()) {
		_exporter.ImGui_window_export_image([this]() {render(); }, m_renderer.renderBuffer());
		// Particle System
		ImGui::Begin("Particle System");
		ImGui::Checkbox("Reset on export", &_reset_on_export);
		m_particle_system.ImGui();
		ImGui::End();
		// Debug
#ifndef NDEBUG
		if (m_bShow_Debug) {
			ImGui::Begin("Debug", &m_bShow_Debug);
			ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
			ImGui::SameLine();
			bool capFramerate = m_mainWindow.isVSyncEnabled();
			if (ImGui::Checkbox("Cap framerate", &capFramerate)) {
				if (capFramerate)
					m_mainWindow.enableVSync();
				else
					m_mainWindow.disableVSync();
			}
			ImGui::Text("Rendering Size : %d %d", RenderState::Size().width(), RenderState::Size().height());
			ImGui::Text("Mouse Position in Render Area : %.0f %.0f screen coordinates", Input::MouseInScreenCoordinates().x, Input::MouseInScreenCoordinates().y);
			ImGui::Text("Mouse Position Normalized : %.2f %.2f", Input::MouseInNormalizedRatioSpace().x, Input::MouseInNormalizedRatioSpace().y);
			ImGui::ColorEdit3("Background Color", glm::value_ptr(m_bgColor));
			ImGui::Checkbox("Show Demo Window", &m_bShow_ImGuiDemo);
			ImGui::End();
		}
		if (m_bShow_ImGuiDemo) // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			ImGui::ShowDemoWindow(&m_bShow_ImGuiDemo);
#endif
	}
}

void App::ImGuiMenus() {
	if (ImGui::BeginMenu("Windows")) {
#ifndef NDEBUG
		ImGui::Separator();
		ImGui::Checkbox("Debug", &m_bShow_Debug);
#endif
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Export")) {
		_exporter.ImGui_menu_items();
		ImGui::EndMenu();
	}
}

void App::onKeyboardEvent(int key, int scancode, int action, int mods) {
	if (!RenderState::IsExporting() && !ImGui::GetIO().WantTextInput) {
		if (mods & GLFW_MOD_CONTROL && action == GLFW_RELEASE) {
			if (Input::MatchesChar("z", key))
				ParamsHistory::Get().move_backward();
			if (Input::MatchesChar("y", key))
				ParamsHistory::Get().move_forward();
			if (Input::MatchesChar("e", key))
				_exporter.open_window_export_image_sequence();
			if (Input::MatchesChar("s", key))
				_exporter.open_window_export_image();
		}
	}
}

void App::onMouseButtonEvent(int button, int action, int mods) {
	if (!RenderState::IsExporting() && !ImGui::GetIO().WantCaptureMouse) {
		m_particle_system.onMouseButtonEvent(button, action, mods);
		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			if (action == GLFW_PRESS)
				_camera.onWheelDown(mods);
			else if (action == GLFW_RELEASE)
				_camera.onWheelUp();
		}
	}
}

void App::onScrollEvent(double xOffset, double yOffset) {
	if (!RenderState::IsExporting() && !ImGui::GetIO().WantCaptureMouse) {
		_camera.onWheelScroll(yOffset);
	}
}

void App::onMouseMoveEvent(double xPos, double yPos) {
	if (!RenderState::IsExporting() && !ImGui::GetIO().WantCaptureMouse) {
		m_particle_system.onMouseMoveEvent(xPos, yPos);
	}
}