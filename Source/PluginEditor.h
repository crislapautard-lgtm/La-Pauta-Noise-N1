#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// LookAndFeel que no dibuja el slider (el fader se pinta en el editor)
// y usa radio de thumb 0 para que el recorrido coincida con los limites del componente.
class InvisibleSliderLnF : public juce::LookAndFeel_V4
{
public:
    int getSliderThumbRadius (juce::Slider&) override { return 0; }
    void drawLinearSlider (juce::Graphics&, int, int, int, int, float, float, float,
                           juce::Slider::SliderStyle, juce::Slider&) override {}
};

class PluginEditor : public juce::AudioProcessorEditor,
                     private juce::Timer
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawMeterPanel (juce::Graphics&);
    void drawFader (juce::Graphics&);
    void drawValueBox (juce::Graphics&, juce::Rectangle<float> r, const juce::String& text);
    void drawTitle (juce::Graphics&);

    PluginProcessor& processor;
    InvisibleSliderLnF lnf;
    juce::Slider fader;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

    float displayedLevelDb = -48.0f;

    // Geometria (ventana 345 x 480)
    static constexpr float faderTopY = 73.0f;     // valor 100
    static constexpr float faderBottomY = 372.0f; // valor 0
    static constexpr float faderCenterX = 257.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
