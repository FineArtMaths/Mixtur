/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChaoticGoodAudioProcessor::ChaoticGoodAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
#endif    
    , tree(*this, nullptr, "Chaotic_Good_Parameter_Tree", {
        std::make_unique<juce::AudioParameterChoice>("amplifier_structure", "Structure", juce::StringArray{
                                                                                    "Clean",
                                                                                    "Hard",
                                                                                    "Logistic",
                                                                                    "Arctan",
                                                                                    "Root",
                                                                                    "Half Vin",
                                                                                    "Tanh",
                                                                                    "Sine"
                                                                                }, 0),
        std::make_unique<juce::AudioParameterFloat>("amplifier_gain", "Gain", 0.0f, 1.0f, 0.15f),
        std::make_unique<juce::AudioParameterFloat>("power_positive", "Bias", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("output_level", "Output", 0.0f, 1.0f, 0.35f),
        std::make_unique<juce::AudioParameterFloat>("dry_blend", "Dry Blend", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("low_cutoff", "Low Cut", 0.0f, 9.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("mid_cutoff", "Mid Boost", 0.0f, 9.0f, 9.0f),
        std::make_unique<juce::AudioParameterFloat>("high_cutoff", "High Cut", 0.0f, 9.0f, 9.0f),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_freq_A", "Freq A", 0.1f, 16000.0f, 500.0f),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_freq_B", "Freq B", 0.1f, 16000.0f, 500.0f),
        std::make_unique<juce::AudioParameterBool>("ring_mod_low_A", "LFO A", false),
        std::make_unique<juce::AudioParameterBool>("ring_mod_low_B", "LFO B", false),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_amt", "Amount", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_fm_amt", "FM/AM Amount", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterChoice>("ring_mod_mode", "Mode", juce::StringArray {
                                                                                    "Single",
                                                                                    "Series",
                                                                                    "Parallel",
                                                                                    "FM B>A",
                                                                                    "AM B>A",
                                                                                    "FM A>B",
                                                                                    "AM A>B",
                                                                                    "FM Tone",
                                                                                    "AM Tone"
                                                                                }, 0
            ),
        std::make_unique<juce::AudioParameterInt>("ring_mod_waveA", "Ring Mod Wave A", 0, 3, 0),
        std::make_unique<juce::AudioParameterInt>("ring_mod_waveB", "Ring Mod Wave B", 0, 3, 0),
        std::make_unique<juce::AudioParameterFloat>("flanger_lfo_rate", "LFO Rate", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("flanger_lfo_depth", "LFO Depth", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("flanger_centre_position", "Frequency", 0.0f, 1000.0f, 500.0f),
        std::make_unique<juce::AudioParameterFloat>("flanger_amount", "Amount", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("flanger_resonance", "Resonance", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("flanger_AM", "AM", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_level", "Level", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_density", "Density", 0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_density_variation", "Variation", 0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_filter_cutoff", "Cutoff", 0.0f, 9.0f, 9.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_filter_resonance", "Resonance", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterChoice>("noise_filter_type", "Type", juce::StringArray{
                                                                                    "Lo Pass",
                                                                                    "Hi Pass",
                                                                                    "Band",
                                                                                    "Notch",
                                                                                    "Bell",
                                                                                    "L Shlf",
                                                                                    "H Shlf"
                                                                                }, 0),
        std::make_unique<juce::AudioParameterBool>("receive_midi_noise", "MIDI to Noise", false),
        std::make_unique<juce::AudioParameterBool>("receive_midi_ringA", "MIDI to Ring A", false),
        std::make_unique<juce::AudioParameterBool>("receive_midi_ringB", "MIDI to Ring B", false),
        std::make_unique<juce::AudioParameterBool>("receive_midi_comb", "MIDI to Comb", false),
        std::make_unique<juce::AudioParameterInt>("midi_noise_channel", "MIDI Noise Channel", 0, 16, 0),
        std::make_unique<juce::AudioParameterInt>("midi_ringA_channel", "MIDI Ring A Channel", 0, 16, 0),
        std::make_unique<juce::AudioParameterInt>("midi_ringB_channel", "MIDI Ring B Channel", 0, 16, 0),
        std::make_unique<juce::AudioParameterInt>("midi_comb_channel", "MIDI Comb Channel", 0, 16, 0),
        }) {
    tree.addParameterListener("amplifier_structure", this);
    tree.addParameterListener("amplifier_gain", this);
    tree.addParameterListener("power_positive", this);
    tree.addParameterListener("output_level", this);
    tree.addParameterListener("dry_blend", this);
    tree.addParameterListener("low_cutoff", this);
    tree.addParameterListener("mid_cutoff", this);
    tree.addParameterListener("high_cutoff", this);
    tree.addParameterListener("ring_mod_freq_A", this);
    tree.addParameterListener("ring_mod_freq_B", this);
    tree.addParameterListener("ring_mod_low_A", this);
    tree.addParameterListener("ring_mod_low_B", this);
    tree.addParameterListener("ring_mod_amt", this);
    tree.addParameterListener("ring_mod_fm_amt", this);
//    tree.addParameterListener("ring_mod_waveA", this);
//    tree.addParameterListener("ring_mod_waveB", this);
    tree.addParameterListener("ring_mod_mode", this);
    tree.addParameterListener("flanger_lfo_rate", this);
    tree.addParameterListener("flanger_lfo_depth", this);
    tree.addParameterListener("flanger_centre_position", this);
    tree.addParameterListener("flanger_amount", this);
    tree.addParameterListener("flanger_resonance", this);
    tree.addParameterListener("flanger_AM", this);
    tree.addParameterListener("noise_density", this);
    tree.addParameterListener("noise_density_variation", this);
    tree.addParameterListener("noise_level", this);
    tree.addParameterListener("noise_filter_cutoff", this);
    tree.addParameterListener("noise_filter_resonance", this);
    tree.addParameterListener("noise_filter_type", this);
    tree.addParameterListener("receive_midi_noise", this);
    tree.addParameterListener("receive_midi_ringA", this);
    tree.addParameterListener("receive_midi_ringB", this);
    tree.addParameterListener("receive_midi_comb", this);
    tree.addParameterListener("midi_noise_channel", this);
    tree.addParameterListener("midi_ringA_channel", this);
    tree.addParameterListener("midi_ringB_channel", this);
    tree.addParameterListener("midi_comb_channel", this);

    preset_names = new juce::String[NUM_PROGRAMMES];
    preset_filenames = new juce::String[NUM_PROGRAMMES];
    preset_names[0] = "Init";
    preset_filenames[0] = "preset_Init_xml";
    preset_names[1] = "Guitar amp";
    preset_filenames[1] = "preset_GuitarAmp_xml";
    preset_names[2] = "Two-Transistor Fuzz";
    preset_filenames[2] = "preset_FuzzFace_xml";
    preset_names[3] = "Germanium Treble Boost";
    preset_filenames[3] = "preset_Germanium_xml";
    preset_names[4] = "Double Tremolo";
    preset_filenames[4] = "preset_DoubleTremolo_xml";
    preset_names[5] = "Ring modulator with amplitude modulation";
    preset_filenames[5] = "preset_AMRingMod_xml";
    preset_names[6] = "Classic ring modulator (tuned to A)";
    preset_filenames[6] = "preset_ClassicRingModTunedA_xml";
    preset_names[7] = "Jagged glitter";
    preset_filenames[7] = "preset_RMJaggedGlitter_xml";
    preset_names[8] = "Inharmonic banjo";
    preset_filenames[8] = "preset_InharmonicBanjo_xml";
    preset_names[9] = "Breathing feedback";
    preset_filenames[9] = "preset_BreathingFeedback_xml";
    preset_names[10] = "The guitar is a gong";
    preset_filenames[10] = "preset_GuitarIsAGong_xml";
    preset_names[11] = "Surging comb";
    preset_filenames[11] = "preset_SurgingComb_xml";
    preset_names[12] = "Tremolo and jet planes";
    preset_filenames[12] = "preset_TremoloAndJetPlanes_xml";

    parameterChanged("", 0);    // Dummy call to cause all parameters to be cached
}

ChaoticGoodAudioProcessor::~ChaoticGoodAudioProcessor()
{
}

//==============================================================================
const juce::String ChaoticGoodAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChaoticGoodAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChaoticGoodAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChaoticGoodAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChaoticGoodAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChaoticGoodAudioProcessor::getNumPrograms()
{
    return NUM_PROGRAMMES;
}

int ChaoticGoodAudioProcessor::getCurrentProgram()
{
    return current_programme;
}

void ChaoticGoodAudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= NUM_PROGRAMMES) {
        return;
    }
    current_programme = index;
    int dataSize = 0;
    const char* xml = BinaryData::getNamedResource(preset_filenames[index].toRawUTF8(), dataSize);
    std::unique_ptr<juce::XmlElement> xmlState = juce::XmlDocument(xml).getDocumentElement();

    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(tree.state.getType())) {
            tree.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

const juce::String ChaoticGoodAudioProcessor::getProgramName (int index)
{
    if (index < 0 || index >= NUM_PROGRAMMES) {
        return "ERROR in value of index";
    }
    return preset_names[index];
    return juce::String("");
}

void ChaoticGoodAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ChaoticGoodAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    host_sample_rate = sampleRate;
    num_channels = getNumInputChannels();

    cached_midi_channel_noise = 0;
    cached_midi_channel_ringA = 0;
    cached_midi_channel_ringB = 0;
    cached_midi_channel_comb = 0;

    ring_mod_angle_A = new double[num_channels];
    ring_mod_angle_B = new double[num_channels];
    for (int i = 0; i < num_channels; ++i) {
        ring_mod_angle_A[i] = 0.0f;
        ring_mod_angle_B[i] = 0.0f;
    }

    low_filter = new Biquad[num_channels];
    mid_filter = new Biquad[num_channels];
    high_filter = new Biquad[num_channels];
    noise_filter = new Biquad[num_channels];
    parameterChanged("", 0);
    updateLowFilter();
    updateMidFilter();
    updateHighFilter();
    updateNoiseFilter();

    delay_line_length = sampleRate;
    delay_line = new float* [num_channels];
    delay_line_write_idx = new int[num_channels];
    delay_line_read_idx = new int[num_channels];
    delay_line_read_excess = new float[num_channels];
    lfo_angle = new double[num_channels];

    clearDelayLines();

    centre_modifier = (float)(delay_line_length) / 60000.0f;
}

void ChaoticGoodAudioProcessor::clearDelayLines() {
    for (int j = 0; j < num_channels; ++j) {
        delay_line_write_idx[j] = 0;
        delay_line_read_idx[j] = 0;
        delay_line_read_excess[j] = 0.0f;
        delay_line[j] = new float[delay_line_length];
        for (int i = 0; i < delay_line_length; ++i) {
            delay_line[j][i] = 0.0f;
        }
        lfo_angle[j] = 0.0f;
    }
}

/*
* angle varies from 0 to 2PI
*/
float ChaoticGoodAudioProcessor::get_lfo_value(float angle) {
        return sin(angle);
}

void ChaoticGoodAudioProcessor::advance_delay_line_read_head(int channel) {
    int centre = cached_centre_position;
    delay_line_read_idx[channel] = delay_line_write_idx[channel] - centre;
    float mod_value = 0.0f;
    if (cached_comb_mod_mode == COMB_MOD_MODE_FM_A) {
        mod_value = apply_wave(ring_mod_angle_A[channel], cached_ring_mod_waveA);
    } else if (cached_comb_mod_mode == COMB_MOD_MODE_FM_B) {
            mod_value = apply_wave(ring_mod_angle_B[channel], cached_ring_mod_waveB);
     } else if (cached_comb_mod_mode == COMB_MOD_MODE_NOISE) {
        mod_value = last_noise_sample;
    }
    else {
        mod_value = 1.0f;
    }

    float newval = mod_value * cached_flanger_FM;
    delay_line_read_idx[channel] += trunc(newval);
    delay_line_read_excess[channel] = newval - floor(newval);
    while (delay_line_read_idx[channel] >= delay_line_length) {
        delay_line_read_idx[channel] -= delay_line_length;
    }
    while (delay_line_read_idx[channel] < 0) {
        delay_line_read_idx[channel] += delay_line_length;
    }
}

void ChaoticGoodAudioProcessor::advance_delay_line_write_head(int amount, int channel) {
    delay_line_write_idx[channel] += amount;
    if (delay_line_write_idx[channel] >= delay_line_length) {
        delay_line_write_idx[channel] -= delay_line_length;
    }
    else if (delay_line_write_idx[channel] < 0) {
        delay_line_write_idx[channel] = delay_line_length - delay_line_write_idx[channel];
    }
}


void ChaoticGoodAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChaoticGoodAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ChaoticGoodAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) {

    // MIDI
    cached_midi_noise = *tree.getRawParameterValue("receive_midi_noise");
    cached_midi_ringA = *tree.getRawParameterValue("receive_midi_ringA");
    cached_midi_ringB = *tree.getRawParameterValue("receive_midi_ringB");
    cached_midi_comb = *tree.getRawParameterValue("receive_midi_comb");
    cached_midi_channel_noise = *tree.getRawParameterValue("midi_noise_channel");
    cached_midi_channel_ringA = *tree.getRawParameterValue("midi_ringA_channel");
    cached_midi_channel_ringB = *tree.getRawParameterValue("midi_ringB_channel");
    cached_midi_channel_comb = *tree.getRawParameterValue("midi_comb_channel");

    // EQ
    cached_low_filter_cutoff = 16 * pow(2, *tree.getRawParameterValue("low_cutoff"));
    if (cached_low_filter_cutoff >= host_sample_rate / 2) {
        cached_low_filter_cutoff = host_sample_rate / 2 - 1;
    }
    updateLowFilter();

    cached_mid_filter_cutoff = 16 * pow(2, *tree.getRawParameterValue("mid_cutoff"));
    if (cached_mid_filter_cutoff >= host_sample_rate / 2) {
        cached_mid_filter_cutoff = host_sample_rate / 2 - 1;
    }
    updateMidFilter();

    cached_high_filter_cutoff = 16 * pow(2, *tree.getRawParameterValue("high_cutoff"));
    if (cached_high_filter_cutoff >= host_sample_rate / 2) {
        cached_high_filter_cutoff = host_sample_rate / 2 - 1;
    }
    updateHighFilter();

    if (!cached_midi_noise) {
        cached_noise_filter_cutoff = 16 * pow(2, *tree.getRawParameterValue("noise_filter_cutoff"));
        if (cached_noise_filter_cutoff >= host_sample_rate / 2) {
            cached_noise_filter_cutoff = host_sample_rate / 2 - 1;
        }
    }
    cached_noise_filter_type = (int)*tree.getRawParameterValue("noise_filter_type");
    cached_noise_filter_resonance = 1.0 - *tree.getRawParameterValue("noise_filter_resonance");
    updateNoiseFilter();

    // Ring mod
    cached_ring_mod_mode = (int) * tree.getRawParameterValue("ring_mod_mode");
    cached_ring_mod_fm_amt = *tree.getRawParameterValue("ring_mod_fm_amt");
    if (!cached_midi_ringA) {
        cached_ring_mod_freq_A = *tree.getRawParameterValue("ring_mod_freq_A");
    }
    if (!cached_midi_ringB) {
        cached_ring_mod_freq_B = *tree.getRawParameterValue("ring_mod_freq_B");
    }
    cached_ring_mod_low_A = *tree.getRawParameterValue("ring_mod_low_A");
    if (cached_ring_mod_low_A) {
        cached_ring_mod_freq_A /= 4000;
    }
    cached_ring_mod_low_B = *tree.getRawParameterValue("ring_mod_low_B");
    if (cached_ring_mod_low_B) {
        cached_ring_mod_freq_B /= 400;
    }

    cached_ring_mod_amt = *tree.getRawParameterValue("ring_mod_amt");
//    cached_ring_mod_waveA = *tree.getRawParameterValue("ring_mod_waveA");
//    cached_ring_mod_waveB = *tree.getRawParameterValue("ring_mod_waveB");

    // Flanger
    cached_flanger_amount = *tree.getRawParameterValue("flanger_amount");
    
    if (!cached_midi_comb) {
        float centre_position = tree.getParameterRange("flanger_centre_position").end;
        centre_position -= *tree.getRawParameterValue("flanger_centre_position");
        centre_position *= centre_modifier;
        if (centre_position < 0) {
            centre_position += 1;
        }
        cached_centre_position = floor(centre_position);
    }
    cached_lfo_depth = *tree.getRawParameterValue("flanger_lfo_depth");
    cached_lfo_depth *= fmin(cached_centre_position, delay_line_length - cached_centre_position);
    cached_lfo_depth /= 10;

    cached_lfo_rate = *tree.getRawParameterValue("flanger_lfo_rate") * M_TWO_PI;
    cached_lfo_rate *= 2;
    cached_lfo_rate /= host_sample_rate;

    cached_resonance = *tree.getRawParameterValue("flanger_resonance");
    cached_resonance /= 2.0f;
    cached_resonance += 1.5f;

    cached_flanger_AM = *tree.getRawParameterValue("flanger_AM");
    cached_flanger_FM = cached_flanger_AM * fmin(cached_centre_position, delay_line_length - cached_centre_position);
    cached_flanger_FM /= 10;

    // Noise
    cached_noise_level = *tree.getRawParameterValue("noise_level");
    cached_noise_density = floor(pow(*tree.getRawParameterValue("noise_density"), 2) * RAND_MAX / 10);
    cached_noise_resolution = 1 + pow(floor(* tree.getRawParameterValue("noise_density_variation") * 10), 2);

    // Misc parameters
    cached_input_level_positive = *tree.getRawParameterValue("amplifier_gain");
    cached_input_level_positive *= MAX_GAIN;

    cached_power_positive = *tree.getRawParameterValue("power_positive");
    cached_power_positive *= MAX_GATE;
    cached_power_positive += 1;

    cached_type_positive = (int) * tree.getRawParameterValue("amplifier_structure");

    cached_output_level = *tree.getRawParameterValue("output_level");
    cached_output_level *= 2.67f;
    cached_output_level = pow(cached_output_level, 3);
    cached_output_level *= MAX_OUTPUT;
    cached_dry_blend = *tree.getRawParameterValue("dry_blend");
}

void ChaoticGoodAudioProcessor::updateLowFilter() {
    for (int i = 0; i < num_channels; ++i) {
        low_filter[i].recalculate(
            host_sample_rate,
            cached_low_filter_cutoff,
            1.0f,
            cached_low_filter_gain,
            LSH
        );
    }
}

void ChaoticGoodAudioProcessor::updateMidFilter() {
    for (int i = 0; i < num_channels; ++i) {
        mid_filter[i].recalculate(
            host_sample_rate,
            cached_mid_filter_cutoff,
            1.0f,
            cached_mid_filter_gain,
            PEQ
        );
    }
}

void ChaoticGoodAudioProcessor::updateHighFilter() {
    for (int i = 0; i < num_channels; ++i) {
        high_filter[i].recalculate(
            host_sample_rate,
            cached_high_filter_cutoff,
            1.0f,
            cached_high_filter_gain,
            HSH
        );
    }
}

void ChaoticGoodAudioProcessor::updateNoiseFilter() {
    for (int i = 0; i < num_channels; ++i) {
        noise_filter[i].recalculate(
            host_sample_rate,
            cached_noise_filter_cutoff,
            cached_noise_filter_resonance,
            cached_noise_filter_gain,
            cached_noise_filter_type
        );
    }
}

float ChaoticGoodAudioProcessor::clamp(float val, float min, float max) {
    if (val > max) {
        return max;
    }
    if (val < min) {
        return min;
    }
    return val;
}

int ChaoticGoodAudioProcessor::sgn(double v) {
    if (v < 0) return -1;
    if (v > 0) return 1;
    return 0;
}

float ChaoticGoodAudioProcessor::fold(float sample) {
    if (sample > 0) {
        if (sample > 2) {
            sample -= floor(sample - 1);
        }
        if (sample > 1) {
            sample = 1 - (sample - 1);
        }
    }
    else if (sample < 0) {
        if (sample < -2) {
            sample += floor(abs(sample) - 1);
        }
        if (sample < -1) {
            sample = -1 - (sample + 1);
        }
    }
    return sample * 0.95;
}

float ChaoticGoodAudioProcessor::apply_noise(float sample, int channel) {
    if (rand() < cached_noise_density) {
        last_noise_sample = ((float)(rand() % cached_noise_resolution) * (F_RAND_MAX / cached_noise_resolution) / (F_HALF_RAND_MAX)) - 1;
    }
    else {
        last_noise_sample = 0.0f;
    }
    last_noise_sample = noise_filter[channel].apply(last_noise_sample);
    return last_noise_sample;
}

float ChaoticGoodAudioProcessor::apply_wave(float angle, int waveform) {
    while (angle < 0) {
        angle += M_TWO_PI;
    }
    while (angle > M_TWO_PI) {
        angle -= M_TWO_PI;
    }
    switch (waveform) {
    case waves::WAVE_SINE:
        return sin(angle);
    case waves::WAVE_SQUARE:
        return waves::square_wave(angle);
    case waves::WAVE_RAMP:
        return waves::ramp_wave(angle);
    case waves::WAVE_TRIANGLE:
        return waves::triangle_wave(angle);
    }
}


float ChaoticGoodAudioProcessor::apply_ring_mod(float sample, int channel) {
    // Ring mod
    float rm_B = apply_wave(ring_mod_angle_B[channel], cached_ring_mod_waveB);
    float rm_A = 0.0f;

    switch (cached_ring_mod_mode) {
    case RM_MODE_SINGLE:
        rm_A = apply_wave(ring_mod_angle_A[channel], cached_ring_mod_waveA);
        break;
    case RM_MODE_SERIES:
        rm_A = apply_wave(ring_mod_angle_A[channel], cached_ring_mod_waveA);
        rm_A *= rm_B;
        break;
    case RM_MODE_PARALLEL:
        rm_A = apply_wave(ring_mod_angle_A[channel], cached_ring_mod_waveA);
        break;
    case RM_MODE_FM:
        rm_A = apply_wave(
            ring_mod_angle_A[channel] * (1 - cached_ring_mod_fm_amt)
            + ring_mod_angle_A[channel] * ((1 + rm_B)) * cached_ring_mod_fm_amt,
            cached_ring_mod_waveA
        );
        break;
    case RM_MODE_AM:
        rm_A = apply_wave(ring_mod_angle_A[channel], cached_ring_mod_waveA) * ((1 + rm_B) / 2) * cached_ring_mod_fm_amt;
        rm_A += apply_wave(ring_mod_angle_A[channel], cached_ring_mod_waveA) * (1 - cached_ring_mod_fm_amt);
        break;
    case RM_MODE_FM_2:
        rm_B = apply_wave(ring_mod_angle_A[channel], cached_ring_mod_waveA);
        rm_A = apply_wave(
            ring_mod_angle_B[channel] * (1 - cached_ring_mod_fm_amt)
            + ring_mod_angle_B[channel] * ((1 + rm_B)) * cached_ring_mod_fm_amt,
            cached_ring_mod_waveB
        );
        break;
    case RM_MODE_AM_2:
        rm_B = apply_wave(ring_mod_angle_A[channel], cached_ring_mod_waveA);
        rm_A = apply_wave(ring_mod_angle_B[channel], cached_ring_mod_waveB) * ((1 + rm_B) / 2) * cached_ring_mod_fm_amt;
        rm_A += apply_wave(ring_mod_angle_B[channel], cached_ring_mod_waveB) * (1 - cached_ring_mod_fm_amt);
        break;
    case RM_MODE_SYNTH_FM:
        rm_A = apply_wave(
            ring_mod_angle_A[channel] * (1 - cached_ring_mod_fm_amt)
            + ring_mod_angle_A[channel] * ((1 + rm_B) / 2) * cached_ring_mod_fm_amt,
            cached_ring_mod_waveA
        );
        break;
    case RM_MODE_SYNTH_AM:
        rm_A = apply_wave(
            ring_mod_angle_A[channel], cached_ring_mod_waveA
        ) * ((1 + rm_B) / 2) * cached_ring_mod_fm_amt 
            + apply_wave(
                ring_mod_angle_A[channel], cached_ring_mod_waveA
        ) * (1 - cached_ring_mod_fm_amt);
        break;
    }
    if (cached_ring_mod_freq_A == 0.0f) {
        rm_A = 0.0f;
    }
    if (cached_ring_mod_mode == RM_MODE_PARALLEL) {
        sample = sample * (1 - cached_ring_mod_amt)
            + (sample * rm_A + sample * rm_B) * cached_ring_mod_amt / 2;
    }
    else if (cached_ring_mod_mode == RM_MODE_SYNTH_FM || cached_ring_mod_mode == RM_MODE_SYNTH_AM) {
        sample = rm_A;
    }
    else {
        sample = sample * (1 - cached_ring_mod_amt)
            + (sample * rm_A) * cached_ring_mod_amt;
    }
    return sample;
}

float ChaoticGoodAudioProcessor::apply_clip(float sample, int channel) {

    sample *= cached_input_level_positive;

    switch (cached_type_positive) {
    case CLIP_TYPE_NONE:
        sample = clamp(sample, -2, 2);
        break;
    case CLIP_TYPE_HARD:
        sample = clamp(sample, -1, 1);
        break;
    case CLIP_TYPE_TANH:
        sample = tanh(sample);
        break;
    case CLIP_TYPE_SINE:
        sample = sin(sample * M_HALF_PI);
        break;
    case CLIP_TYPE_LOGISTIC:
        sample = 2 * (1 / (1 + exp(-1 * sample))) - 1;
        sample *= 1.1f;
        break;
    case CLIP_TYPE_ATAN:
        sample = atan(sample) / M_HALF_PI;
        break;
    case CLIP_TYPE_ROOT:
        sample = sample / sqrt(1 + sample * sample);
    case CLIP_TYPE_VIN_HALF_GAIN:
        sample = sample / (abs(sample) + 0.5);
    }
    return sample;
}

float ChaoticGoodAudioProcessor::process(float sample, int channel) {

    float dry = sample;

    sample = apply_ring_mod(sample, channel);

    sample = mid_filter[channel].apply(sample);

    sample = apply_flange(sample, channel) * cached_flanger_amount + sample * (1 - cached_flanger_amount);

    sample = apply_clip(sample, channel);

    sample = low_filter[channel].apply(sample);

    // Bias
    sample = sgn(sample) * pow(abs(sample), cached_power_positive);

    sample = high_filter[channel].apply(sample);

    // Output Level             
    sample *= cached_output_level;

    // Clamp to valid range
    //sample = clamp(sample, -1, 1);
    sample = tanh(sample);

    sample = dry * cached_dry_blend + sample * (1 - cached_dry_blend);

    return sample;
}

float ChaoticGoodAudioProcessor::read_comb_delay_line(int channel) {
    int i = delay_line_read_idx[channel];
    int i_next = i + 1;
    if (i_next >= delay_line_length) {
        i_next -= delay_line_length;
    }
    float low_result = delay_line[channel][i];
    float high_result = delay_line[channel][i_next];
    float result = low_result + (high_result - low_result) * delay_line_read_excess[channel];
    return result;
}

float ChaoticGoodAudioProcessor::apply_flange(float sample, int channel) {
    lfo_angle[channel] += cached_lfo_rate;
    if (lfo_angle[channel] > M_TWO_PI) {
        lfo_angle[channel] -= M_TWO_PI;
    }
    advance_delay_line_write_head(1, channel);
    advance_delay_line_read_head(channel);
    float result = read_comb_delay_line(channel);

    result = (result + sample)/2;
    delay_line[channel][delay_line_write_idx[channel]] = cached_resonance * result;
    if (cached_comb_mod_mode == COMB_MOD_MODE_AM_A) {
        float tmp = delay_line[channel][delay_line_write_idx[channel]] 
            * apply_wave(ring_mod_angle_A[channel], cached_ring_mod_waveA) * 1.1f;
        delay_line[channel][delay_line_write_idx[channel]] = 
            tmp * cached_flanger_AM 
            + delay_line[channel][delay_line_write_idx[channel]] * (1 - cached_flanger_AM);
    } else if (cached_comb_mod_mode == COMB_MOD_MODE_AM_B) {
        float tmp = delay_line[channel][delay_line_write_idx[channel]]
            * apply_wave(ring_mod_angle_B[channel], cached_ring_mod_waveB) * 1.1f;
        delay_line[channel][delay_line_write_idx[channel]] =
            tmp * cached_flanger_AM
            + delay_line[channel][delay_line_write_idx[channel]] * (1 - cached_flanger_AM);
    }

    return result;
}

void ChaoticGoodAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    float* channel;
    for (auto j = 0; j < buffer.getNumSamples(); ++j) {
        float raw[] = {0.0f, 0.0f};
        float cooked[] = { 0.0f, 0.0f };
        for (auto i = 0; i < 2; ++i) {
            ring_mod_angle_A[i] += cached_ring_mod_freq_A / host_sample_rate;
            if (ring_mod_angle_A[i] >= M_TWO_PI) {
                ring_mod_angle_A[i] -= M_TWO_PI;
            }
            ring_mod_angle_B[i] += cached_ring_mod_freq_B / host_sample_rate;
            if (ring_mod_angle_B[i] >= M_TWO_PI) {
                ring_mod_angle_B[i] -= M_TWO_PI;
            }
            channel = buffer.getWritePointer(i);
            float sample = channel[j] * (1 - cached_noise_level) + cached_noise_level * apply_noise(channel[j], i);
            channel[j] = process(sample, i);
        }
    }
    juce::MidiMessage m;
    juce::MidiBuffer::Iterator it{ midiMessages };
    int sample;

    while (it.getNextEvent(m, sample))
    {
        if (m.isNoteOn()) {
            int c = m.getChannel();
            float f = m.getMidiNoteInHertz(m.getNoteNumber());
            if (cached_midi_noise) {
                if (cached_midi_channel_noise == 0 || c == cached_midi_channel_noise) {
                    midi_note_is_playing_noise = true;
                    midi_note_number_noise = m.getNoteNumber();
                    cached_noise_filter_cutoff = f;
                    updateNoiseFilter();
                }
            }
            if (cached_midi_ringA) {
                if (cached_midi_channel_ringA == 0 || c == cached_midi_channel_ringA) {
                    midi_note_is_playing_ringA = true;
                    midi_note_number_ringA = m.getNoteNumber();
                    cached_ring_mod_freq_A = f;
                }
            }
            if (cached_midi_ringB) {
                if (cached_midi_channel_ringB == 0 || c == cached_midi_channel_ringB) {
                    midi_note_is_playing_ringB = true;
                    midi_note_number_ringB = m.getNoteNumber();
                    cached_ring_mod_freq_B = f;
                }
            }
            if (cached_midi_comb) {
                if (cached_midi_channel_comb == 0 || c == cached_midi_channel_comb) {
                    midi_note_is_playing_comb = true;
                    midi_note_number_comb = m.getNoteNumber();
                    if (f > 0) {
                        cached_centre_position = host_sample_rate / f;
                    }
                }
            }
        }
        else if (m.isNoteOff()) {
            int c = m.getChannel();
            if (cached_midi_channel_noise == 0 || c == cached_midi_channel_noise) {
                if (m.getNoteNumber() == midi_note_number_noise) {
                    midi_note_is_playing_noise = false;
                }
            }
            if (cached_midi_channel_ringA == 0 || c == cached_midi_channel_ringA) {
                if (m.getNoteNumber() == midi_note_number_ringA) {
                    if (cached_ring_mod_mode != RM_MODE_SYNTH_AM
                        && cached_ring_mod_mode != RM_MODE_SYNTH_FM
                        ) {
                        midi_note_is_playing_ringA = true;
                    }
                    else {
                        midi_note_is_playing_ringA = false;
                    }
                }
            }
            if (cached_midi_channel_ringB == 0 || c == cached_midi_channel_ringB) {
                if (m.getNoteNumber() == midi_note_number_ringB) {
                    if (cached_ring_mod_mode != RM_MODE_SYNTH_AM
                        && cached_ring_mod_mode != RM_MODE_SYNTH_FM
                        ) {
                        midi_note_is_playing_ringB = false;
                    }
                    else {
                        midi_note_is_playing_ringB = false;
                    }
                }
            }
            if (cached_midi_channel_comb == 0 || c == cached_midi_channel_comb) {
                if (m.getNoteNumber() == midi_note_number_comb) {
                    midi_note_is_playing_comb = false;
                }
            }
        }
    }
}

//==============================================================================
bool ChaoticGoodAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChaoticGoodAudioProcessor::createEditor()
{
    return new ChaoticGoodAudioProcessorEditor (*this, tree);
}

//==============================================================================
void ChaoticGoodAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = tree.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ChaoticGoodAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(tree.state.getType())) {
            tree.replaceState(juce::ValueTree::fromXml(*xmlState));
            current_programme = -1;
        }
    }
}

