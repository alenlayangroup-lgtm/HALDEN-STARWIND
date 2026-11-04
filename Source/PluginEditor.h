#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class HaldenStarwindAudioProcessorEditor final : public juce::GenericAudioProcessorEditor
{
public:
    explicit HaldenStarwindAudioProcessorEditor(HaldenStarwindAudioProcessor& processor)
        : juce::GenericAudioProcessorEditor(processor)
    {
        setSize(520, 680);
    }
};
