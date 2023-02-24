/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MainComponent.cpp"

//==============================================================================
/**
*/
class ChaoticGoodAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ChaoticGoodAudioProcessorEditor (ChaoticGoodAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~ChaoticGoodAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ChaoticGoodAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    MainComponent mainComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChaoticGoodAudioProcessorEditor)
};
