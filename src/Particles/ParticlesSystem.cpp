#include "ParticlesSystem.h"

#include <Cool/Random/Random.h>
#include <Cool/App/RenderState.h>

ParticleSystem::ParticleSystem(int nbParticles)
    : m_renderingShader("shaders/particle.vert", "shaders/particle.frag"),
      m_posSSBO(0),
      m_velSSBO(1),
      m_colorSSBO(2),
      m_physicsShader("shaders/physics.comp"),
      m_colorGradientComputeShader("shaders/colorGradient.comp"),
      m_hueGradientComputeShader("shaders/hueGradient.comp")
{
    setNbParticles(nbParticles);
    // Vertex array
    GLCall(glGenVertexArrays(1, &m_vaoID));
    GLCall(glBindVertexArray(m_vaoID));
    // Vertex buffer
    GLCall(glGenBuffers(1, &m_vboID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vboID));
    float vertices[] = {
        //   pos        uv
        -1.f, -1.f,  0.f, 0.f,
         1.f, -1.f,  1.f, 0.f,
         1.f,  1.f,  1.f, 1.f,

        -1.f, -1.f,  0.f, 0.f,
         1.f,  1.f,  1.f, 1.f,
        -1.f,  1.f,  0.f, 1.f
    };
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    // Vertex Attribute pos
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0));
    // Vertex Attribute uv
    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float))));
}

ParticleSystem::~ParticleSystem() {
    GLCall(glDeleteBuffers(1, &m_vboID));
    GLCall(glDeleteVertexArrays(1, &m_vaoID));
}

void ParticleSystem::render() {
    m_renderingShader.bind();
    m_renderingShader.setUniform1f("u_invAspectRatio", 1.f/RenderState::Size().aspectRatio());
    m_renderingShader.setUniform1f("u_particle_size", m_particle_size);
    GLCall(glBindVertexArray(m_vaoID));
    GLCall(glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_nbParticles));
}

void ParticleSystem::update() {
    m_physicsShader.compute(m_nbParticles);
}

void ParticleSystem::ImGui() {
    ImGui::SliderFloat("Size", &m_particle_size, 0.f, 0.1f);
}

void ParticleSystem::setNbParticles(int N) {
    // Set
    m_nbParticles = N;
    // Resize SSBOs
    std::vector<float> v(N * 2);
    for (auto& x : v)
        x = Random::getMinus1to1();
    m_posSSBO.uploadData  (v);
    m_velSSBO.uploadData  (m_nbParticles * 2, nullptr);
    m_colorSSBO.uploadData(m_nbParticles * 3, nullptr);
    // Update uniform
    // m_physicsShader.get().bind();
    // m_physicsShader.get().setUniform1i("u_NbOfParticles", m_nbParticles);
    // m_colorGradientComputeShader.get().bind();
    // m_colorGradientComputeShader.get().setUniform1i("u_NbOfParticles", m_nbParticles);
    // m_colorGradientComputeShader.get().unbind();
    // m_hueGradientComputeShader.get().bind();
    // m_hueGradientComputeShader.get().setUniform1i("u_NbOfParticles", m_nbParticles);
    // m_hueGradientComputeShader.get().unbind();
    // m_setAllRestPositionsComputeShader.get().bind();
    // m_setAllRestPositionsComputeShader.get().setUniform1i("u_NbOfParticles", m_nbParticles);
    // m_setAllRestPositionsComputeShader.get().unbind();
    // Colors
    // applyParticleColors(colorSettings);
}

//void ParticleSystem::applyParticleColors(const ColorSettingsValues& colorSettings) {
//    // Hue gradient mode
//    if (colorSettings.bColorModeHueGradient) {
//        m_hueGradientComputeShader.get().bind();
//        m_hueGradientComputeShader.get().setUniform1f("hueStart", colorSettings.particlesHueStart / 360.0f);
//        m_hueGradientComputeShader.get().setUniform1f("hueEnd",   colorSettings.particlesHueEnd   / 360.0f);
//        m_hueGradientComputeShader.get().setUniform1f("saturation", colorSettings.particleSaturation / 100.0f);
//        m_hueGradientComputeShader.get().setUniform1f("value", colorSettings.particleValue / 100.0f);
//        m_hueGradientComputeShader.compute(m_nbParticles);
//    }
//    // Color gradient mode
//    else {
//        m_colorGradientComputeShader.get().bind();
//        m_colorGradientComputeShader.get().setUniform3f("colorStart", colorSettings.particleColorStart);
//        m_colorGradientComputeShader.get().setUniform3f("colorEnd",   colorSettings.particleColorEnd);
//        m_colorGradientComputeShader.compute(m_nbParticles);
//    }
//}