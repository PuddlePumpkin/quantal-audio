#include "QuantalAudioExtendedMixer.hpp"
#include "Daisy.hpp"

struct DaisyBlank2 : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightsIds {
        LINK_LIGHT_L,
        LINK_LIGHT_R,
        NUM_LIGHTS
    };

    float link_l = 0.f;
    float link_r = 0.f;

    dsp::ClockDivider lightDivider;

    DaisyMessage daisyInputMessage[2][1];
    DaisyMessage daisyOutputMessage[2][1];

    DaisyBlank2() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configLight(LINK_LIGHT_L, "Daisy chain link input");
        configLight(LINK_LIGHT_R, "Daisy chain link output");

        // Set the expander messages
        leftExpander.producerMessage = &daisyInputMessage[0];
        leftExpander.consumerMessage = &daisyInputMessage[1];
        rightExpander.producerMessage = &daisyOutputMessage[0];
        rightExpander.consumerMessage = &daisyOutputMessage[1];

        lightDivider.setDivision(512);
    }

    void process(const ProcessArgs &args) override {

        float daisySignals_l[16] = {};
        float daisySignals_r[16] = {};
        int chainChannels = 1;

        // Get daisy-chained data from left-side linked module
        if (leftExpander.module && (
            leftExpander.module->model == modelDaisyChannel2
            || leftExpander.module->model == modelDaisyChannelVu
            || leftExpander.module->model == modelDaisyChannelSends2
            || leftExpander.module->model == modelDaisyChannelSends3
            || leftExpander.module->model == modelDaisyMaster2
            || leftExpander.module->model == modelDaisyBlank1
            || leftExpander.module->model == modelDaisyBlank2
        )) {
            DaisyMessage *msgFromModule = (DaisyMessage *)(leftExpander.consumerMessage);
            chainChannels = msgFromModule->channels;
            for (int c = 0; c < chainChannels; c++) {
                daisySignals_l[c] = msgFromModule->voltages_l[c];
                daisySignals_r[c] = msgFromModule->voltages_r[c];
            }

            link_l = 0.8f;
        } else {
            link_l = 0.0f;
        }

        // Set daisy-chained output to right-side linked module
        if (rightExpander.module && (
            rightExpander.module->model == modelDaisyMaster2
            || rightExpander.module->model == modelDaisyChannel2
            || rightExpander.module->model == modelDaisyChannelVu
            || rightExpander.module->model == modelDaisyChannelSends2
            || rightExpander.module->model == modelDaisyChannelSends3
            || rightExpander.module->model == modelDaisyBlank1
            || rightExpander.module->model == modelDaisyBlank2
        )) {
            DaisyMessage *msgToModule = (DaisyMessage *)(rightExpander.module->leftExpander.producerMessage);

            msgToModule->channels = chainChannels;
            for (int c = 0; c < chainChannels; c++) {
                msgToModule->voltages_l[c] = daisySignals_l[c];
                msgToModule->voltages_r[c] = daisySignals_r[c];
            }

            rightExpander.module->leftExpander.messageFlipRequested = true;

            link_r = 0.8f;
        } else {
            link_r = 0.0f;
        }

        // Set lights
        if (lightDivider.process()) {
            lights[LINK_LIGHT_L].setBrightness(link_l);
            lights[LINK_LIGHT_R].setBrightness(link_r);
        }
    }
};

struct DaisyBlank2Widget : ModuleWidget {
    DaisyBlank2Widget(DaisyBlank2 *module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/DaisyBlank2.svg"), asset::plugin(pluginInstance, "res/DaisyBlank2-dark.svg")));

        // Screws
        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // Link lights
        addChild(createLightCentered<TinyLight<YellowLight>>(Vec(RACK_GRID_WIDTH - 4, 361.0f), module, DaisyBlank2::LINK_LIGHT_L));
        addChild(createLightCentered<TinyLight<YellowLight>>(Vec(RACK_GRID_WIDTH + 4, 361.0f), module, DaisyBlank2::LINK_LIGHT_R));
    }
};

Model *modelDaisyBlank2 = createModel<DaisyBlank2, DaisyBlank2Widget>("DaisyBlank2");
