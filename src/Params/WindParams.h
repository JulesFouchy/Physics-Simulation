#include <Cool/Params/Params.h>

using namespace Cool;

struct WindParamValues {
	WindParamValues() = default;

	Param::Float pulsation = {"Pulsation", 0.5f, 0.f, 10.f};
	Param::Float strength = {"Strength", 40.f, 0.f, 100.f};
	Param::Float speed = {"Speed", 0.5f, 0.f, 2.f};
	Param::Dir3 direction = {"Direction"};

	bool ImGui(Action on_edit_ended, std::function<void()> on_value_change) {
		bool b = false;
		b |= pulsation.ImGui(on_edit_ended, on_value_change);
		b |= strength.ImGui(on_edit_ended, on_value_change);
		b |= speed.ImGui(on_edit_ended, on_value_change);
		b |= direction.ImGui(on_edit_ended, on_value_change);
		return b;
	}

private:
	// Serialization
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			pulsation,
			strength,
			speed,
			direction
		);
	}
};

class WindParams : public Params<WindParamValues> {
public:
	WindParams()
		: Params("wind", File::RootDir + "/presets")
	{}
};