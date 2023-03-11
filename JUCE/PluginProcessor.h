/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Biquad.cpp"
#include "Waves.h"
#include "libMTSClient.h"

#ifndef M_PI
#define M_PI		3.14159265358
#endif

#ifndef M_TWO_PI
#define M_TWO_PI    6.28318530718
#endif

#ifndef M_HALF_PI
#define M_HALF_PI    1.570796327
#endif

enum {
    CLIP_TYPE_NONE,
    CLIP_TYPE_HARD,
    CLIP_TYPE_LOGISTIC,
    CLIP_TYPE_ATAN,
    CLIP_TYPE_ROOT,
    CLIP_TYPE_VIN_HALF_GAIN,
    CLIP_TYPE_TANH,
    CLIP_TYPE_SINE
};

enum {
    MIDI_NOTE_OFF_IGNORE,
    MIDI_NOTE_OFF_CLEAN,
    MIDI_NOTE_OFF_SILENT
};

enum{
    RM_MODE_SINGLE,
    RM_MODE_SERIES,
    RM_MODE_PARALLEL,
    RM_MODE_FM,
    RM_MODE_AM,
    RM_MODE_FM_2,
    RM_MODE_AM_2,
    RM_MODE_SYNTH_FM,
    RM_MODE_SYNTH_AM
};

enum {
    COMB_MODE_SINGLE,
    COMB_MODE_SERIES,
    COMB_MODE_PARALLEL,
    COMB_MODE_MULTIPLY
};

enum {
    COMB_MOD_MODE_FM_A,
    COMB_MOD_MODE_FM_B,
    COMB_MOD_MODE_AM_A,
    COMB_MOD_MODE_AM_B,
    COMB_MOD_MODE_NOISE
};

enum {
    NOISE_WHITE,
    NOISE_LOGIC,
    NOISE_SH
};

enum {
    ENV_SEG_ATTACK,
    ENV_SEG_SUSTAIN,
    ENV_SEG_DECAY,
    ENV_SEG_OFF
};

