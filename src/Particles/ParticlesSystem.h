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

	void setNbParticles(int N);

private:
	int _nbParticles;
	float _particle_size = 0.02f;
	float _stiffness = 5.f;
	float _air_damping = 10.f;

	Shader m_renderingShader;
	SSBO<float> m_posSSBO;
	SSBO<float> m_velSSBO;
	SSBO<float> m_colorSSBO;
	ComputeShader<256> m_physicsShader;
	ComputeShader<256> m_colorGradientComputeShader;
	ComputeShader<256> m_hueGradientComputeShader;
	GLuint m_vaoID;
	GLuint m_vboID;

private:
	//Serialization
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("Particle Size", _particle_size),
			cereal::make_nvp("Stiffness", _stiffness),
			cereal::make_nvp("Air Damping", _air_damping)
		);
	}
};