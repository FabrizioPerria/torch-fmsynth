#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include <sstream>
#include <string>
#include <torch/torch.h>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), apvts (p.getAPVTS())
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 500);

    // ============================================================================================
    // ENABLE SIGNAL BUTTON

    auto& enableSignalParam = *apvts.getRawParameterValue ("main_enabled");
    addAndMakeVisible (enableSignalButton);
    enableSignalButton.setButtonText ("Enable Signal");
    enableSignalButton.setToggleState (enableSignalParam.load() > 0.5f, juce::dontSendNotification);
    enableSignalAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts,
                                                                                                     "main_enabled",
                                                                                                     enableSignalButton);
    // ============================================================================================
    // AMPLITUDE SLIDER

    addAndMakeVisible (amplitudeLabel);
    amplitudeLabel.setText ("Amplitude", juce::dontSendNotification);
    auto& amplitudeParam = *apvts.getRawParameterValue ("main_amplitude");
    auto amplitudeParamRange = apvts.getParameterRange ("main_amplitude");
    addAndMakeVisible (amplitudeSlider);
    amplitudeSlider.setRange (amplitudeParamRange.start, amplitudeParamRange.end, 0.01);
    amplitudeSlider.setValue (amplitudeParam.load());
    amplitudeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "main_amplitude", amplitudeSlider);
    // ============================================================================================
    // MODULATION RATIO SLIDER

    addAndMakeVisible (modulationRatioLabel);
    modulationRatioLabel.setText ("Modulation Ratio", juce::dontSendNotification);
    auto& modulationRatioParam = *apvts.getRawParameterValue ("main_modulation_ratio");
    auto modulationRatioParamRange = apvts.getParameterRange ("main_modulation_ratio");
    addAndMakeVisible (modulationRatioSlider);
    modulationRatioSlider.setRange (modulationRatioParamRange.start, modulationRatioParamRange.end, 0.01);
    modulationRatioSlider.setValue (modulationRatioParam.load());
    modulationRatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts,
                                                                                                        "main_modulation_ratio",
                                                                                                        modulationRatioSlider);
    // ============================================================================================
    // ENVELOPE BUTTON

    auto& envelopeEnabledParam = *apvts.getRawParameterValue ("main_envelope_enabled");
    addAndMakeVisible (enableEnvelopeButton);
    enableEnvelopeButton.setButtonText ("Enable Envelope");
    enableEnvelopeButton.setToggleState (envelopeEnabledParam.load() > 0.5f, juce::dontSendNotification);
    enableEnvelopeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts,
                                                                                                       "main_envelope_enabled",
                                                                                                       enableEnvelopeButton);
    // ============================================================================================
    // ENVELOPE ATTACK SLIDER

    addAndMakeVisible (attackLabel);
    attackLabel.setText ("Attack", juce::dontSendNotification);
    auto& attackParam = *apvts.getRawParameterValue ("main_envelope_attack");
    auto attackParamRange = apvts.getParameterRange ("main_envelope_attack");
    addAndMakeVisible (attackSlider);
    attackSlider.setRange (attackParamRange.start, attackParamRange.end, 0.01);
    attackSlider.setValue (attackParam.load());
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "main_envelope_attack", attackSlider);
    // ============================================================================================
    // ENVELOPE DECAY SLIDER

    addAndMakeVisible (decayLabel);
    decayLabel.setText ("Decay", juce::dontSendNotification);
    auto& decayParam = *apvts.getRawParameterValue ("main_envelope_decay");
    auto decayParamRange = apvts.getParameterRange ("main_envelope_decay");
    addAndMakeVisible (decaySlider);
    decaySlider.setRange (decayParamRange.start, decayParamRange.end, 0.01);
    decaySlider.setValue (decayParam.load());
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "main_envelope_decay", decaySlider);
    // ============================================================================================
    // ENVELOPE SUSTAIN SLIDER

    addAndMakeVisible (sustainLabel);
    sustainLabel.setText ("Sustain", juce::dontSendNotification);
    auto& sustainParam = *apvts.getRawParameterValue ("main_envelope_sustain");
    auto sustainParamRange = apvts.getParameterRange ("main_envelope_sustain");
    addAndMakeVisible (sustainSlider);
    sustainSlider.setRange (sustainParamRange.start, sustainParamRange.end, 0.01);
    sustainSlider.setValue (sustainParam.load());
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts,
                                                                                                "main_envelope_sustain",
                                                                                                sustainSlider);
    // ============================================================================================
    // ENVELOPE RELEASE SLIDER

    addAndMakeVisible (releaseLabel);
    releaseLabel.setText ("Release", juce::dontSendNotification);
    auto& releaseParam = *apvts.getRawParameterValue ("main_envelope_release");
    auto releaseParamRange = apvts.getParameterRange ("main_envelope_release");
    addAndMakeVisible (releaseSlider);
    releaseSlider.setRange (releaseParamRange.start, releaseParamRange.end, 0.01);
    releaseSlider.setValue (releaseParam.load());
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts,
                                                                                                "main_envelope_release",
                                                                                                releaseSlider);
    // ============================================================================================
    // MODULATION BUTTON

    auto& modulationEnabledParam = *apvts.getRawParameterValue ("main_mod_enabled");
    addAndMakeVisible (enableModulationButton);
    enableModulationButton.setButtonText ("Enable Modulation");
    enableModulationButton.setToggleState (modulationEnabledParam.load() > 0.5f, juce::dontSendNotification);
    enableModulationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts,
                                                                                                         "main_mod_enabled",
                                                                                                         enableModulationButton);
    // ============================================================================================
    // MODULATION DEPTH SLIDER

    addAndMakeVisible (modulationDepthLabel);
    modulationDepthLabel.setText ("Modulation Depth", juce::dontSendNotification);
    auto& modulationDepthParam = *apvts.getRawParameterValue ("main_mod_amplitude");
    auto modulationDepthParamRange = apvts.getParameterRange ("main_mod_amplitude");
    addAndMakeVisible (modulationDepthSlider);
    modulationDepthSlider.setRange (modulationDepthParamRange.start, modulationDepthParamRange.end, 0.01);
    modulationDepthSlider.setValue (modulationDepthParam.load());
    modulationDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts,
                                                                                                        "main_mod_amplitude",
                                                                                                        modulationDepthSlider);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    torch::Tensor tensor = torch::rand ({ 2, 3 });
    // Display the tensor as a string in the editor
    std::ostringstream oss;
    oss << "Tensor: " << tensor;
    std::string tensorString = oss.str();
    g.drawFittedText (tensorString, getLocalBounds(), juce::Justification::centred, 5);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    const auto labelX = 10;
    auto labelY = 10;
    const auto height = 40;
    const auto labelWidth = 80;
    const auto sliderX = labelX + labelWidth + 10;
    const auto sliderWidth = getWidth() - sliderX - 20;

    enableSignalButton.setBounds (labelX, labelY, 150, height);
    labelY += 40;

    amplitudeLabel.setBounds (labelX, labelY, labelWidth, height);
    amplitudeSlider.setBounds (sliderX, labelY, sliderWidth, height);
    labelY += 60;

    enableEnvelopeButton.setBounds (labelX, labelY, 150, height);
    labelY += 40;

    attackLabel.setBounds (labelX, labelY, labelWidth, height);
    attackSlider.setBounds (sliderX, labelY, sliderWidth, height);
    labelY += 40;

    decayLabel.setBounds (labelX, labelY, labelWidth, height);
    decaySlider.setBounds (sliderX, labelY, sliderWidth, height);
    labelY += 40;

    sustainLabel.setBounds (labelX, labelY, labelWidth, height);
    sustainSlider.setBounds (sliderX, labelY, sliderWidth, height);
    labelY += 40;

    releaseLabel.setBounds (labelX, labelY, labelWidth, height);
    releaseSlider.setBounds (sliderX, labelY, sliderWidth, height);
    labelY += 60;

    enableModulationButton.setBounds (labelX, labelY, 150, height);
    labelY += 40;

    modulationRatioLabel.setBounds (labelX, labelY, labelWidth, height);
    modulationRatioSlider.setBounds (sliderX, labelY, sliderWidth, height);
    labelY += 40;

    modulationDepthLabel.setBounds (labelX, labelY, labelWidth, height);
    modulationDepthSlider.setBounds (sliderX, labelY, sliderWidth, height);
}
