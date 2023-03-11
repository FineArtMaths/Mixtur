/*
  ==============================================================================

    We are COMPLETE for features, GUI elements and general GUI layout.

    TODO LIST

    * Make the GUI look pretty
        *   Consider achieving this with just a background image and some tweaks to the colours
        *       It's worth at least TRYING to use an image for a knob though -- what if it's easy??
        *   Maybe make little diagrams for the ring mod showing what it's doing
        *       This is fiddly but would look cute
    * Add a popup for MIDI configuration
        *   Should be able to set the MIDI channels and pitch bend range, maybe more options
        *   Could even allow the user to customize the sequence of the effects.
        *       Have the five sections in a column with up/down arrows on each
        *       This would be fiddly but not very hard and might be really great.
        *       Can we also then reflect the signal path in the GUI layout?
        *       Maybe this is something for v2 as it seems like it would take quite a lot of work
        *   Consider going back to the "MIDI note off" behaviours as well.
        *       This would take a bit of coding but might be worth it.
    * Presets
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
        std::make_unique<juce::AudioParameterFloat>("amp_blend", "Blend", 0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("low_cutoff", "Low Shelf", 0.0f, 9.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("mid_cutoff", "Mid Hump", 0.0f, 9.0f, 9.0f),
        std::make_unique<juce::AudioParameterFloat>("high_cutoff", "High Shelf", 0.0f, 9.0f, 9.0f),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_freq_A", "Freq A", 0.1f, 16000.0f, 500.0f),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_freq_B", "Freq B", 0.1f, 16000.0f, 500.0f),
        std::make_unique<juce::AudioParameterBool>("ring_mod_low_A", "LFO A", false),
        std::make_unique<juce::AudioParameterBool>("ring_mod_low_B", "LFO B", false),
        std::make_unique<juce::AudioParameterBool>("receive_midi_ringA", "MIDI to Ring A", false),
        std::make_unique<juce::AudioParameterBool>("receive_midi_ringB", "MIDI to Ring B", false),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_freq_A_MIDI", "Freq A", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_freq_B_MIDI", "Freq A", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_amt", "Blend", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_xfade", "XFade", -1.0f, 0.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("ring_mod_fm_amt", "XMod Amount", 0.0f, 1.0f, 0.0f),
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
        std::make_unique<juce::AudioParameterFloat>("flanger_centre_position_A", "Freq A", 0.0f, 1000.0f, 500.0f),
        std::make_unique<juce::AudioParameterFloat>("flanger_centre_position_MIDI_A", "Freq A", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("flanger_centre_position_B", "Freq B", 0.0f, 1000.0f, 500.0f),
        std::make_unique<juce::AudioParameterFloat>("flanger_centre_position_MIDI_B", "Freq B", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("flanger_amount", "Blend", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("flanger_resonance", "Resonance", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("flanger_AM", "XMod Amount", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterChoice>("comb_mode", "Mode", juce::StringArray {
                                                                                    "Single",
                                                                                    "Series",
                                                                                    "Parallel",
                                                                                    "Multiply"
                                                                                }, 0
            ),
        std::make_unique<juce::AudioParameterFloat>("noise_level", "Blend", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_density", "Density", 0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_density_variation", "Variation", 0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_filter_cutoff", "Cutoff", 0.0f, 9.0f, 9.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_filter_cutoff_MIDI", "Cutoff", -1.0f, 1.0f, 0.0f),
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
        std::make_unique<juce::AudioParameterBool>("receive_midi_comb_A", "MIDI to Comb", false),
        std::make_unique<juce::AudioParameterBool>("receive_midi_comb_B", "MIDI to Comb", false),
        std::make_unique<juce::AudioParameterInt>("midi_noise_channel", "MIDI Noise Channel", 0, 16, 0),
        std::make_unique<juce::AudioParameterInt>("midi_ringA_channel", "MIDI Ring A Channel", 0, 16, 0),
        std::make_unique<juce::AudioParameterInt>("midi_ringB_channel", "MIDI Ring B Channel", 0, 16, 0),
        std::make_unique<juce::AudioParameterInt>("midi_comb_channel_A", "MIDI Comb A Channel", 0, 16, 0),
        std::make_unique<juce::AudioParameterInt>("midi_comb_channel_B", "MIDI Comb B Channel", 0, 16, 0),
        std::make_unique<juce::AudioParameterFloat>("jitter_depth", "Blend", 0.0, 1.0, 0.0),
        std::make_unique<juce::AudioParameterFloat>("jitter_frequency", "Frequency", 0.0, 1.0, 0.0),
        std::make_unique<juce::AudioParameterFloat>("jitter_regularity", "Regularity", 0.0, 1.0, 0.0),
        std::make_unique<juce::AudioParameterFloat>("jitter_duty_cycle", "Duty Cycle", -1.0, 1.0, 0.0),
        std::make_unique<juce::AudioParameterBool>("filter_input", "Filter Input", false),
        std::make_unique<juce::AudioParameterBool>("jitter_low", "Jitter LFO", true),
        std::make_unique<juce::AudioParameterFloat>("jitter_slew", "Slew", 0.0f, 1.0f, 0.1f),
        }) {
    tree.addParameterListener("amplifier_structure", this);
    tree.addParameterListener("amplifier_gain", this);
    tree.addParameterListener("power_positive", this);
    tree.addParameterListener("output_level", this);
    tree.addParameterListener("amp_blend", this);
    tree.addParameterListener("low_cutoff", this);
    tree.addParameterListener("mid_cutoff", this);
    tree.addParameterListener("high_cutoff", this);
    tree.addParameterListener("ring_mod_freq_A", this);
    tree.addParameterListener("ring_mod_freq_B", this);
    tree.addParameterListener("ring_mod_low_A", this);
    tree.addParameterListener("ring_mod_low_B", this);
    tree.addParameterListener("ring_mod_amt", this);
    tree.addParameterListener("ring_mod_fm_amt", this);
    tree.addParameterListener("ring_mod_xfade", this);
    tree.addParameterListener("ring_mod_mode", this);
    tree.addParameterListener("flanger_lfo_rate", this);
    tree.addParameterListener("flanger_lfo_depth", this);
    tree.addParameterListener("flanger_centre_position_A", this);
    tree.addParameterListener("flanger_centre_position_B", this);
    tree.addParameterListener("flanger_amount", this);
    tree.addParameterListener("flanger_resonance", this);
    tree.addParameterListener("flanger_AM", this);
    tree.addParameterListener("comb_mode", this);
    tree.addParameterListener("noise_density", this);
    tree.addParameterListener("noise_density_variation", this);
    tree.addParameterListener("noise_level", this);
    tree.addParameterListener("noise_filter_cutoff", this);
    tree.addParameterListener("noise_filter_resonance", this);
    tree.addParameterListener("noise_filter_type", this);
    tree.addParameterListener("receive_midi_noise", this);
    tree.addParameterListener("receive_midi_ringA", this);
    tree.addParameterListener("receive_midi_ringB", this);
    tree.addParameterListener("receive_midi_comb_A", this);
    tree.addParameterListener("receive_midi_comb_B", this);
    tree.addParameterListener("midi_noise_channel", this);
    tree.addParameterListener("midi_ringA_channel", this);
    tree.addParameterListener("midi_ringB_channel", this);
    tree.addParameterListener("midi_comb_channel_A", this);
    tree.addParameterListener("midi_comb_channel_B", this);
    tree.addParameterListener("jitter_depth", this);
    tree.addParameterListener("jitter_frequency", this);
    tree.addParameterListener("jitter_regularity", this);
    tree.addParameterListener("jitter_duty_cycle", this);
    tree.addParameterListener("jitter_slew", this);
    tree.addParameterListener("filter_input", this);
    tree.addParameterListener("jitter_low", this);
    tree.addParameterListener("noise_filter_cutoff_MIDI", this);
    tree.addParameterListener("flanger_centre_position_MIDI_A", this);
    tree.addParameterListener("flanger_centre_position_MIDI_B", this);
    tree.addParameterListener("ring_mod_freq_A_MIDI", this);
    tree.addParameterListener("ring_mod_freq_B_MIDI", this);

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
    MTS_DeregisterClient(client);
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
    if (!client) {
        client = MTS_RegisterClient();
    }

    host_sample_rate = sampleRate;
    num_channels = getNumInputChannels();

    cached_midi_channel_noise = 0;
    cached_midi_channel_ringA = 0;
    cached_midi_channel_ringB = 0;
    cached_midi_channel_comb_A = 0;
    cached_midi_channel_comb_B = 0;

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
    noise_filter_input = new Biquad[num_channels];
    parameterChanged("", 0);
    updateLowFilter();
    updateMidFilter();
    updateHighFilter();
    updateNoiseFilter();

    delay_line_length = sampleRate;
    delay_line = new float* [num_channels * 2];
    delay_line_write_idx = new int[num_channels * 2];
    delay_line_read_idx = new int[num_channels * 2];
    delay_line_read_excess = new float[num_channels * 2];
    lfo_angle = new double[num_channels * 2];

    clearDelayLines();

    centre_modifier = (float)(delay_line_length) / 60000.0f;
}

void ChaoticGoodAudioProcessor::clearDelayLines() {
    for (int j = 0; j < num_channels * 2; ++j) {
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

void ChaoticGoodAudioProcessor::advance_delay_line_read_head(int channel, int comb_idx) {
    channel += num_channels * comb_idx;
    int centre = 0;
    if (comb_idx == 0) {
        centre = cached_centre_position_A;
        if (cached_midi_comb_A) {
            centre *= cached_centre_position_MIDI_A * midi_pitch_wheel_comb_A;
        }
    }
    else {
        centre = cached_centre_position_B;
        if (cached_midi_comb_B) {
            centre *= cached_centre_position_MIDI_B * midi_pitch_wheel_comb_B;
        }
    }
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
    float newval = mod_value;
    if (comb_idx == 0) {
        newval *= cached_flanger_FM_A;
    }
    else {
        newval *= cached_flanger_FM_B;
    }
    delay_line_read_idx[channel] += trunc(newval);
    delay_line_read_excess[channel] = newval - floor(newval);
    while (delay_line_read_idx[channel] >= delay_line_length) {
        delay_line_read_idx[channel] -= delay_line_length;
    }
    while (delay_line_read_idx[channel] < 0) {
        delay_line_read_idx[channel] += delay_line_length;
    }
}

void ChaoticGoodAudioProcessor::advance_delay_line_write_head(int amount, int channel, int comb_idx) {
    channel += num_channels * comb_idx;
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

/*
    Values in the range [0, 1] multiply by [1, 2^max]
    Values in the range [-1, 0) multiply by [1/2^max, 1)
*/
float ChaoticGoodAudioProcessor::map_midi_frequency_offset(float value) {
    if (value >= 0) {
        value += 1;
    }
    else {
        value *= -1;  // range (0, 1]
        value += 1;
        value = 1 / value;
    }
    return value;
}

void ChaoticGoodAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) {

    // Jitter
    cached_jitter_depth = *tree.getRawParameterValue("jitter_depth");
    cached_jitter_slew = *tree.getRawParameterValue("jitter_slew");
    cached_jitter_duty_cycle = M_HALF_PI * (* tree.getRawParameterValue("jitter_duty_cycle") + 1);
    if (*tree.getRawParameterValue("jitter_low")) {
        cached_jitter_frequency = *tree.getRawParameterValue("jitter_frequency") * 10.0f;
    }
    else {
        cached_jitter_frequency = *tree.getRawParameterValue("jitter_frequency") * 100.0f;
    }
    cached_jitter_regularity = *tree.getRawParameterValue("jitter_regularity");
    cached_filter_input = *tree.getRawParameterValue("filter_input");

    // MIDI
    cached_midi_noise = *tree.getRawParameterValue("receive_midi_noise");
    cached_midi_ringA = *tree.getRawParameterValue("receive_midi_ringA");
    cached_midi_ringB = *tree.getRawParameterValue("receive_midi_ringB");
    cached_midi_comb_A = *tree.getRawParameterValue("receive_midi_comb_A");
    cached_midi_comb_B = *tree.getRawParameterValue("receive_midi_comb_B");
    cached_midi_channel_noise = *tree.getRawParameterValue("midi_noise_channel");
    cached_midi_channel_ringA = *tree.getRawParameterValue("midi_ringA_channel");
    cached_midi_channel_ringB = *tree.getRawParameterValue("midi_ringB_channel");
    cached_midi_channel_comb_A = *tree.getRawParameterValue("midi_comb_channel_A");
    cached_midi_channel_comb_B = *tree.getRawParameterValue("midi_comb_channel_B");

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

    if (cached_midi_noise) {
        cached_noise_filter_cutoff_MIDI = map_midi_frequency_offset(*tree.getRawParameterValue("noise_filter_cutoff_MIDI"));
    }
    else {
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
    cached_ring_mod_xfade = (* tree.getRawParameterValue("ring_mod_xfade") + 1) / 2;
    if (cached_midi_ringA) {
        cached_ring_mod_freq_A_MIDI = map_midi_frequency_offset(*tree.getRawParameterValue("ring_mod_freq_A_MIDI"));
    }
    else {
        cached_ring_mod_freq_A = *tree.getRawParameterValue("ring_mod_freq_A");
    }

    if (cached_midi_ringB) {
        cached_ring_mod_freq_B_MIDI = map_midi_frequency_offset(*tree.getRawParameterValue("ring_mod_freq_B_MIDI"));
    }
    else {
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

    // Flanger
    if (parameterID == "comb_mode") {
        clearDelayLines();
    }
    cached_flanger_amount = *tree.getRawParameterValue("flanger_amount");
    cached_comb_mode = (int) * tree.getRawParameterValue("comb_mode");

    if (cached_midi_comb_A) {
        cached_centre_position_MIDI_A = map_midi_frequency_offset(-1 * *tree.getRawParameterValue("flanger_centre_position_MIDI_A"));
    }
    else {
        float centre_position = tree.getParameterRange("flanger_centre_position_A").end;
        centre_position -= *tree.getRawParameterValue("flanger_centre_position_A");
        centre_position *= centre_modifier;
        if (centre_position < 0) {
            centre_position = 0;
        }
        cached_centre_position_A = floor(centre_position);
    }

    if (cached_midi_comb_B) {
        cached_centre_position_MIDI_B = map_midi_frequency_offset(-1 * *tree.getRawParameterValue("flanger_centre_position_MIDI_B"));
    }
    else {
        float centre_position = tree.getParameterRange("flanger_centre_position_B").end;
        centre_position -= *tree.getRawParameterValue("flanger_centre_position_B");
        centre_position *= centre_modifier;
        if (centre_position < 0) {
            centre_position = 0;
        }
        cached_centre_position_B = floor(centre_position);
    }
    /*
    cached_lfo_depth = *tree.getRawParameterValue("flanger_lfo_depth");
    cached_lfo_depth *= fmin(cached_centre_position, delay_line_length - cached_centre_position);
    cached_lfo_depth /= 10;

    cached_lfo_rate = *tree.getRawParameterValue("flanger_lfo_rate") * M_TWO_PI;
    cached_lfo_rate *= 2;
    cached_lfo_rate /= host_sample_rate;
    */
    cached_resonance = *tree.getRawParameterValue("flanger_resonance");
    cached_resonance /= 2.0f;
    cached_resonance += 1.5f;

    cached_flanger_AM = *tree.getRawParameterValue("flanger_AM");
    cached_flanger_FM_A = cached_flanger_AM * fmin(cached_centre_position_A, delay_line_length - cached_centre_position_A);
    cached_flanger_FM_A /= 10;
    cached_flanger_FM_B = cached_flanger_AM * fmin(cached_centre_position_B, delay_line_length - cached_centre_position_B);
    cached_flanger_FM_B /= 10;

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
    cached_dry_blend = 1 - *tree.getRawParameterValue("amp_blend");
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
    float cutoff = cached_noise_filter_cutoff;
    if (cached_midi_noise) {
        cutoff *= cached_noise_filter_cutoff_MIDI;
        cutoff *= midi_pitch_wheel_noise;
    }
    for (int i = 0; i < num_channels; ++i) {
        noise_filter[i].recalculate(
            host_sample_rate,
            cutoff,
            cached_noise_filter_resonance,
            cached_noise_filter_gain,
            cached_noise_filter_type
        );
        noise_filter_input[i].recalculate(
            host_sample_rate,
            cutoff,
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
            + (sample * rm_A * cached_ring_mod_xfade + sample * rm_B * (1 - cached_ring_mod_xfade));
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

float ChaoticGoodAudioProcessor::apply_jitter(float sample) {
    // Advance jitter_angle using jitter_frequency
    jitter_angle += cached_jitter_frequency / host_sample_rate;
    if (jitter_angle > M_TWO_PI) {
        jitter_angle -= M_TWO_PI;
        jitter_offset = ((float)(rand() % cached_noise_resolution) * (F_RAND_MAX / cached_noise_resolution) / (F_HALF_RAND_MAX)) - 1;
        jitter_offset *= 1 - cached_jitter_regularity;
        jitter_offset *= M_PI;
    }

    // These should really be cached in parameterChanged()
    float JITTER_RAMP_DOWN_DISTANCE = (M_PI * cached_jitter_slew);
    float JITTER_RAMP_DOWN_START = M_TWO_PI - JITTER_RAMP_DOWN_DISTANCE;
    float cutover = M_PI * cached_jitter_duty_cycle + jitter_offset;
    float JITTER_RAMP_UP_DISTANCE = cutover * cached_jitter_slew;
    float JITTER_RAMP_UP_START = cutover - JITTER_RAMP_UP_DISTANCE;

    if (jitter_angle > JITTER_RAMP_DOWN_START) {
        float scale = jitter_angle - JITTER_RAMP_DOWN_START;
        scale /= JITTER_RAMP_DOWN_DISTANCE;
        return sample * (1 - scale);
    }
    else if (jitter_angle > cutover) {
        return sample;
    }
    else if (jitter_angle > JITTER_RAMP_UP_START) {
        float scale = jitter_angle - JITTER_RAMP_UP_START;
        scale /= JITTER_RAMP_UP_DISTANCE;
        return sample * scale;
    }
    else {
        return 0.0f;
    }

    if (jitter_angle > M_PI * cached_jitter_duty_cycle + jitter_offset) {
        return 0.0f;
    }
    else {
        // Apply slew rate
        return sample;
    }
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

    sample = apply_ring_mod(sample, channel);
    float flange_tmp = apply_flange(sample, channel);
    sample = flange_tmp * cached_flanger_amount + sample * (1 - cached_flanger_amount);
    
    float dry = sample;

    sample = mid_filter[channel].apply(sample);

    sample = apply_clip(sample, channel);

    sample = low_filter[channel].apply(sample);

    // Bias
    sample = sgn(sample) * pow(abs(sample), cached_power_positive);

    sample = high_filter[channel].apply(sample);

    // Output Level             
    sample *= cached_output_level;

    // Clamp to valid range
    // NB if amplifier is blended out, it can still be necessary to clamp this later (see processBlock)
    sample = tanh(sample);

    sample = dry * cached_dry_blend + sample * (1 - cached_dry_blend);

    return sample;
}

float ChaoticGoodAudioProcessor::read_comb_delay_line(int channel, int comb_idx) {
    channel += num_channels * comb_idx;
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
float ChaoticGoodAudioProcessor::apply_comb(float sample, int channel, int comb_idx, float scale_resonance = 1.0f) {
    int channel_idx = channel + 2 * comb_idx;
    lfo_angle[channel_idx] += cached_lfo_rate;
    if (lfo_angle[channel_idx] > M_TWO_PI) {
        lfo_angle[channel_idx] -= M_TWO_PI;
    }
    advance_delay_line_write_head(1, channel, comb_idx);
    advance_delay_line_read_head(channel, comb_idx);
    float result = read_comb_delay_line(channel, comb_idx);

    result = clamp((result + sample) / 2, -1, 1);

    delay_line[channel_idx][delay_line_write_idx[channel_idx]] = cached_resonance * result * scale_resonance;

    if (cached_comb_mod_mode == COMB_MOD_MODE_AM_A) {
        float tmp = delay_line[channel_idx][delay_line_write_idx[channel_idx]]
            * apply_wave(ring_mod_angle_A[channel_idx], cached_ring_mod_waveA) * 1.1f;
        delay_line[channel_idx][delay_line_write_idx[channel_idx]] =
            tmp * cached_flanger_AM
            + delay_line[channel_idx][delay_line_write_idx[channel_idx]] * (1 - cached_flanger_AM);
    }
    else if (cached_comb_mod_mode == COMB_MOD_MODE_AM_B) {
        float tmp = delay_line[channel_idx][delay_line_write_idx[channel_idx]]
            * apply_wave(ring_mod_angle_B[channel_idx], cached_ring_mod_waveB) * 1.1f;
        delay_line[channel_idx][delay_line_write_idx[channel_idx]] =
            tmp * cached_flanger_AM
            + delay_line[channel_idx][delay_line_write_idx[channel_idx]] * (1 - cached_flanger_AM);
    }

    return result;
}

float ChaoticGoodAudioProcessor::apply_flange(float sample, int channel) {
    switch (cached_comb_mode) {
    case COMB_MODE_SINGLE:
        sample = apply_comb(sample, channel, 0);
        break;
    case COMB_MODE_SERIES:
        sample = apply_comb(sample, channel, 0, 0.9);
        sample = apply_comb(sample, channel, 1);
        break;
    case COMB_MODE_PARALLEL:
        sample = apply_comb(sample, channel, 0)/2 + apply_comb(sample, channel, 1)/2;
        break;
    case COMB_MODE_MULTIPLY:
        sample = apply_comb(sample, channel, 0) * apply_comb(sample, channel, 1);
        break;
    }
    return sample;
}
void ChaoticGoodAudioProcessor::recalculate_pitch_bends_from_MIDI(juce::MidiMessage m) {
    int midi_pitch_wheel_value = m.getPitchWheelValue();
    int channel = m.getChannel();
    float bend_amount = 0.0f;
    if (midi_pitch_wheel_value < 8192) {
        bend_amount = -1 * (8192.0f - (float)midi_pitch_wheel_value) / 8192.0f;
    }
    else if (midi_pitch_wheel_value > 8192) {
        bend_amount = ((float)midi_pitch_wheel_value - 8192.0f) / 8192.0f;
    }
    if (cached_midi_noise) {
        if (cached_midi_channel_noise == 0 || channel == cached_midi_channel_noise) {
            midi_pitch_wheel_noise = pow(2, bend_amount) * midi_pitch_bend_range / 12;
            updateNoiseFilter();
        }
    }
    if (cached_midi_ringA) {
        if (cached_midi_channel_ringA == 0 || channel == cached_midi_channel_ringA) {
            midi_pitch_wheel_ringA = 1 + bend_amount * midi_pitch_bend_range / 12;
        }
    }
    if (cached_midi_ringB) {
        if (cached_midi_channel_ringB == 0 || channel == cached_midi_channel_ringB) {
            midi_pitch_wheel_ringB = 1 + bend_amount * midi_pitch_bend_range / 12;
        }
    }
    if (cached_midi_comb_A) {
        if (cached_midi_channel_comb_A == 0 || channel == cached_midi_channel_comb_A) {
            if (midi_pitch_wheel_value < 8192) {
                bend_amount /= 1.5;
                midi_pitch_wheel_comb_A = 1 + bend_amount * midi_pitch_bend_range / 12;
                midi_pitch_wheel_comb_A = 1 / midi_pitch_wheel_comb_A;
            } else if(midi_pitch_wheel_value > 8192){
                midi_pitch_wheel_comb_A = 1 + bend_amount * midi_pitch_bend_range / 12;
                midi_pitch_wheel_comb_A = 1 / midi_pitch_wheel_comb_A;
           }
            else {
                midi_pitch_wheel_comb_A = 1;
            }
        }
    }
    if (cached_midi_comb_B) {
        if (cached_midi_channel_comb_B == 0 || channel == cached_midi_channel_comb_B) {
            if (midi_pitch_wheel_value < 8192) {
                bend_amount /= 1.5;
                midi_pitch_wheel_comb_B = 1 + bend_amount * midi_pitch_bend_range / 12;
                midi_pitch_wheel_comb_B = 1 / midi_pitch_wheel_comb_B;
            }
            else if (midi_pitch_wheel_value > 8192) {
                midi_pitch_wheel_comb_B = 1 + bend_amount * midi_pitch_bend_range / 12;
                midi_pitch_wheel_comb_B = 1 / midi_pitch_wheel_comb_B;
            }
            else {
                midi_pitch_wheel_comb_B = 1;
            }
        }
    }

}
void ChaoticGoodAudioProcessor::recalculate_pitches_from_MIDI(juce::MidiMessage m) {
    int MIDI_note_number = m.getNoteNumber();
    int channel = m.getChannel();
    float f = 0.0f;
    if (client) {
        f = MTS_NoteToFrequency(client, MIDI_note_number, channel);
    }
    else {
        f = m.getMidiNoteInHertz(MIDI_note_number);
    }
    if (cached_midi_noise) {
        if (cached_midi_channel_noise == 0 || channel == cached_midi_channel_noise) {
            midi_note_is_playing_noise = true;
            midi_note_number_noise = MIDI_note_number;
            cached_noise_filter_cutoff = f;
            updateNoiseFilter();
        }
    }
    if (cached_midi_ringA) {
        if (cached_midi_channel_ringA == 0 || channel == cached_midi_channel_ringA) {
            midi_note_is_playing_ringA = true;
            midi_note_number_ringA = MIDI_note_number;
            cached_ring_mod_freq_A = f * cached_ring_mod_freq_A_MIDI;
        }
    }
    if (cached_midi_ringB) {
        if (cached_midi_channel_ringB == 0 || channel == cached_midi_channel_ringB) {
            midi_note_is_playing_ringB = true;
            midi_note_number_ringB = MIDI_note_number;
            cached_ring_mod_freq_B = f * cached_ring_mod_freq_B_MIDI;
        }
    }
    if (cached_midi_comb_A) {
        if (cached_midi_channel_comb_A == 0 || channel == cached_midi_channel_comb_A) {
            midi_note_is_playing_comb_A = true;
            midi_note_number_comb_A = MIDI_note_number;
            if (f > 0) {
                cached_centre_position_A = host_sample_rate / f;
            }
        }
    }
    if (cached_midi_comb_B) {
        if (cached_midi_channel_comb_B == 0 || channel == cached_midi_channel_comb_B) {
            midi_note_is_playing_comb_B = true;
            midi_note_number_comb_B = MIDI_note_number;
            if (f > 0) {
                cached_centre_position_B = host_sample_rate / f;
            }
        }
    }
}

void ChaoticGoodAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    float* channel;
    for (auto j = 0; j < buffer.getNumSamples(); ++j) {
        float raw[] = {0.0f, 0.0f};
        float cooked[] = { 0.0f, 0.0f };
        // Increment these for both channels first to keep them in sync
        for (auto i = 0; i < 2; ++i) {
            if (cached_midi_ringA) {
                ring_mod_angle_A[i] += (cached_ring_mod_freq_A * cached_ring_mod_freq_A_MIDI * midi_pitch_wheel_ringA) / host_sample_rate;
            }
            else {
                ring_mod_angle_A[i] += cached_ring_mod_freq_A / host_sample_rate;
            }
            if (ring_mod_angle_A[i] >= M_TWO_PI) {
                ring_mod_angle_A[i] -= M_TWO_PI;
            }
            if (cached_midi_ringB) {
                ring_mod_angle_B[i] += (cached_ring_mod_freq_B * cached_ring_mod_freq_B_MIDI * midi_pitch_wheel_ringB) / host_sample_rate;
            }
            else {
                ring_mod_angle_B[i] += cached_ring_mod_freq_B / host_sample_rate;
            }
            if (ring_mod_angle_B[i] >= M_TWO_PI) {
                ring_mod_angle_B[i] -= M_TWO_PI;
            }
        }
        for (auto i = 0; i < 2; ++i) {
            channel = buffer.getWritePointer(i);
            if (cached_filter_input) {
                channel[j] = noise_filter_input[i].apply(channel[j]);
            }
            if (cached_jitter_depth > 0.0f) {
                channel[j] = cached_jitter_depth * apply_jitter(channel[j]) + (1 - cached_jitter_depth) * channel[j];
            }
            float sample = channel[j];
            sample = sample * (1 - cached_noise_level) + cached_noise_level * apply_noise(sample, i);
            channel[j] = clamp(process(sample, i), -1, 1);
        }
        // Fix for occasional descynchronization issues
        delay_line_write_idx[1] = delay_line_write_idx[0];
        delay_line_read_idx[1] = delay_line_read_idx[0];
    }
    juce::MidiMessage m;
    juce::MidiBuffer::Iterator it{ midiMessages };
    int sample;

    while (it.getNextEvent(m, sample))
    {
        if (m.isPitchWheel()) {
            recalculate_pitch_bends_from_MIDI(m);
        } else if (m.isNoteOn()) {
            recalculate_pitches_from_MIDI(m);
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
            if (cached_midi_channel_comb_A == 0 || c == cached_midi_channel_comb_A) {
                if (m.getNoteNumber() == midi_note_number_comb_A) {
                    midi_note_is_playing_comb_A = false;
                }
            }
            if (cached_midi_channel_comb_B == 0 || c == cached_midi_channel_comb_B) {
                if (m.getNoteNumber() == midi_note_number_comb_B) {
                    midi_note_is_playing_comb_B = false;
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
        return juce::String("SIN");
    case waves::WAVE_TRIANGLE:
        return juce::String("TRI");
    case waves::WAVE_SQUARE:
        return juce::String("SQU");
    case waves::WAVE_RAMP:
        return juce::String("SAW");
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