//==============================================================================
/**
*/
class ChaoticGoodAudioProcessor  : public juce::AudioProcessor
    , public juce::AudioProcessorValueTreeState::Listener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    ChaoticGoodAudioProcessor();
    ~ChaoticGoodAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameters
    juce::AudioProcessorValueTreeState tree;
    void parameterChanged(const juce::String& parameterID, float newValue);

    void saveFactoryPreset(juce::String name);

    //==============================================================================
    // Used by the GUI to see what's going on
    juce::String get_rm_wave_type(int which);
    juce::String get_comb_mod_mode_type();
    void cycleRMWaveParamValue(juce::String parameterID);
    void cycleCombModParamValue();

    // Settings that need to be accessed directly by the GUI
    // (The param tree doesn't seem to update when needed)
    bool cached_midi_noise = false;
    bool cached_midi_ringA = false;
    bool cached_midi_ringB = false;
    bool cached_midi_comb_A = false;
    bool cached_midi_comb_B = false;
    int cached_ring_mod_mode = 0;
    int cached_comb_mode = 0;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChaoticGoodAudioProcessor)
    float F_RAND_MAX = (float)RAND_MAX;
    float F_HALF_RAND_MAX = ((float)RAND_MAX) / 2.0f;
    float MAX_GAIN = 20.0f;
    float MAX_GATE = 5.0f;
    float MAX_OUTPUT = 0.4f;
    double host_sample_rate;
    int num_channels;
    float angle_osc_1 = 0.0f;
    float angle_osc_2 = 0.0f;

    // General processing
    int cached_type_positive = 0;
    float cached_output_level;
    float cached_dry_blend;
    float cached_input_level_positive;
    float cached_power_positive;
    float clamp(float val, float min, float max);
    int sgn(double v);
    float fold(float sample);
    float process(float sample, int channel);
    float apply_clip(float sample, int channel);

    // Ring Mod
    double cached_ring_mod_freq_A;
    double cached_ring_mod_freq_B;
    double cached_ring_mod_freq_A_MIDI;
    double cached_ring_mod_freq_B_MIDI;
    bool cached_ring_mod_low_A;
    bool cached_ring_mod_low_B;
    float cached_ring_mod_amt;
    float cached_ring_mod_fm_amt;
    double* ring_mod_angle_A;
    double* ring_mod_angle_B;
    int cached_ring_mod_waveA = 0;
    int cached_ring_mod_waveB = 0;
    float cached_ring_mod_xfade = 0.0f;
    float apply_ring_mod(float sample, int channel);
    float apply_wave(float angle, int waveform);

    // MIDI
    int cached_midi_channel_noise;
    int cached_midi_channel_ringA;
    int cached_midi_channel_ringB;
    int cached_midi_channel_comb_A;
    int cached_midi_channel_comb_B;
    bool midi_note_is_playing_noise = false;
    bool midi_note_is_playing_ringA = false;
    bool midi_note_is_playing_ringB = false;
    bool midi_note_is_playing_comb_A = false;
    bool midi_note_is_playing_comb_B = false;
    int midi_note_number_noise = 0;
    int midi_note_number_ringA = 0;
    int midi_note_number_ringB = 0;
    int midi_note_number_comb_A = 0;
    int midi_note_number_comb_B = 0;
    float midi_pitch_wheel_noise = 1.0f;
    float midi_pitch_wheel_ringA = 1.0f;
    float midi_pitch_wheel_ringB = 1.0f;
    float midi_pitch_wheel_comb_A = 1.0f;
    float midi_pitch_wheel_comb_B = 1.0f;
    float midi_pitch_bend_range = 12;   // semitones
    void recalculate_pitches_from_MIDI(juce::MidiMessage m);
    void recalculate_pitch_bends_from_MIDI(juce::MidiMessage m);
    float map_midi_frequency_offset(float value);

    // Filters

    Biquad* low_filter;
    float cached_low_filter_cutoff;
    float cached_low_filter_gain = -10.0f;
    void updateLowFilter();

    Biquad* mid_filter;
    float cached_mid_filter_cutoff;
    float cached_mid_filter_gain = 10.0f;
    void updateMidFilter();

    Biquad* high_filter;
    float cached_high_filter_cutoff;
    float cached_high_filter_gain = -10.0f;
    void updateHighFilter();

    Biquad* noise_filter;
    Biquad* noise_filter_input;
    float cached_noise_filter_cutoff;
    float cached_noise_filter_cutoff_MIDI;
    float cached_noise_filter_gain = -10.0f;
    int cached_noise_filter_type;
    float cached_noise_filter_resonance = 1.0f;
    void updateNoiseFilter();

    // Flanger
    float** delay_line;
    int delay_line_length = 0;
    int* delay_line_read_idx = 0;
    float* delay_line_read_excess;
    int* delay_line_write_idx = 0;
    double* lfo_angle;
    float centre_modifier = 0.0f;
    void advance_delay_line_read_head(int channel, int comb_idx);
    void advance_delay_line_write_head(int amount, int channel, int comb_idx);
    void clearDelayLines();
    float apply_flange(float sample, int channel);
    float apply_comb(float sample, int channel, int comb_idx, float scale_resonance);
    float read_comb_delay_line(int channel, int comb_idx);

    double cached_lfo_rate = 0.0;
//    float cached_lfo_depth = 0.0f;
    int cached_centre_position_A = 0;
    int cached_centre_position_B = 0;
    float cached_centre_position_MIDI_A = 0.0f;
    float cached_centre_position_MIDI_B = 0.0f;
    float cached_resonance = 0.0f;
    float cached_flanger_amount = 0.0f;
    float cached_flanger_AM = 0.0f;
    float cached_flanger_FM_A = 0.0f;
    float cached_flanger_FM_B = 0.0f;
    int cached_comb_mod_mode = 0;

    // Noise
    int cached_noise_density;
    float cached_noise_level;
    float last_noise_sample = 0.0f;
    int cached_noise_resolution = 100;
    float apply_noise(float sample, int channel);
    
    // Jitter and Input Stage
    float cached_jitter_depth = 0.0f;
    float cached_jitter_duty_cycle = 0.0f;
    float cached_jitter_frequency = 0.0f;
    float cached_jitter_regularity = 0.0f;
    float cached_jitter_slew = 0.0f;
    float cached_filter_input = false;
    float jitter_level = 1.0f;
    float jitter_angle = 0.0f;
    float jitter_offset = 0.0f;
    float apply_jitter(float sample);

;   //==============================================
    // Factory Presets
    juce::String* preset_names;
    juce::String* preset_filenames;
    int current_programme;
    const int NUM_PROGRAMMES = 13;

    // MTS-ESP
    MTSClient* client = 0;
};
