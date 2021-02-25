#include <Cool/OpenGL/ComputeShader.h>
#include <Cool/OpenGL/SSBO.h>

using namespace Cool;

class ParticleSystem {
public:
	ParticleSystem(int nbParticles);
	~ParticleSystem();

	void render();
	void update();

	void setNbParticles(int N);

private:
	int m_nbParticles;

	SSBO<float> m_posSSBO;
	SSBO<float> m_velSSBO;
	SSBO<float> m_colorSSBO;

	ComputeShader<256> m_physicsShader;
	ComputeShader<256> m_colorGradientComputeShader;
	ComputeShader<256> m_hueGradientComputeShader;

	GLuint m_vaoID;
	GLuint m_vboID;
};