#include <Cool/OpenGL/ComputeShader.h>
#include <Cool/OpenGL/SSBO.h>
#include "Params/ColorParams.h"
#include "Params/PhysicsParams.h"
#include "Params/WindParams.h"
#include "PoulpeBody.h"
#include <Cool/FileWatcher/FileWatcher.h>

using namespace Cool;

class ParticleSystem {
public:
	ParticleSystem();
	~ParticleSystem();

	void render(const glm::mat4& view_mat, const glm::mat4& proj_mat);
	void update();
	void ImGui();
	void init_vertices_and_indices();
	void reset_pos_and_vel();
	
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

	int _grid_width = 10;
	int _grid_height = 10;
	GLuint _vaoID;
	GLuint _vboID;
	GLuint _iboID;
	Shader _rendering_shader{ "shaders/particle.vert", "shaders/particle.frag" };
	ComputeShader<256> _init_vertices_cs{ "shaders/init_vertices.comp" };
	ComputeShader<256> _reset_pos_and_vel_cs{ "shaders/reset_pos_and_vel.comp" };
	ComputeShader<256> _update_physics_cs{ "shaders/update_physics.comp" };

private:
	//Serialization
	friend class cereal::access;
	template<class Archive>
	void save(Archive& archive) const
	{
		archive(
			cereal::make_nvp("Physics", _physics_params),
			cereal::make_nvp("Color", _color_params)
		);
	}
	template<class Archive>
	void load(Archive& archive)
	{
		archive(
			cereal::make_nvp("Physics", _physics_params),
			cereal::make_nvp("Color", _color_params)
		);
		on_nb_vertices_change();
	}
};