#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include <sstream>
#include <string>
#include <torch/torch.h>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), mainSine (p.getMainSine()), envelope (mainSine.getEnvelope())
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);

    addAndMakeVisible (enableSignalButton);
    enableSignalButton.setButtonText ("Enable Signal");
    enableSignalButton.setToggleState (mainSine.isEnabled(), juce::dontSendNotification);
    enableSignalButton.onClick = [this]() { mainSine.setEnabled (enableSignalButton.getToggleState()); };

    addAndMakeVisible (enableEnvelopeButton);
    enableEnvelopeButton.setButtonText ("Enable Envelope");
    enableEnvelopeButton.setToggleState (envelope.isEnabled(), juce::dontSendNotification);
    enableEnvelopeButton.onClick = [this]() { envelope.setEnabled (enableEnvelopeButton.getToggleState()); };

    addAndMakeVisible (amplitudeLabel);
    amplitudeLabel.setText ("Amplitude", juce::dontSendNotification);
    addAndMakeVisible (amplitudeSlider);
    amplitudeSlider.setRange (0.0, 1.0, 0.01);
    amplitudeSlider.setValue (mainSine.getAmplitude());
    amplitudeSlider.addListener (this);

    addAndMakeVisible (attackLabel);
    attackLabel.setText ("Attack", juce::dontSendNotification);
    addAndMakeVisible (attackSlider);
    attackSlider.setRange (0.01, 1.0, 0.01);
    attackSlider.setValue (envelope.getEnvelopeAttack());
    attackSlider.addListener (this);

    addAndMakeVisible (decayLabel);
    decayLabel.setText ("Decay", juce::dontSendNotification);
    addAndMakeVisible (decaySlider);
    decaySlider.setRange (0.01, 1.0, 0.01);
    decaySlider.setValue (envelope.getEnvelopeDecay());
    decaySlider.addListener (this);

    addAndMakeVisible (sustainLabel);
    sustainLabel.setText ("Sustain", juce::dontSendNotification);
    addAndMakeVisible (sustainSlider);
    sustainSlider.setRange (0.0, 1.0, 0.01);
    sustainSlider.setValue (envelope.getEnvelopeSustain());
    sustainSlider.addListener (this);

    addAndMakeVisible (releaseLabel);
    releaseLabel.setText ("Release", juce::dontSendNotification);
    addAndMakeVisible (releaseSlider);
    releaseSlider.setRange (0.0, 1.0, 0.01);
    releaseSlider.setValue (envelope.getEnvelopeRelease());
    releaseSlider.addListener (this);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

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
}

void AudioPluginAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &amplitudeSlider)
    {
        processorRef.getMainSine().updateAmplitude (amplitudeSlider.getValue());
    }
    else if (slider == &attackSlider || slider == &decaySlider || slider == &sustainSlider || slider == &releaseSlider)
    {
        envelope.setEnvelopeParameters (attackSlider.getValue(),
                                        decaySlider.getValue(),
                                        sustainSlider.getValue(),
                                        releaseSlider.getValue());
    }
}
