#include <Cool/Params/Params.h>

using namespace Cool;

struct WindParamValues {
	WindParamValues() = default;

	Param::Float noise_puls = {"Pulsation", 5.f, 0.f, 100.f};
	Param::Float min_strength = {"Min Strength", 5.f, 0.f, 100.f};
	Param::Float max_strength = {"Max Strength", 5.f, 0.f, 100.f};
	Param::Float direction_angle = {"Direction", 5.f, 0.f, 100.f};
	Param::Float offset = {"Offset", 5.f, 0.f, 100.f};

	bool ImGui(Action on_edit_ended, std::function<void()> on_value_change) {
		bool b = false;
		b |= noise_puls.ImGui(on_edit_ended, on_value_change);
		b |= min_strength.ImGui(on_edit_ended, on_value_change);
		b |= max_strength.ImGui(on_edit_ended, on_value_change);
		b |= direction_angle.ImGui(on_edit_ended, on_value_change);
		b |= offset.ImGui(on_edit_ended, on_value_change);
		return b;
	}

private:
	// Serialization
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			noise_puls,
			min_strength,
			max_strength,
			direction_angle,
			offset
		);
	}
};

class WindParams : public Params<WindParamValues> {
public:
	WindParams()
		: Params("wind", File::RootDir + "/presets")
	{}
};