#include "ParticleSystem.h"

#include <Cool/Random/Random.h>
#include <Cool/App/RenderState.h>
#include <Cool/Time/Time.h>
#include <Cool/String/String.h>
#include <Cool/App/Input.h>

ParticleSystem::ParticleSystem(int nbParticles)
    : m_renderingShader("shaders/particle.vert", "shaders/particle.frag"),
      m_pos1SSBO(1),
      m_pos2SSBO(2),
      m_velSSBO(3),
      m_colorSSBO(0),
      _held_particle_SSBO(4),
      _reset_velocities_shader("shaders/reset_particle_velocities.comp"),
      _check_held_particle_shader("shaders/check_held_particle.comp"),
      m_colorGradientComputeShader("shaders/colorGradient.comp"),
      m_hueGradientComputeShader("shaders/hueGradient.comp")
{
    setNbParticles(nbParticles);
    // Compile compute shaders
    std::string physicsShaderSrc;
    File::ToString("shaders/physics.comp", &physicsShaderSrc);
    std::string physicsShaderSrc12 = physicsShaderSrc;
    String::ReplaceAll(physicsShaderSrc12, "__bindingIN__",  "1");
    String::ReplaceAll(physicsShaderSrc12, "__bindingOUT__", "2");
    std::string physicsShaderSrc21 = physicsShaderSrc;
    String::ReplaceAll(physicsShaderSrc21, "__bindingIN__",  "2");
    String::ReplaceAll(physicsShaderSrc21, "__bindingOUT__", "1");
    m_physicsShaderFrom1to2.createProgramFromCode(physicsShaderSrc12);
    m_physicsShaderFrom2to1.createProgramFromCode(physicsShaderSrc21);
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
    m_renderingShader.setUniform1f("_invAspectRatio", 1.f/RenderState::Size().aspectRatio());
    m_renderingShader.setUniform1f("_particle_size", _particle_size);
    m_renderingShader.setUniform1i("_bPingPong", _bPingPong);
    GLCall(glBindVertexArray(m_vaoID));
    GLCall(glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _nbParticles));
}

void ParticleSystem::update() {
    physicsShader().get().bind();
    physicsShader().get().setUniform1f("_dt", Time::deltaTime());
    physicsShader().get().setUniform1f("_stiffness", _stiffness);
    physicsShader().get().setUniform1f("_air_damping", _air_damping);
    physicsShader().get().setUniform2f("_mouse_pos", Input::MouseInNormalizedRatioSpace());
    physicsShader().compute(_nbParticles);
    _bPingPong = !_bPingPong;
}

void ParticleSystem::ImGui() {
    int N = _nbParticles;
    if (ImGui::SliderInt("Number of Particles", &N, 1, 1000)) {
        setNbParticles(N);
    }
    ImGui::SliderFloat("Size", &_particle_size, 0.f, 0.1f);
    ImGui::SliderFloat("Stiffness", &_stiffness, 0.f, 500.f);
    ImGui::SliderFloat("Damping", &_air_damping, 0.f, 10.f);
    if (ImGui::Button("Reset")) {
        setNbParticles(_nbParticles);
    }
}

void ParticleSystem::onMouseButtonEvent(int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        _check_held_particle_shader.get().bind();
        _check_held_particle_shader.get().setUniform2f("_mouse_pos", Input::MouseInNormalizedRatioSpace());
        _check_held_particle_shader.get().setUniform1f("_particle_radius", _particle_size);
        _check_held_particle_shader.compute(_nbParticles);
    }
    else if (action == GLFW_RELEASE) {
        unsigned int __idx = -1;
        _held_particle_SSBO.uploadData(1, &__idx);
    }
}

void ParticleSystem::setNbParticles(int N) {
    // Set
    _nbParticles = N;
    // Held particle SSBO
    unsigned int __idx = -1;
    _held_particle_SSBO.uploadData(1, &__idx);
    // Resize SSBOs
    std::vector<float> v(N * 2);
    for (int i = 0; i < N; ++i) {
        v[2 * i] = (static_cast<float>(i) / static_cast<float>(N)) * 2.f - 1.f;
        v[2 * i + 1] = i  == N / 2 ? 1.f : 0.f;
    }
    _bPingPong = true;
    m_pos1SSBO.uploadData(v);
    m_pos2SSBO.uploadData(v);
    m_velSSBO.uploadData  (_nbParticles * 2, nullptr);
    _reset_velocities_shader.compute(_nbParticles);
    m_colorSSBO.uploadData(_nbParticles * 3, nullptr);
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