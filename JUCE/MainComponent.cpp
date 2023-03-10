#pragma once

#include <JuceHeader.h>
#include <BinaryData.h>
#include "PluginProcessor.h"
#include "GaloisLookAndFeel.cpp"
#include <cstdlib>

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//#define FACTORY_PRESET_BUTTON

class MainComponent : public juce::Component
{
public:
    //==============================================================================
    MainComponent (
        ChaoticGoodAudioProcessor* ap, juce::AudioProcessorValueTreeState& vts
    ) : valueTreeState(vts)
    {
#ifdef FACTORY_PRESET_BUTTON

        factoryPresetButton.setButtonText("SAVE");
        addAndMakeVisible(factoryPresetButton);
        factoryPresetButton.onClick = [this] { presetButtonClicked(); };
        factoryPresetNameLabel.setEditable(true);
        addAndMakeVisible(factoryPresetNameLabel);

#endif // FACTORY_PRESET_BUTTON

        audioProcessor = ap;

        juce::PNGImageFormat format;
        juce::MemoryInputStream* mis = new juce::MemoryInputStream(BinaryData::logo_png, BinaryData::logo_pngSize, false);
        juce::Image img = format.decodeImage(*mis);
        delete(mis);
        logoImage.setImage(img);
        addAndMakeVisible(logoImage);

        mis = new juce::MemoryInputStream(BinaryData::background_v3_png, BinaryData::background_v3_pngSize, false);
        img = format.decodeImage(*mis);
        delete(mis);
        backgroundImage.setImage(img);
        addAndMakeVisible(backgroundImage);

        makeSlider(inputSlider, inputSliderLabel, "amplifier_gain", inputSliderAttachment, false, true, true);
        makeSlider(outputSlider, outputSliderLabel, "output_level", outputSliderAttachment, false, true, true);
        makeSlider(typeSlider, typeSliderLabel, "amplifier_structure", typeSliderAttachment, false, true, true);

        makeSlider(powerSlider, powerSliderLabel, "power_positive", powerSliderAttachment, false, true, true);
        makeSlider(dryBlendSlider, dryBlendSliderLabel, "amp_blend", dryBlendSliderAttachment, false, true, true);
        makeSlider(lowCutoffSlider, lowCutoffSliderLabel, "low_cutoff", lowCutoffSliderAttachment, false, true, true);
        makeSlider(midCutoffSlider, midCutoffSliderLabel, "mid_cutoff", midCutoffSliderAttachment, false, true, true);
        makeSlider(highCutoffSlider, highCutoffSliderLabel, "high_cutoff", highCutoffSliderAttachment, false, true, true);

        makeSlider(rmFreqASlider, rmFreqASliderLabel, "ring_mod_freq_A", rmFreqASliderAttachment, false, true, true);
        makeSlider(rmFreqBSlider, rmFreqBSliderLabel, "ring_mod_freq_B", rmFreqBSliderAttachment, false, true, true);
        makeSlider(MIDI_rmFreqASlider, MIDI_rmFreqASliderLabel, "ring_mod_freq_A_MIDI", MIDI_rmFreqASliderAttachment, true, true, true);
        makeSlider(MIDI_rmFreqBSlider, MIDI_rmFreqBSliderLabel, "ring_mod_freq_B_MIDI", MIDI_rmFreqBSliderAttachment, true, true, true);

        makeSlider(rmAmountSlider, rmAmountSliderLabel, "ring_mod_amt", rmAmountSliderAttachment, false, true, true);
        makeSlider(rmFMAmountSlider, rmFMAmountSliderLabel, "ring_mod_fm_amt", rmFMAmountSliderAttachment, false, true, true);
        makeSlider(rmXFadeSlider, rmXFadeSliderLabel, "ring_mod_xfade", rmXFadeSliderAttachment, true, true, true);

        rmWaveA.setButtonText(audioProcessor->get_rm_wave_type(0));
        rmWaveA.onClick = [this] { rmWaveAClicked(); };
        rmWaveA.setSize(35, 20);
        rmWaveA.setColour(juce::Label::textColourId, juce::Colours::lightcoral);
        rmWaveA.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(rmWaveA);

        rmWaveB.setButtonText(audioProcessor->get_rm_wave_type(1));
        rmWaveB.onClick = [this] { rmWaveBClicked(); };
        rmWaveB.setSize(35, 20);
        rmWaveB.setColour(juce::Label::textColourId, juce::Colours::lightcoral);
        rmWaveB.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(rmWaveB);

        combModModeButton.setButtonText(audioProcessor->get_comb_mod_mode_type());
        combModModeButton.onClick = [this] { combModModeClicked(); };
        combModModeButton.setSize(50, 20);
        combModModeButton.setColour(juce::Label::textColourId, juce::Colours::lightcoral);
        combModModeButton.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(combModModeButton);

        makeSlider(rmModeSlider, rmModeSliderLabel, "ring_mod_mode", rmModeSliderAttachment, false, true, true);
        rmModeSlider.onValueChange = [this] { resized(); };
//        makeSlider(flangerLFORateSlider, flangerLFORateSliderLabel, "noise_cutoff_feedback_amount", flangerLFORateSliderAttachment, false, true, true);
        makeSlider(flangerLFODepthSlider, flangerLFODepthSliderLabel, "flanger_lfo_depth", flangerLFODepthSliderAttachment, false, true, true);

        makeSlider(flangerAmountSlider, flangerAmountSliderLabel, "flanger_amount", flangerAmountSliderAttachment, false, true, true);
        makeSlider(flangerCentreSliderA, flangerCentreSliderALabel, "flanger_centre_position_A", flangerCentreSliderAAttachment, false, true, true);
        makeSlider(MIDI_flangerCentreSliderA, MIDI_flangerCentreSliderALabel, "flanger_centre_position_MIDI_A", MIDI_flangerCentreSliderAAttachment, true, true, true);
        makeSlider(flangerCentreSliderB, flangerCentreSliderBLabel, "flanger_centre_position_B", flangerCentreSliderBAttachment, false, true, true);
        makeSlider(MIDI_flangerCentreSliderB, MIDI_flangerCentreSliderBLabel, "flanger_centre_position_MIDI_B", MIDI_flangerCentreSliderBAttachment, true, true, true);
        makeSlider(flangerResonanceSlider, flangerResonanceSliderLabel, "flanger_resonance", flangerResonanceSliderAttachment, false, true, true);
        makeSlider(flangerAMSlider, flangerAMSliderLabel, "flanger_AM", flangerAMSliderAttachment, false, true, true);
        makeSlider(flangerModeSlider, flangerModeSliderLabel, "comb_mode", flangerModeSliderAttachment, false, true, true);
        flangerModeSlider.onValueChange = [this] { resized(); };

        makeSlider(noiseDensitySlider, noiseDensitySliderLabel, "noise_density", noiseDensitySliderAttachment, false, true, true);
        makeSlider(noiseDensityVarSlider, noiseDensityVarSliderLabel, "noise_density_variation", noiseDensityVarSliderAttachment, false, true, true);
        makeSlider(noiseLevelSlider, noiseLevelSliderLabel, "noise_level", noiseLevelSliderAttachment, false, true, true);
        makeSlider(noiseFilterCutoffSlider, noiseFilterCutoffSliderLabel, "noise_filter_cutoff", noiseFilterCutoffSliderAttachment, false, true, true);
        makeSlider(MIDI_noiseFilterCutoffSlider, MIDI_noiseFilterCutoffSliderLabel, "noise_filter_cutoff_MIDI", MIDI_noiseFilterCutoffSliderAttachment, true, true, true);
        makeSlider(noiseFilterResonanceSlider, noiseFilterResonanceSliderLabel, "noise_filter_resonance", noiseFilterResonanceSliderAttachment, false, true, true);
        makeSlider(noiseFilterTypeSlider, noiseFilterTypeSliderLabel, "noise_filter_type", noiseFilterTypeSliderAttachment, false, true, true);

        makeSlider(jitterDepthSlider, jitterDepthSliderLabel, "jitter_depth", jitterDepthSliderAttachment, false, true, true);
        makeSlider(jitterFrequencySlider, jitterFrequencySliderLabel, "jitter_frequency", jitterFrequencySliderAttachment, false, true, true);
        makeSlider(jitterDutyCycleSlider, jitterDutyCycleSliderLabel, "jitter_duty_cycle", jitterDutyCycleSliderAttachment, true, true, true);
        makeSlider(jitterRegularitySlider, jitterRegularitySliderLabel, "jitter_regularity", jitterRegularitySliderAttachment, false, true, true);
        makeSlider(jitterSlewSlider, jitterSlewSliderLabel, "jitter_slew", jitterSlewSliderAttachment, false, true, true);

        makeHeadingLabel(noiseSectionLabel, "Noise");
        makeHeadingLabel(ringModSectionLabel, "Ring Modulation");
        makeHeadingLabel(flangerSectionLabel, "Comb Filter");
        makeHeadingLabel(ampSectionLabel, "Amplifier");
        makeHeadingLabel(inputSectionLabel, "Input");
        makeHeadingLabel(toneSectionLabel, "Tone");
        makeHeadingLabel(outputSectionLabel, "Output");

        lookAndFeel.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::darkgrey);
        lookAndFeel.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::lightcoral);

        noiseMidiButton.onClick = [this] { resized(); };
        noiseMidiButton.setSize(35, 20);
        makeButton(noiseMidiButton, "MIDI", "receive_midi_noise", noiseMidiButtonAttachment);
        ringAMidiButton.onClick = [this] { resized(); };
        ringAMidiButton.setSize(35, 20);
        makeButton(ringAMidiButton, "MIDI", "receive_midi_ringA", ringAMidiButtonAttachment);
        ringBMidiButton.onClick = [this] { resized(); };
        ringBMidiButton.setSize(35, 20);
        makeButton(ringBMidiButton, "MIDI", "receive_midi_ringB", ringBMidiButtonAttachment);
        combAMidiButton.onClick = [this] { resized(); };
        combAMidiButton.setSize(35, 20);
        makeButton(combAMidiButton, "MIDI", "receive_midi_comb_A", combAMidiButtonAttachment);
        combBMidiButton.onClick = [this] { resized(); };
        combBMidiButton.setSize(35, 20);
        makeButton(combBMidiButton, "MIDI", "receive_midi_comb_B", combBMidiButtonAttachment);

        makeButton(rmALFOButton, "LFO", "ring_mod_low_A", rmALFOButtonAttachment);
        rmALFOButton.setSize(35, 20);
        makeButton(rmBLFOButton, "LFO", "ring_mod_low_B", rmBLFOButtonAttachment);
        rmBLFOButton.setSize(35, 20);

        makeButton(jitterLFOButton, "LFO", "jitter_low", jitterLFOButtonAttachment);
        jitterLFOButton.setSize(35, 20);
        makeButton(filterInputButton, "IN", "filter_input", filterInputButtonAttachment);
        filterInputButton.setSize(35, 20);
    }

    void makeButton(
        juce::TextButton& button,
        const char* text,
        const juce::String param,
        std::unique_ptr<ButtonAttachment>& ba
    ) {
        addAndMakeVisible(button);
        button.setClickingTogglesState(true);
        button.setWantsKeyboardFocus(false);
        button.setButtonText(text);
        button.setToggleable(true);
        button.setLookAndFeel(&lookAndFeel);
        button.toFront(false);

        ButtonAttachment* baptr = new ButtonAttachment(valueTreeState, param, button);
        ba.reset(baptr);
    }

    void makeHeadingLabel(
        juce::Label& label,
        const char* text
    ) {
        addAndMakeVisible(label);
        label.setWantsKeyboardFocus(false);
        label.setText(text, juce::NotificationType::dontSendNotification);
        label.setFont(juce::Font(24.0f, juce::Font::plain));
        label.setColour(juce::Label::textColourId, juce::Colours::black);
        label.setColour(juce::Label::backgroundColourId, juce::Colours::lightgreen);
        label.setColour(juce::Label::outlineColourId, juce::Colours::lightgreen);
        label.setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::black);
        label.setJustificationType(juce::Justification::centred);
        label.toFront(false);
    }

    void makeSlider(
        juce::Slider& slider, 
        juce::Label& label, 
        const char* param, 
        std::unique_ptr<SliderAttachment>& sa, 
        bool centred=false, bool text_box=true, bool show_label=true
    ) {
        addAndMakeVisible(slider);
        if (centred) {
            slider.setLookAndFeel(&lookAndFeelCentred);
        }
        else {
            slider.setLookAndFeel(&lookAndFeel);
        }
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        if (text_box) {
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, slider.getTextBoxHeight());
        }
        else {
            slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        }
        slider.setWantsKeyboardFocus(true);

        SliderAttachment* saptr = new SliderAttachment(valueTreeState, param, slider);
        sa.reset(saptr);
        if (show_label) {
            addAndMakeVisible(label);

            juce::String text = valueTreeState.getParameter(param)->name;
            label.setText(text, juce::NotificationType::dontSendNotification);
            label.setFont(juce::Font(18.0f, juce::Font::plain));
            label.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
            label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
            label.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
            label.setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
            label.setJustificationType(juce::Justification::centred);
            label.attachToComponent(&slider, false);
        }
    }

    ~MainComponent()
    {
        // ...
    }

    void paint(juce::Graphics& g) {
       g.fillAll(juce::Colours::slategrey);
       g.setColour(juce::Colours::black);
       g.setFillType(juce::FillType(juce::Colours::black));
       int line_spacer = knob_size + knob_spacer * 5;
    }

    void resized() override
    {
        backgroundImage.setSize(getWidth(), getHeight());
        backgroundImage.setTopLeftPosition(0, 0);

        int line_spacer = knob_size + knob_spacer * 7;
        int col_spacer = knob_size + knob_spacer + knob_size + knob_spacer * 8;
        int left_margin = 30;
        int top_margin = 100;

        int xpos = left_margin;
        int ypos = top_margin;

        // Jitter
        placeSlider(jitterDepthSlider, jitterDepthSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(jitterDutyCycleSlider, jitterDutyCycleSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(jitterSlewSlider, jitterSlewSliderLabel, xpos, ypos);
        
        ypos = top_margin;
        xpos += knob_size + knob_spacer;
        placeSlider(jitterFrequencySlider, jitterFrequencySliderLabel, xpos, ypos);
        jitterLFOButton.setTopLeftPosition(xpos + 80, ypos);
        jitterLFOButton.setSize(35, 25);
        ypos += line_spacer;
        placeSlider(jitterRegularitySlider, jitterRegularitySliderLabel, xpos, ypos);
        ypos += line_spacer;

        // Noise

        xpos = left_margin + col_spacer;
        ypos = top_margin;

        placeSlider(noiseLevelSlider, noiseLevelSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(noiseDensitySlider, noiseDensitySliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(noiseDensityVarSlider, noiseDensityVarSliderLabel, xpos, ypos);

        ypos = top_margin;
        xpos += knob_size + knob_spacer;
        if (audioProcessor->cached_midi_noise) {
            noiseFilterCutoffSlider.setVisible(false);
            MIDI_noiseFilterCutoffSlider.setVisible(true);
            placeSlider(MIDI_noiseFilterCutoffSlider, MIDI_noiseFilterCutoffSliderLabel, xpos, ypos);
        }
        else {
            MIDI_noiseFilterCutoffSlider.setVisible(false);
            noiseFilterCutoffSlider.setVisible(true);
            placeSlider(noiseFilterCutoffSlider, noiseFilterCutoffSliderLabel, xpos, ypos);
        }
        noiseMidiButton.setTopLeftPosition(xpos + 80, ypos + 60);
        ypos += line_spacer;
        placeSlider(noiseFilterResonanceSlider, noiseFilterResonanceSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(noiseFilterTypeSlider, noiseFilterTypeSliderLabel, xpos, ypos);
        filterInputButton.setTopLeftPosition(xpos + 80, ypos);
        filterInputButton.setSize(35, 25);

        // Ring Modulator

        xpos = left_margin + col_spacer * 2;
        ypos = top_margin;

        if (audioProcessor->cached_midi_ringA) {
            rmFreqASlider.setVisible(false);
            MIDI_rmFreqASlider.setVisible(true);
            placeSlider(MIDI_rmFreqASlider, MIDI_rmFreqASliderLabel, xpos, ypos);
        }
        else {
            MIDI_rmFreqASlider.setVisible(false);
            rmFreqASlider.setVisible(true);
            placeSlider(rmFreqASlider, rmFreqASliderLabel, xpos, ypos);
        }
        rmWaveA.setTopLeftPosition(xpos + 80, ypos);
        rmWaveA.toFront(false);
        rmALFOButton.setTopLeftPosition(xpos + 85, ypos + 30);
        ringAMidiButton.setTopLeftPosition(xpos + 80, ypos + 60);

        ypos += line_spacer;
        placeSlider(rmAmountSlider, rmAmountSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(rmModeSlider, rmModeSliderLabel, xpos, ypos);
        
        ypos = top_margin;
        xpos += knob_size + knob_spacer;
        if (audioProcessor->cached_ring_mod_mode == RM_MODE_SINGLE) {
            rmFreqBSlider.setVisible(false);
            MIDI_rmFreqBSlider.setVisible(false);
            rmBLFOButton.setVisible(false);
            rmWaveB.setVisible(false);
            ringBMidiButton.setVisible(false);
        } else {
            if (audioProcessor->cached_midi_ringB) {
                rmFreqBSlider.setVisible(false);
                MIDI_rmFreqBSlider.setVisible(true);
                placeSlider(MIDI_rmFreqBSlider, MIDI_rmFreqBSliderLabel, xpos, ypos);
            }
            else {
                MIDI_rmFreqBSlider.setVisible(false);
                rmFreqBSlider.setVisible(true);
                placeSlider(rmFreqBSlider, rmFreqBSliderLabel, xpos, ypos);
            }
            rmWaveB.setTopLeftPosition(xpos + 80, ypos);
            rmWaveB.toFront(false);
            rmBLFOButton.setTopLeftPosition(xpos + 85, ypos + 30);
            ringBMidiButton.setTopLeftPosition(xpos + 80, ypos + 60);
            rmBLFOButton.setVisible(true);
            rmWaveB.setVisible(true);
            ringBMidiButton.setVisible(true);
        }

        ypos += line_spacer;
        //        placeSlider(rmWaveSlider, rmWaveSliderLabel, xpos, ypos);
        ypos += line_spacer;
        switch (audioProcessor->cached_ring_mod_mode) {
        case RM_MODE_SINGLE:
            rmFMAmountSlider.setVisible(false);
            rmXFadeSlider.setVisible(false);
            break;
        case RM_MODE_SERIES:
            rmFMAmountSlider.setVisible(false);
            rmXFadeSlider.setVisible(false);
            break;
        case RM_MODE_PARALLEL:
            rmFMAmountSlider.setVisible(false);
            rmXFadeSlider.setVisible(true);
            placeSlider(rmXFadeSlider, rmFMAmountSliderLabel, xpos, ypos);
            break;
        default:
            rmXFadeSlider.setVisible(false);
            rmFMAmountSlider.setVisible(true);
            placeSlider(rmFMAmountSlider, rmFMAmountSliderLabel, xpos, ypos);
        }


        // Comb Filter
        
        xpos = left_margin + col_spacer * 3;
        ypos = top_margin;

        if (audioProcessor->cached_midi_comb_A) {
            flangerCentreSliderA.setVisible(false);
            MIDI_flangerCentreSliderA.setVisible(true);
            placeSlider(MIDI_flangerCentreSliderA, MIDI_flangerCentreSliderALabel, xpos, ypos);
        }
        else {
            MIDI_flangerCentreSliderA.setVisible(false);
            flangerCentreSliderA.setVisible(true);
            placeSlider(flangerCentreSliderA, flangerCentreSliderALabel, xpos, ypos);
        }
        combAMidiButton.setTopLeftPosition(xpos + 80, ypos + 60);
        ypos += line_spacer;
        placeSlider(flangerAmountSlider, flangerAmountSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(flangerModeSlider, flangerModeSliderLabel, xpos, ypos);

        ypos = top_margin;
        xpos += knob_size + knob_spacer;

        if (audioProcessor->cached_comb_mode == COMB_MODE_SINGLE) {
            flangerCentreSliderB.setVisible(false);
            MIDI_flangerCentreSliderB.setVisible(false);
            combBMidiButton.setVisible(false);
        }
        else {
            if (audioProcessor->cached_midi_comb_B) {
                flangerCentreSliderB.setVisible(false);
                MIDI_flangerCentreSliderB.setVisible(true);
                placeSlider(MIDI_flangerCentreSliderB, MIDI_flangerCentreSliderBLabel, xpos, ypos);
            }
            else {
                MIDI_flangerCentreSliderB.setVisible(false);
                flangerCentreSliderB.setVisible(true);
                placeSlider(flangerCentreSliderB, flangerCentreSliderBLabel, xpos, ypos);
            }
            combBMidiButton.setTopLeftPosition(xpos + 80, ypos + 60);
            combBMidiButton.setVisible(true);
        }
        ypos += line_spacer;
        placeSlider(flangerResonanceSlider, flangerResonanceSliderLabel, xpos, ypos);
        //        placeSlider(flangerLFORateSlider, flangerLFORateSliderLabel, xpos, ypos);

        ypos += line_spacer;
        placeSlider(flangerAMSlider, flangerAMSliderLabel, xpos, ypos);
        combModModeButton.setTopLeftPosition(xpos + 80, ypos);
        combModModeButton.toFront(false);
        ypos += line_spacer;

        // Amplifier

        xpos = left_margin + col_spacer * 4;
        ypos = top_margin;
        int amp_xpos = xpos;
  //      inputSectionLabel.setTopLeftPosition(xpos, 40);
  //      inputSectionLabel.setSize(100, 30);

        placeSlider(typeSlider, typeSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(inputSlider, inputSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(powerSlider, powerSliderLabel, xpos, ypos);

        ypos = top_margin;
        xpos += knob_size + knob_spacer;

  //      toneSectionLabel.setTopLeftPosition(xpos, 40);
  //      toneSectionLabel.setSize(100, 30);

        placeSlider(lowCutoffSlider, lowCutoffSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(midCutoffSlider, midCutoffSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(highCutoffSlider, highCutoffSliderLabel, xpos, ypos);

        ypos = top_margin;
        xpos += knob_size + knob_spacer;

 //       outputSectionLabel.setTopLeftPosition(xpos, 40);
 //       outputSectionLabel.setSize(100, 30);

        placeSlider(dryBlendSlider, dryBlendSliderLabel, xpos, ypos);
        ypos += line_spacer;
        placeSlider(outputSlider, outputSliderLabel, xpos, ypos);
        ypos += line_spacer;
        logoImage.setSize(knob_size * 0.9, knob_size * 0.9);
        logoImage.setTopLeftPosition(xpos, ypos);
        logoImage.toFront(false);
 //       ampSectionLabel.setTopLeftPosition(amp_xpos, 15);
 //       ampSectionLabel.setSize(334, 30);
 //       ampSectionLabel.toFront(false);

#ifdef FACTORY_PRESET_BUTTON
        factoryPresetButton.setSize(knob_size, knob_size/4);
        factoryPresetButton.setTopLeftPosition(xpos , 20);
        factoryPresetButton.toFront(false);
        factoryPresetNameLabel.setSize(knob_size, knob_size/4);
        factoryPresetNameLabel.setTopLeftPosition(xpos, 40);
        factoryPresetNameLabel.toFront(false);
#endif
    }

    void placeSlider(juce::Slider& slider, juce::Label& label, int x, int y, float scale=1.0f) {
        slider.setSize(knob_size * scale, knob_size * scale);
        slider.setTopLeftPosition(x, y);
    }

    void presetButtonClicked() {
        audioProcessor->saveFactoryPreset(factoryPresetNameLabel.getText());
    }

    void rmWaveAClicked() {
        audioProcessor->cycleRMWaveParamValue("ring_mod_waveA");
        rmWaveA.setButtonText(audioProcessor->get_rm_wave_type(0));
    }

    void rmWaveBClicked() {
        audioProcessor->cycleRMWaveParamValue("ring_mod_waveB");
        rmWaveB.setButtonText(audioProcessor->get_rm_wave_type(1));
    }

    void combModModeClicked() {
        audioProcessor->cycleCombModParamValue();
        combModModeButton.setButtonText(audioProcessor->get_comb_mod_mode_type());
    }

private:
    ChaoticGoodAudioProcessor* audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    GaloisLookAndFeel lookAndFeel;
    GaloisLookAndFeelCentred lookAndFeelCentred;

    // Controls
    juce::Slider noiseLevelSlider;
    juce::Label noiseLevelSliderLabel;
    std::unique_ptr<SliderAttachment> noiseLevelSliderAttachment;

    juce::Slider noiseDensitySlider;
    juce::Label noiseDensitySliderLabel;
    std::unique_ptr<SliderAttachment> noiseDensitySliderAttachment;

    juce::Slider noiseDensityVarSlider;
    juce::Label noiseDensityVarSliderLabel;
    std::unique_ptr<SliderAttachment> noiseDensityVarSliderAttachment;

    juce::Slider noiseFilterTypeSlider;
    juce::Label noiseFilterTypeSliderLabel;
    std::unique_ptr<SliderAttachment> noiseFilterTypeSliderAttachment;

    juce::Slider noiseFilterCutoffSlider;
    juce::Label noiseFilterCutoffSliderLabel;
    std::unique_ptr<SliderAttachment> noiseFilterCutoffSliderAttachment;

    juce::Slider MIDI_noiseFilterCutoffSlider;
    juce::Label MIDI_noiseFilterCutoffSliderLabel;
    std::unique_ptr<SliderAttachment> MIDI_noiseFilterCutoffSliderAttachment;

    juce::Slider noiseFilterResonanceSlider;
    juce::Label noiseFilterResonanceSliderLabel;
    std::unique_ptr<SliderAttachment> noiseFilterResonanceSliderAttachment;

    juce::Slider inputSlider;
    juce::Label inputSliderLabel;
    std::unique_ptr<SliderAttachment> inputSliderAttachment;

    juce::Slider outputSlider;
    juce::Label outputSliderLabel;
    std::unique_ptr<SliderAttachment> outputSliderAttachment;

    juce::Slider typeSlider;
    juce::Label typeSliderLabel;
    std::unique_ptr<SliderAttachment> typeSliderAttachment;

    juce::Slider powerSlider;
    juce::Label powerSliderLabel;
    std::unique_ptr<SliderAttachment> powerSliderAttachment;

    juce::Slider dryBlendSlider;
    juce::Label dryBlendSliderLabel;
    std::unique_ptr<SliderAttachment> dryBlendSliderAttachment;

    juce::Slider lowCutoffSlider;
    juce::Label lowCutoffSliderLabel;
    std::unique_ptr<SliderAttachment> lowCutoffSliderAttachment;

    juce::Slider midCutoffSlider;
    juce::Label midCutoffSliderLabel;
    std::unique_ptr<SliderAttachment> midCutoffSliderAttachment;

    juce::Slider highCutoffSlider;
    juce::Label highCutoffSliderLabel;
    std::unique_ptr<SliderAttachment> highCutoffSliderAttachment;

    juce::Slider rmFreqASlider;
    juce::Label rmFreqASliderLabel;
    std::unique_ptr<SliderAttachment> rmFreqASliderAttachment;

    juce::Slider MIDI_rmFreqASlider;
    juce::Label MIDI_rmFreqASliderLabel;
    std::unique_ptr<SliderAttachment> MIDI_rmFreqASliderAttachment;

    juce::Slider rmFreqBSlider;
    juce::Label rmFreqBSliderLabel;
    std::unique_ptr<SliderAttachment> rmFreqBSliderAttachment;

    juce::Slider MIDI_rmFreqBSlider;
    juce::Label MIDI_rmFreqBSliderLabel;
    std::unique_ptr<SliderAttachment> MIDI_rmFreqBSliderAttachment;

    juce::Slider rmAmountSlider;
    juce::Label rmAmountSliderLabel;
    std::unique_ptr<SliderAttachment> rmAmountSliderAttachment;

    juce::Slider rmFMAmountSlider;
    juce::Label rmFMAmountSliderLabel;
    std::unique_ptr<SliderAttachment> rmFMAmountSliderAttachment;

    juce::Slider rmXFadeSlider;
    juce::Label rmXFadeSliderLabel;
    std::unique_ptr<SliderAttachment> rmXFadeSliderAttachment;

    //  juce::Slider rmWaveSlider;
  //  juce::Label rmWaveSliderLabel;
  //  std::unique_ptr<SliderAttachment> rmWaveSliderAttachment;

    juce::TextButton rmWaveA;
    juce::TextButton rmWaveB;

    juce::TextButton combModModeButton;

    juce::Slider rmModeSlider;
    juce::Label rmModeSliderLabel;
    std::unique_ptr<SliderAttachment> rmModeSliderAttachment;

    juce::Slider flangerCentreSliderA;
    juce::Label flangerCentreSliderALabel;
    std::unique_ptr<SliderAttachment> flangerCentreSliderAAttachment;

    juce::Slider flangerCentreSliderB;
    juce::Label flangerCentreSliderBLabel;
    std::unique_ptr<SliderAttachment> flangerCentreSliderBAttachment;

    juce::Slider MIDI_flangerCentreSliderA;
    juce::Label MIDI_flangerCentreSliderALabel;
    std::unique_ptr<SliderAttachment> MIDI_flangerCentreSliderAAttachment;

    juce::Slider MIDI_flangerCentreSliderB;
    juce::Label MIDI_flangerCentreSliderBLabel;
    std::unique_ptr<SliderAttachment> MIDI_flangerCentreSliderBAttachment;

    juce::Slider flangerModeSlider;
    juce::Label flangerModeSliderLabel;
    std::unique_ptr<SliderAttachment> flangerModeSliderAttachment;

    juce::Slider flangerLFORateSlider;
    juce::Label flangerLFORateSliderLabel;
    std::unique_ptr<SliderAttachment> flangerLFORateSliderAttachment;

    juce::Slider flangerLFODepthSlider;
    juce::Label flangerLFODepthSliderLabel;
    std::unique_ptr<SliderAttachment> flangerLFODepthSliderAttachment;

    juce::Slider flangerAmountSlider;
    juce::Label flangerAmountSliderLabel;
    std::unique_ptr<SliderAttachment> flangerAmountSliderAttachment;

    juce::Slider flangerResonanceSlider;
    juce::Label flangerResonanceSliderLabel;
    std::unique_ptr<SliderAttachment> flangerResonanceSliderAttachment;

    juce::Slider flangerAMSlider;
    juce::Label flangerAMSliderLabel;
    std::unique_ptr<SliderAttachment> flangerAMSliderAttachment;

    juce::Slider jitterDepthSlider;
    juce::Label jitterDepthSliderLabel;
    std::unique_ptr<SliderAttachment> jitterDepthSliderAttachment;

    juce::Slider jitterDutyCycleSlider;
    juce::Label jitterDutyCycleSliderLabel;
    std::unique_ptr<SliderAttachment> jitterDutyCycleSliderAttachment;

    juce::Slider jitterFrequencySlider;
    juce::Label jitterFrequencySliderLabel;
    std::unique_ptr<SliderAttachment> jitterFrequencySliderAttachment;

    juce::Slider jitterRegularitySlider;
    juce::Label jitterRegularitySliderLabel;
    std::unique_ptr<SliderAttachment> jitterRegularitySliderAttachment;

    juce::Slider jitterSlewSlider;
    juce::Label jitterSlewSliderLabel;
    std::unique_ptr<SliderAttachment> jitterSlewSliderAttachment;

    juce::TextButton filterInputButton;
    std::unique_ptr<ButtonAttachment> filterInputButtonAttachment;

    juce::TextButton jitterLFOButton;
    std::unique_ptr<ButtonAttachment> jitterLFOButtonAttachment;

    juce::TextButton noiseMidiButton;
    std::unique_ptr<ButtonAttachment> noiseMidiButtonAttachment;
    juce::TextButton ringAMidiButton;
    std::unique_ptr<ButtonAttachment> ringAMidiButtonAttachment;
    juce::TextButton ringBMidiButton;
    std::unique_ptr<ButtonAttachment> ringBMidiButtonAttachment;
    juce::TextButton combAMidiButton;
    std::unique_ptr<ButtonAttachment> combAMidiButtonAttachment;
    juce::TextButton combBMidiButton;
    std::unique_ptr<ButtonAttachment> combBMidiButtonAttachment;

    juce::TextButton rmALFOButton;
    std::unique_ptr<ButtonAttachment> rmALFOButtonAttachment;
    juce::TextButton rmBLFOButton;
    std::unique_ptr<ButtonAttachment> rmBLFOButtonAttachment;

    int knob_size = 110;
    int knob_spacer = 7;

    juce::Label noiseSectionLabel;
    juce::Label ringModSectionLabel;
    juce::Label flangerSectionLabel;
    juce::Label ampSectionLabel;
    juce::Label inputSectionLabel;
    juce::Label toneSectionLabel;
    juce::Label outputSectionLabel;

    juce::TextButton factoryPresetButton;
    juce::Label factoryPresetNameLabel;

    juce::ImageComponent backgroundImage;
    juce::ImageComponent logoImage;
};