void ChaoticGoodAudioProcessor::saveFactoryPreset(juce::String name) {
    juce::ValueTree t = tree.copyState();
    std::unique_ptr<juce::XmlElement> xml = t.createXml();
    juce::File dir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory);
    juce::String fname = dir.getFullPathName();
    fname.append("\\preset_", 9);
    fname.append(name, 99);
    fname.append(".xml", 4);
    juce::File f = juce::File(fname);
    xml->writeTo(f);
}

juce::String ChaoticGoodAudioProcessor::get_rm_wave_type(int which)
{
    int wave = which == 0 ? cached_ring_mod_waveA : cached_ring_mod_waveB;
    switch (wave){
    case waves::WAVE_SINE:
        return juce::String("SINE");
    case waves::WAVE_TRIANGLE:
        return juce::String("TRIANGLE");
    case waves::WAVE_SQUARE:
        return juce::String("SQUARE");
    case waves::WAVE_RAMP:
        return juce::String("RAMP");
    }
    return juce::String("ERROR");
}

void ChaoticGoodAudioProcessor::cycleRMWaveParamValue(juce::String parameterID) {
    int current = 0;
    if (parameterID == "ring_mod_waveA") {
        current = cached_ring_mod_waveA;
    }
    else if (parameterID == "ring_mod_waveB") {
        current = cached_ring_mod_waveB;
    }
    ++current;
    if (current > 3) {
        current = 0;
    }
    if (parameterID == "ring_mod_waveA") {
        cached_ring_mod_waveA = current;
    } else if (parameterID == "ring_mod_waveB") {
        cached_ring_mod_waveB = current;
    }
}

juce::String ChaoticGoodAudioProcessor::get_comb_mod_mode_type()
{
    switch (cached_comb_mod_mode) {
    case COMB_MOD_MODE_AM_A:
        return juce::String("AM A");
    case COMB_MOD_MODE_AM_B:
        return juce::String("AM B");
    case COMB_MOD_MODE_FM_A:
        return juce::String("FM A");
    case COMB_MOD_MODE_FM_B:
        return juce::String("FM B");
    case COMB_MOD_MODE_NOISE:
        return juce::String("NOISE");
    }
    return juce::String("ERROR");
}
void ChaoticGoodAudioProcessor::cycleCombModParamValue() {
    int current = cached_comb_mod_mode;
    ++current;
    if (current > 4) {
        current = 0;
    }
    cached_comb_mod_mode = current;
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChaoticGoodAudioProcessor();
}
