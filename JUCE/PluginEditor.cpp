/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChaoticGoodAudioProcessorEditor::ChaoticGoodAudioProcessorEditor (
    ChaoticGoodAudioProcessor& p, 
    juce::AudioProcessorValueTreeState& vts
) : 
    AudioProcessorEditor (&p), 
    audioProcessor (p), 
    mainComponent(&p, vts), 
    valueTreeState(vts)
{
//    setSize(960, 550);
    setSize(1260, 550);
    addAndMakeVisible(mainComponent);
}

ChaoticGoodAudioProcessorEditor::~ChaoticGoodAudioProcessorEditor()
{
}

//==============================================================================
void ChaoticGoodAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void ChaoticGoodAudioProcessorEditor::resized()
{
    mainComponent.setSize(getWidth(), getHeight());
    mainComponent.setTopLeftPosition(0, 0);
}
