#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
constexpr auto mixId = "mix";
constexpr auto decayId = "decay";
constexpr auto roomId = "size";
constexpr auto widthId = "width";
constexpr auto dampingId = "damping";
constexpr auto hallEnabledId = "hallEnabled";
constexpr auto deEsserEnabledId = "deEsserEnabled";
constexpr auto chorusEnabledId = "chorusEnabled";
constexpr auto breezeId = "breeze";
constexpr auto stardustId = "stardust";
constexpr auto silkTailId = "silkTail";
constexpr auto freezeId = "freeze";
constexpr auto infiniteId = "infinite";
constexpr auto postEqEnabledId = "postEqEnabled";
}

HaldenStarwindAudioProcessor::HaldenStarwindAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "HALDEN_PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
HaldenStarwindAudioProcessor::createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;
    const auto addFloat = [&p](const char* id, const char* name, NormalisableRange<float> range, float value)
    {
        p.push_back(std::make_unique<AudioParameterFloat>(ParameterID { id, 1 }, name, range, value));
    };
    const auto addBool = [&p](const char* id, const char* name, bool value)
    {
        p.push_back(std::make_unique<AudioParameterBool>(ParameterID { id, 1 }, name, value));
    };

    addFloat(mixId, "Wet", { 0.0f, 1.0f, 0.001f }, 0.28f);
    addFloat(decayId, "Decay", { 0.1f, 0.98f, 0.001f }, 0.72f);
    addFloat(roomId, "Size", { 0.0f, 1.0f, 0.001f }, 0.68f);
    addFloat(widthId, "Width", { 0.0f, 1.0f, 0.001f }, 0.85f);
    addFloat(dampingId, "Damping", { 0.0f, 1.0f, 0.001f }, 0.42f);
    addFloat(breezeId, "Breeze", { 0.0f, 1.0f, 0.001f }, 0.18f);
    addFloat(stardustId, "Stardust", { 0.0f, 1.0f, 0.001f }, 0.08f);
    addFloat(silkTailId, "Silk Tail", { 0.0f, 1.0f, 0.001f }, 0.65f);
    addBool(hallEnabledId, "Hall", true);
    addBool(deEsserEnabledId, "De-Esser", false);
    addBool(chorusEnabledId, "Chorus", false);
    addBool(freezeId, "Freeze", false);
    addBool(infiniteId, "Infinite", false);
    addBool(postEqEnabledId, "Post EQ", false);
    return { p.begin(), p.end() };
}

void HaldenStarwindAudioProcessor::prepareToPlay(double sampleRate, int)
{
    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = 0.68f;
    rp.damping = 0.42f;
    rp.wetLevel = 1.0f;
    rp.dryLevel = 0.0f;
    rp.width = 0.85f;
    rp.freezeMode = 0.0f;
    hall.setParameters(rp);
    juce::ignoreUnused(sampleRate);
}

void HaldenStarwindAudioProcessor::releaseResources() {}

bool HaldenStarwindAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto mainIn = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();
    return mainIn == mainOut && (mainOut == juce::AudioChannelSet::mono()
                              || mainOut == juce::AudioChannelSet::stereo());
}

void HaldenStarwindAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const auto inputChannels = getTotalNumInputChannels();
    const auto outputChannels = getTotalNumOutputChannels();
    for (int ch = inputChannels; ch < outputChannels; ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    const auto wet = parameters.getRawParameterValue(mixId)->load();
    const auto decay = parameters.getRawParameterValue(decayId)->load();
    const auto size = parameters.getRawParameterValue(roomId)->load();
    const auto width = parameters.getRawParameterValue(widthId)->load();
    const auto damping = parameters.getRawParameterValue(dampingId)->load();
    const bool enabled = parameters.getRawParameterValue(hallEnabledId)->load() > 0.5f;
    const bool freeze = parameters.getRawParameterValue(freezeId)->load() > 0.5f
                     || parameters.getRawParameterValue(infiniteId)->load() > 0.5f;

    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = juce::jlimit(0.0f, 1.0f, size);
    rp.damping = juce::jlimit(0.0f, 1.0f, damping);
    rp.wetLevel = 1.0f;
    rp.dryLevel = 0.0f;
    rp.width = juce::jlimit(0.0f, 1.0f, width);
    rp.freezeMode = freeze ? 1.0f : 0.0f;
    hall.setParameters(rp);

    if (! enabled)
        return;

    juce::AudioBuffer<float> dry(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), buffer.getNumSamples());
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    hall.process(context);

    const auto feedbackScale = juce::jlimit(0.65f, 1.0f, decay);
    buffer.applyGain(feedbackScale);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* out = buffer.getWritePointer(ch);
        const auto* in = dry.getReadPointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            out[i] = in[i] * (1.0f - wet) + out[i] * wet;
    }
}

juce::AudioProcessorEditor* HaldenStarwindAudioProcessor::createEditor()
{
    return new HaldenStarwindAudioProcessorEditor(*this);
}

void HaldenStarwindAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}

void HaldenStarwindAudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HaldenStarwindAudioProcessor();
}
