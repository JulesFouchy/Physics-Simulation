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
    : _rendering_shader("shaders/particle.vert", "shaders/particle.frag"),
      _physics_params([this]() {on_nb_particles_change(); }),
      _color_params([this]() {}),
      _reset_pos_and_vel_cs("shaders/reset_pos_and_vel.comp"),
      _update_physics_cs("shaders/update_physics.comp")
{
    // Vertex array
    GLCall(glGenVertexArrays(1, &_vaoID));
    GLCall(glBindVertexArray(_vaoID));
    // Vertex & Index buffer
    GLCall(glGenBuffers(1, &_vboID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vboID));
    GLCall(glGenBuffers(1, &_iboID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboID));
    std::vector<Vertex> vertices(_grid_width * _grid_height);
    std::vector<unsigned int> indices; indices.reserve(3 * nb_of_triangles());
    for (int y = 0; y < _grid_height; ++y) {
        for (int x = 0; x < _grid_width; ++x) {
            int idx = x + y * _grid_width;
            // Vertex
            vertices[idx].uv = glm::vec2(
                x / float(_grid_width - 1),
                y / float(_grid_height - 1)
            );
            // Index
            if (x != _grid_width - 1 && y != _grid_height - 1) {
                indices.push_back(idx);
                indices.push_back(idx + _grid_width + 1);
                indices.push_back(idx + _grid_width);

                indices.push_back(idx);
                indices.push_back(idx + 1);
                indices.push_back(idx + _grid_width + 1);
            }
        }
    }
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _vboID));
    // Vertex Attribute pos
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos))));
    // Vertex Attribute uv
    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex,  uv))));
    //
    on_nb_particles_change();
}

int ParticleSystem::nb_of_triangles() {
    return 2 * (_grid_width - 1) * (_grid_height - 1);
}

int ParticleSystem::nb_of_vertices() {
    return _grid_width * _grid_height;
}

ParticleSystem::~ParticleSystem() {
    GLCall(glDeleteBuffers(1, &_vboID));
    GLCall(glDeleteBuffers(1, &_iboID));
    GLCall(glDeleteVertexArrays(1, &_vaoID));
}

void ParticleSystem::render(const glm::mat4& view_mat, const glm::mat4& proj_mat) {
    // Background
    glClearColor(_color_params->background->r, _color_params->background->g, _color_params->background->b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Particles
    _rendering_shader.bind();
    _rendering_shader.setUniformMat4f("_view_mat", view_mat);
    _rendering_shader.setUniformMat4f("_proj_mat", proj_mat);
    GLCall(glBindVertexArray(_vaoID));
    GLCall(glDrawElements(GL_TRIANGLES, 3 * nb_of_triangles(), GL_UNSIGNED_INT, 0));
}

void ParticleSystem::reset_pos_and_vel() {
    _reset_pos_and_vel_cs->bind();
    _reset_pos_and_vel_cs->setUniform1i("_grid_width", _grid_width);
    _reset_pos_and_vel_cs->setUniform1i("_grid_height", _grid_height);
    _reset_pos_and_vel_cs.compute(nb_of_vertices());
}

void ParticleSystem::update() {
    _update_physics_cs->bind();
    _update_physics_cs->setUniform1i("_grid_width", _grid_width);
    _update_physics_cs->setUniform1i("_grid_height", _grid_height);
    _update_physics_cs.compute(nb_of_vertices());
}

void ParticleSystem::on_nb_particles_change() {
    reset_pos_and_vel();
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