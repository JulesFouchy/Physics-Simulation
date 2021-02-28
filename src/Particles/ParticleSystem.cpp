#include "ParticleSystem.h"

#include <Cool/Random/Random.h>
#include <Cool/App/RenderState.h>
#include <Cool/Time/Time.h>
#include <Cool/String/String.h>
#include <Cool/App/Input.h>

ParticleSystem::ParticleSystem()
    : m_renderingShader("shaders/particle.vert", "shaders/particle.frag"),
      m_pos1SSBO(1),
      m_pos2SSBO(2),
      m_velSSBO(3),
      m_colorSSBO(0),
      _held_particle_SSBO(4),
      _reset_velocities_shader("shaders/reset_particle_velocities.comp"),
      _check_held_particle_shader("shaders/check_held_particle.comp"),
      _compute_color_gradient_shader("shaders/color_gradient.comp"),
      m_hueGradientComputeShader("shaders/hueGradient.comp"),
      _physics_params([this]() {on_nb_particles_change(); }),
      _color_params([this]() {on_color_gradient_change(); })
{
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
    //
    on_nb_particles_change();
}

void ParticleSystem::on_color_gradient_change() {
    _compute_color_gradient_shader.get().bind();
    _compute_color_gradient_shader.get().setUniform3f("_color_begin", *_color_params->color_gradient_begin);
    _compute_color_gradient_shader.get().setUniform3f("_color_end", *_color_params->color_gradient_end);
    _compute_color_gradient_shader.compute(*_physics_params->nb_particles);
}

ParticleSystem::~ParticleSystem() {
    GLCall(glDeleteBuffers(1, &m_vboID));
    GLCall(glDeleteVertexArrays(1, &m_vaoID));
}

void ParticleSystem::render() {
    glClearColor(_color_params->background->r, _color_params->background->g, _color_params->background->b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_renderingShader.bind();
    m_renderingShader.setUniform1f("_invAspectRatio", 1.f/RenderState::Size().aspectRatio());
    m_renderingShader.setUniform1f("_particle_size", *_physics_params->size);
    m_renderingShader.setUniform1i("_bPingPong", _bPingPong);
    m_renderingShader.setUniform1i("_nb_particles", *_physics_params->nb_particles);
    GLCall(glDrawArraysInstanced(GL_TRIANGLES, 0, 6, *_physics_params->nb_particles));
}

void ParticleSystem::update() {
    physicsShader().get().bind();
    physicsShader().get().setUniform1f("_dt", Time::deltaTime());
    physicsShader().get().setUniform1f("_stiffness", *_physics_params->stiffness);
    physicsShader().get().setUniform1f("_air_damping", *_physics_params->internal_damping);
    physicsShader().get().setUniform1f("_gravity", *_physics_params->gravity);
    physicsShader().get().setUniform2f("_mouse_pos", Input::MouseInNormalizedRatioSpace());
    physicsShader().get().setUniform1f("_time", Time::time());
    //physicsShader().get().setUniform1f("_amplitude", _amplitude);
    //physicsShader().get().setUniform1f("_pulsation", _pulsation);
    glm::vec2 curr_pos = Input::MouseInNormalizedRatioSpace();
    physicsShader().get().setUniform2f("_body_position", curr_pos);
    physicsShader().get().setUniform2f("_body_last_position", Input::AltIsDown() ? _last_position : curr_pos);
    _last_position = curr_pos;
    physicsShader().compute(*_physics_params->nb_particles);
    _bPingPong = !_bPingPong;
}

void ParticleSystem::ImGui() {
    ImGui::PushID(321);
        _physics_params.ImGui();
    ImGui::Separator();
    ImGui::PopID();
    ImGui::PushID(322);
        _color_params.ImGui();
    ImGui::PopID();
}

void ParticleSystem::onMouseButtonEvent(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        unsigned int idx;
        _held_particle_SSBO.downloadData(1, &idx);
        if (idx == -1) {
            _check_held_particle_shader.get().bind();
            _check_held_particle_shader.get().setUniform2f("_mouse_pos", Input::MouseInNormalizedRatioSpace());
            _check_held_particle_shader.get().setUniform1f("_particle_radius", *_physics_params->size);
            _check_held_particle_shader.compute(*_physics_params->nb_particles);
        }
        else {
            unsigned int __idx = -1;
            _held_particle_SSBO.uploadData(1, &__idx);
        }
    }
}

void ParticleSystem::on_nb_particles_change() {
    const auto N = *_physics_params->nb_particles;
    if (_current_nb_particles != N) {
        _current_nb_particles = N;
        // Held particle SSBO
        unsigned int __idx = -1;
        _held_particle_SSBO.uploadData(1, &__idx);
        // Resize SSBOs
        std::vector<float> v(N * 2);
        for (int i = 0; i < N; ++i) {
            v[2 * i] = (static_cast<float>(i) / static_cast<float>(N)) * 2.f - 1.f;
            v[2 * i + 1] = 0.f;
        }
        _bPingPong = true;
        m_pos1SSBO.uploadData(v);
        m_pos2SSBO.uploadData(v);
        m_velSSBO.uploadData(N * 2, nullptr);
        _reset_velocities_shader.compute(N);
        m_colorSSBO.uploadData(N * 3, nullptr);
        // Update colors
        on_color_gradient_change();
    }
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