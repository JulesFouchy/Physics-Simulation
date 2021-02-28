#include <Cool/Params/Params.h>

using namespace Cool;

struct ColorParamValues {
	ColorParamValues() = default;
	ColorParamValues(std::function<void()> on_color_gradient_change)
		: _on_color_gradient_change(on_color_gradient_change)
	{}

	Param::Color color_gradient_begin = {"Color Gradient Begin", glm::vec3(0.1f, 0.8f, 0.7f)};
	Param::Color color_gradient_end   = {"Color Gradient End",   glm::vec3(0.9f, 0.4f, 0.7f)};
	Param::Color background           = {"Background",           glm::vec3(0.9f, 0.8f, 0.7f)};

	bool ImGui(Action on_edit_ended) {
		bool b = false;
		b |= color_gradient_begin.ImGui(on_edit_ended, _on_color_gradient_change);
		b |= color_gradient_end  .ImGui(on_edit_ended, _on_color_gradient_change);
		b |= background.ImGui(on_edit_ended);
		return b;
	}

	void on_all_values_change() {
		_on_color_gradient_change();
	}

private:
	std::function<void()> _on_color_gradient_change = []() {Log::ErrorWithoutBreakpoint(":("); };

private:
	// Serialization
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			color_gradient_begin,
			color_gradient_end,
			background
		);
	}
};

class ColorParams : public Params<ColorParamValues> {
public:
	ColorParams(std::function<void()> on_color_gradient_change)
		: Params("color", File::RootDir + "/presets", on_color_gradient_change)
	{}
};