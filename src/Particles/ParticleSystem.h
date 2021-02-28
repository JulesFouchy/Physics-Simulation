#include <Cool/OpenGL/ComputeShader.h>
#include <Cool/OpenGL/SSBO.h>
#include "Params/ColorParams.h"
#include "Params/PhysicsParams.h"

using namespace Cool;

class ParticleSystem {
public:
	ParticleSystem();
	~ParticleSystem();

	void render();
	void update();
	void ImGui();
	void reset_pos_and_vel();
	
	void onMouseButtonEvent(int button, int action, int mods);

private:
	void on_nb_particles_change();
	inline ComputeShader<256>& physicsShader() { return _bPingPong ? m_physicsShaderFrom1to2 : m_physicsShaderFrom2to1; }
	void on_color_gradient_change();

private:
	glm::vec2 _last_position;
	int _current_nb_particles = 0;
	PhysicsParams _physics_params;
	ColorParams _color_params;

	Shader m_renderingShader;
	bool _bPingPong = true;
	SSBO<float> m_pos1SSBO;
	SSBO<float> m_pos2SSBO;
	SSBO<float> m_velSSBO;
	SSBO<float> m_colorSSBO;
	SSBO<unsigned int> _held_particle_SSBO;
	ComputeShader<256> m_physicsShaderFrom1to2;
	ComputeShader<256> m_physicsShaderFrom2to1;
	ComputeShader<256> _reset_velocities_shader;
	ComputeShader<256> _check_held_particle_shader;
	ComputeShader<256> _compute_color_gradient_shader;
	ComputeShader<256> m_hueGradientComputeShader;
	GLuint m_vaoID;
	GLuint m_vboID;

private:
	//Serialization
	friend class cereal::access;
	template<class Archive>
	void save(Archive& archive) const
	{
		archive(
			cereal::make_nvp("Physics", _physics_params),
			cereal::make_nvp("Color", _color_params)
		);
	}
	template<class Archive>
	void load(Archive& archive)
	{
		archive(
			cereal::make_nvp("Physics", _physics_params),
			cereal::make_nvp("Color", _color_params)
		);
		on_nb_particles_change();
	}
};