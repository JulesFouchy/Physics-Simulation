#include <Cool/OpenGL/ComputeShader.h>
#include <Cool/OpenGL/SSBO.h>

using namespace Cool;

class ParticleSystem {
public:
	ParticleSystem(int nbParticles);
	~ParticleSystem();

	void render();
	void update();
	void ImGui();
	
	void onMouseButtonEvent(int button, int action, int mods);

private:
	void setNbParticles(int N);
	inline ComputeShader<256>& physicsShader() { return _bPingPong ? m_physicsShaderFrom1to2 : m_physicsShaderFrom2to1; }

private:
	int _nbParticles;
	float _particle_size = 0.02f;
	float _stiffness = 5.f;
	float _air_damping = 10.f;
	float _gravity = 1.f;

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
	ComputeShader<256> m_colorGradientComputeShader;
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
			cereal::make_nvp("Number of Particles", _nbParticles),
			cereal::make_nvp("Particle Size", _particle_size),
			cereal::make_nvp("Stiffness", _stiffness),
			cereal::make_nvp("Air Damping", _air_damping),
			cereal::make_nvp("Gravity", _gravity)
		);
	}
	template<class Archive>
	void load(Archive& archive)
	{
		archive(
			cereal::make_nvp("Number of Particles", _nbParticles),
			cereal::make_nvp("Particle Size", _particle_size),
			cereal::make_nvp("Stiffness", _stiffness),
			cereal::make_nvp("Air Damping", _air_damping),
			cereal::make_nvp("Gravity", _gravity)
		);
		setNbParticles(_nbParticles);
	}
};