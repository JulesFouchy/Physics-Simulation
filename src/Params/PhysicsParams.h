#include <Cool/Params/Params.h>

using namespace Cool;

struct PhysicsParamValues {
	PhysicsParamValues() = default;

	Param::Float stiffness =        {"Stiffness",        5.f,   0.f, 100.f};
	Param::Float internal_damping = {"Internal Damping", 10.f,  0.f,  10.f};
	Param::Float external_damping = {"External Damping", 10.f,  0.f,  10.f};
	Param::Float gravity =          {"Gravity",          1.f,   0.f,  10.f};
	Param::Float size =             {"Size",             0.02f, 0.f,  0.1f};
	Param::Int nb_particles =       {"Number of Particles", 240, 1, 1000};

	bool ImGui(Action on_edit_ended, std::function<void()> on_value_change) {
		bool b = false;
		b |= stiffness.ImGui(on_edit_ended, on_value_change);
		b |= internal_damping.ImGui(on_edit_ended, on_value_change);
		b |= external_damping.ImGui(on_edit_ended, on_value_change);
		b |= gravity.ImGui(on_edit_ended, on_value_change);
		b |= size.ImGui(on_edit_ended, on_value_change);
		b |= nb_particles.ImGui(on_edit_ended, on_value_change);
		return b;
	}

private:
	// Serialization
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			stiffness,
			internal_damping,
			external_damping,
			gravity,
			size,
			nb_particles
		);
	}
};

class PhysicsParams : public Params<PhysicsParamValues> {
public:
	PhysicsParams(std::function<void()> on_value_change)
		: Params("physics", File::RootDir + "/presets", on_value_change)
	{}
};