#include "ParticleSystem.h"

#include <Cool/Random/Random.h>
#include <Cool/App/RenderState.h>
#include <Cool/Time/Time.h>
#include <Cool/String/String.h>
#include <Cool/App/Input.h>

struct Vertex {
    glm::vec4 pos;
    glm::vec2 uv;
    glm::vec2 _padding;
};

ParticleSystem::ParticleSystem()
      : _physics_params([this]() {on_nb_vertices_change(); }),
      _color_params([this]() {})
{
    // Vertex array
    GLCall(glGenVertexArrays(1, &_vaoID));
    GLCall(glBindVertexArray(_vaoID));
    // Vertex buffer
    GLCall(glGenBuffers(1, &_vboID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vboID));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _vboID));
    // Index buffer
    GLCall(glGenBuffers(1, &_iboID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboID));
    // Vertex Attribute pos
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos))));
    // Vertex Attribute uv
    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex,  uv))));
    //
    on_nb_vertices_change();
}

ParticleSystem::~ParticleSystem() {
    GLCall(glDeleteBuffers(1, &_vboID));
    GLCall(glDeleteBuffers(1, &_iboID));
    GLCall(glDeleteVertexArrays(1, &_vaoID));
}

void ParticleSystem::update() {
    _update_physics_cs->bind();
    _update_physics_cs->setUniform("_grid_width", _grid_width);
    _update_physics_cs->setUniform("_grid_height", _grid_height);
    _update_physics_cs->setUniform("_dt", Time::deltaTime());    
    _update_physics_cs->setUniform("_stiffness", *_physics_params->stiffness);
    _update_physics_cs->setUniform("_internal_damping", *_physics_params->internal_damping);
    _update_physics_cs->setUniform("_external_damping", *_physics_params->external_damping);
    _update_physics_cs->setUniform("_gravity", *_physics_params->gravity);

    _update_physics_cs.compute(nb_of_vertices());
}

void ParticleSystem::render(const glm::mat4& view_mat, const glm::mat4& proj_mat) {
    // Background
    glClearColor(_color_params->background->r, _color_params->background->g, _color_params->background->b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Mesh
    _rendering_shader.bind();
    _rendering_shader.setUniform("_view_mat", view_mat);
    _rendering_shader.setUniform("_proj_mat", proj_mat);
    GLCall(glBindVertexArray(_vaoID));
    GLCall(glDrawElements(GL_TRIANGLES, nb_of_indices(), GL_UNSIGNED_INT, 0));
}

void ParticleSystem::on_nb_vertices_change() {
    // Vertex buffer
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * nb_of_vertices(), nullptr, GL_STATIC_DRAW));
    _init_vertices_cs->bind();
    _init_vertices_cs->setUniform("_grid_width", _grid_width);
    _init_vertices_cs->setUniform("_grid_height", _grid_height);
    _init_vertices_cs.compute(nb_of_vertices());
    // Index buffer
    std::vector<unsigned int> indices;
    indices.reserve(nb_of_indices());
    for (int y = 0; y < _grid_height - 1; ++y) {
        for (int x = 0; x < _grid_width - 1; ++x) {
            int idx = x + y * _grid_width;

            indices.push_back(idx);
            indices.push_back(idx + _grid_width + 1);
            indices.push_back(idx + _grid_width);

            indices.push_back(idx);
            indices.push_back(idx + 1);
            indices.push_back(idx + _grid_width + 1);
        }
    }
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW));
    // Velocities SSBO
    _vel_ssbo.uploadData(4 * nb_of_vertices(), nullptr);
    //
    reset_pos_and_vel();
}

void ParticleSystem::reset_pos_and_vel() {
    _reset_pos_and_vel_cs->bind();
    _reset_pos_and_vel_cs->setUniform("_grid_width", _grid_width);
    _reset_pos_and_vel_cs->setUniform("_grid_height", _grid_height);
    _reset_pos_and_vel_cs.compute(nb_of_vertices());
}

void ParticleSystem::ImGui() {
    if (ImGui::Button("Reset"))
        reset_pos_and_vel();
    ImGui::PushID(321);
        _physics_params.ImGui();
    ImGui::Separator();
    ImGui::PopID();
    ImGui::PushID(322);
        _color_params.ImGui();
    ImGui::PopID();
    ImGui::PushID(322);
        _wind_params.ImGui();
    ImGui::PopID();
}

void ParticleSystem::onMouseButtonEvent(int button, int action, int mods) {

}