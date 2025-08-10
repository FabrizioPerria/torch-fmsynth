#pragma once
#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <JuceHeader.h>

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor, public Slider::Listener
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void sliderValueChanged (juce::Slider* slider) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    Signal& mainSine;
    Envelope& envelope;

    juce::ToggleButton enableSignalButton;
    juce::ToggleButton enableModulationButton;
    juce::ToggleButton enableEnvelopeButton;

    juce::Slider amplitudeSlider;
    juce::Label amplitudeLabel;

    juce::Slider attackSlider;
    juce::Label attackLabel;

    juce::Slider decaySlider;
    juce::Label decayLabel;

    juce::Slider sustainSlider;
    juce::Label sustainLabel;

    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
