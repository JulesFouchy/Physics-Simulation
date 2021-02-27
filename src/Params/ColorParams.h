#include <Cool/Params/Params.h>

using namespace Cool;

struct ColorParamValues {
	Param::Color color_gradient_begin = {"Color Gradient Begin", glm::vec3(0.1f, 0.8f, 0.7f)};
	Param::Color color_gradient_end   = {"Color Gradient End",   glm::vec3(0.9f, 0.4f, 0.7f)};
	Param::Color background           = {"Background",           glm::vec3(0.9f, 0.8f, 0.7f)};

	bool ImGui() {
		bool b = false;
		b |= color_gradient_begin.ImGui();
		b |= color_gradient_end.ImGui();
		b |= background.ImGui();
		return b;
	}

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
	ColorParams()
		: Params("color", File::RootDir + "/presets")
	{}
};