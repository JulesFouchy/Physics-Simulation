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
	int m_nbParticles;
	float m_particle_size = 0.02f;

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
			cereal::make_nvp("Particle Size", m_particle_size)
		);
	}
};