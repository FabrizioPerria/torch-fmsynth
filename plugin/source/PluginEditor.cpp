#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>
#include <sstream>
#include <string>
#include <torch/torch.h>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    addAndMakeVisible (frequencySlider);
    frequencySlider.setRange (20.0, 20000.0, 1.0);
    frequencySlider.setValue (440.0); // Default frequency
    frequencySlider.addListener (this);

    addAndMakeVisible (amplitudeSlider);
    amplitudeSlider.setRange (0.0, 1.0, 0.01);
    amplitudeSlider.setValue (0.5); // Default amplitude
    amplitudeSlider.addListener (this);
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
    frequencySlider.setBounds (10, 10, getWidth() - 20, 40);
    amplitudeSlider.setBounds (10, 60, getWidth() - 20, 40);
}

void AudioPluginAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &frequencySlider)
    {
        DBG ("Frequency changed to: " << frequencySlider.getValue());
        // processorRef.frequency = frequencySlider.getValue();
        // processorRef.phaseIncrement = processorRef.getPhaseIncrement(processorRef.frequency, processorRef.getSampleRate());
    }
    else if (slider == &amplitudeSlider)
    {
        // processorRef.amplitude = amplitudeSlider.getValue();
        DBG ("Amplitude changed to: " << amplitudeSlider.getValue());
    }
}
