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
        _color_params([this]() {}),
        _flag_texture(File::RootDir + "/textures/nono.jpg")
{
    // Vertex array
    GLCall(glGenVertexArrays(1, &_vao0ID));
    GLCall(glBindVertexArray(_vao0ID));
    // Vertex buffer
    GLCall(glGenBuffers(1, &_vbo0ID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo0ID));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _vbo0ID));
    // Index buffer
    GLCall(glGenBuffers(1, &_ibo0ID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo0ID));
    // Vertex Attribute pos
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos))));
    // Vertex Attribute uv
    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex,  uv))));
    // Vertex array
    GLCall(glGenVertexArrays(1, &_vao1ID));
    GLCall(glBindVertexArray(_vao1ID));
    // Vertex buffer
    GLCall(glGenBuffers(1, &_vbo1ID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo1ID));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _vbo1ID));
    // Index buffer
    GLCall(glGenBuffers(1, &_ibo1ID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo1ID));
    // Vertex Attribute pos
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos))));
    // Vertex Attribute uv
    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv))));
    //
    on_nb_vertices_change();
}

ParticleSystem::~ParticleSystem() {
    GLCall(glDeleteBuffers(1, &_vbo0ID));
    GLCall(glDeleteBuffers(1, &_vbo1ID));
    GLCall(glDeleteBuffers(1, &_ibo0ID));
    GLCall(glDeleteBuffers(1, &_ibo1ID));
    GLCall(glDeleteVertexArrays(1, &_vao0ID));
    GLCall(glDeleteVertexArrays(1, &_vao1ID));
}

void ParticleSystem::update() {
    auto& cs = _out_to_0 ? _update_physics_cs1 : _update_physics_cs0;
    cs->bind();
    cs->setUniform("_grid_width", _grid_width);
    cs->setUniform("_grid_height", _grid_height);
    cs->setUniform("_dt", Time::deltaTime());
    cs->setUniform("_time", Time::time());
    cs->setUniform("_stiffness", *_physics_params->stiffness);
    cs->setUniform("_internal_damping", *_physics_params->internal_damping);
    cs->setUniform("_external_damping", *_physics_params->external_damping);
    cs->setUniform("_gravity", *_physics_params->gravity);
    cs->setUniform("_wind_pulsation", *_wind_params->pulsation);
    cs->setUniform("_wind_strength", *_wind_params->strength);
    cs->setUniform("_wind_speed", *_wind_params->speed);
    cs->setUniform("_wind_dir", *_wind_params->direction);
    
    cs.compute(nb_of_vertices());

    std::vector<float> v(4 * nb_of_vertices());
    if (_out_to_0)
        _vel_ssbo0.downloadData(v);
    else
        _vel_ssbo1.downloadData(v);
    for (int i = 0; i < v.size(); i += 4) {
        Log::Info("{} {} {}", v[i], v[i + 1], v[i + 2]);
    }
    Log::Warn("--------------");
}

void ParticleSystem::render(const glm::mat4& view_mat, const glm::mat4& proj_mat) {
    //// Background
    //glClearColor(_color_params->background->r, _color_params->background->g, _color_params->background->b, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //// Mesh
    //_rendering_shader.bind();
    //_rendering_shader.setUniform("_view_mat", view_mat);
    //_rendering_shader.setUniform("_proj_mat", proj_mat);
    //_rendering_shader.setUniform("_texture", 0);
    //_flag_texture.attachToSlot(0);
    //GLCall(glBindVertexArray(_out_to_0 ? _vao0ID : _vao1ID));
    //GLCall(glDrawElements(GL_TRIANGLES, nb_of_indices(), GL_UNSIGNED_INT, 0));
    //GLCall(glBindVertexArray(0));
    _out_to_0 = !_out_to_0;
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
    _vel_ssbo0.uploadData(4 * nb_of_vertices(), nullptr);
    _vel_ssbo1.uploadData(4 * nb_of_vertices(), nullptr);
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
}

void ParticleSystem::ImGuiWindows() {
    ImGui::Begin("Physics");
    _physics_params.ImGui();
    ImGui::End();
    ImGui::Begin("Wind");
    _wind_params.ImGui();
    ImGui::End();
    ImGui::Begin("Colors");
    _color_params.ImGui();
    ImGui::End();
}

void ParticleSystem::onMouseButtonEvent(int button, int action, int mods) {

}