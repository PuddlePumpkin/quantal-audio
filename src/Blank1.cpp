#include "QuantalAudio.hpp"


struct Blank1Widget : ModuleWidget {
	Blank1Widget(Module *module) {
		setModule(module);
		setPanel(SVG::load(assetPlugin(pluginInstance, "res/blank-1.svg")));
	}
};


Model *modelBlank1 = createModel<Module, Blank1Widget>("Blank1");
