#include <Cool/OpenGL/ComputeShader.h>
#include <Cool/OpenGL/SSBO.h>
#include "Params/ColorParams.h"
#include "Params/PhysicsParams.h"
#include "Params/WindParams.h"
#include "PoulpeBody.h"
#include <Cool/FileWatcher/FileWatcher.h>
#include <Cool/OpenGL/Texture.h>

using namespace Cool;

class ParticleSystem {
public:
	ParticleSystem();
	~ParticleSystem();

	void render(const glm::mat4& view_mat, const glm::mat4& proj_mat);
	void update();
	void reset_pos_and_vel();
	void ImGui();
	void ImGuiWindows();
	
	void onMouseButtonEvent(int button, int action, int mods);
	inline void onMouseMoveEvent(double xPos, double yPos) {}

private:
	void on_nb_vertices_change();

	inline int nb_of_vertices() { return _grid_width * _grid_height; }
	inline int nb_of_indices() { return 6 * (_grid_width - 1) * (_grid_height - 1); }

private:
	PhysicsParams _physics_params;
	ColorParams _color_params;
	WindParams _wind_params;

	bool _out_to_0 = true;
	int _grid_width = 20;
	int _grid_height = 20;
	GLuint _vao0ID;
	GLuint _vbo0ID;
	GLuint _vao1ID;
	GLuint _vbo1ID;
	GLuint _ibo0ID;
	GLuint _ibo1ID;
	Shader _rendering_shader{ {{ShaderType::Vertex, "shaders/particle.vert"}, {ShaderType::Geometry, "shaders/particle.geom"} , {ShaderType::Fragment, "shaders/particle.frag"}} };
	ComputeShader<256> _init_vertices_cs{ "shaders/init_vertices.comp" };
	ComputeShader<256> _reset_pos_and_vel_cs{ "shaders/reset_pos_and_vel.comp" };
	ComputeShader<256> _update_physics_cs0{ "shaders/update0.comp" };
	ComputeShader<256> _update_physics_cs1{ "shaders/update1.comp" };
	SSBO<float> _vel_ssbo0{ 12 };
	SSBO<float> _vel_ssbo1{ 13 };
	Texture _flag_texture;

private:
	//Serialization
	friend class cereal::access;
	template<class Archive>
	void save(Archive& archive) const
	{
		archive(
			cereal::make_nvp("Physics", _physics_params),
			cereal::make_nvp("Color", _color_params),
			cereal::make_nvp("Wind", _wind_params)
		);
	}
	template<class Archive>
	void load(Archive& archive)
	{
		archive(
			cereal::make_nvp("Physics", _physics_params),
			cereal::make_nvp("Color", _color_params),
			cereal::make_nvp("Wind", _wind_params)
		);
		on_nb_vertices_change();
	}
};