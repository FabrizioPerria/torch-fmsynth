#pragma once
#include "PluginProcessor.h"
#include "SuperSlider.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <JuceHeader.h>

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    juce::AudioProcessorValueTreeState& apvts;

    juce::ToggleButton enableSignalButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enableSignalAttachment;

    juce::Label amplitudeLabel;
    juce::Slider amplitudeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amplitudeAttachment;

    juce::ToggleButton enableEnvelopeButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enableEnvelopeAttachment;

    juce::Label attackLabel;
    juce::Slider attackSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;

    juce::Label decayLabel;
    juce::Slider decaySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;

    juce::Label sustainLabel;
    juce::Slider sustainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;

    juce::Label releaseLabel;
    juce::Slider releaseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    juce::ToggleButton enableModulationButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enableModulationAttachment;

    juce::Label modulationRatioLabel;
    juce::Slider modulationRatioSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modulationRatioAttachment;

    juce::Label modulationDepthLabel;
    juce::Slider modulationDepthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modulationDepthAttachment;

    juce::Label modulationSuperKnobLabel;
    SuperSlider modulationSuperKnobSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
