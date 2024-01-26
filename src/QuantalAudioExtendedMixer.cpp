#include "QuantalAudioExtendedMixer.hpp"

Plugin *pluginInstance;

void init(Plugin *p) {
    pluginInstance = p;

    // Add all Models defined throughout the pluginInstance
    p->addModel(modelDaisyChannel2);
    p->addModel(modelDaisyChannelSends2);
    p->addModel(modelDaisyChannelSends3);
    p->addModel(modelDaisyChannelVu);
    p->addModel(modelDaisyBlank1);
    p->addModel(modelDaisyBlank2);
    p->addModel(modelDaisyMaster2);

    // Any other pluginInstance initialization may go here.
    // As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
