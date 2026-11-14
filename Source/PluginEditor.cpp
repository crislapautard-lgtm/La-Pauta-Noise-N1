#include "PluginEditor.h"

namespace
{
    const juce::Colour orange { 0xffe8962a };
    const juce::Colour scaleGrey { 0xff8f98a3 };
}

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
      attachment (p.apvts, "suppress", fader)
{
    setSize (345, 480);

    fader.setLookAndFeel (&lnf);
    fader.setSliderStyle (juce::Slider::LinearVertical);
    fader.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    fader.setDoubleClickReturnValue (true, 41.0);
    fader.setSliderSnapsToMousePosition (false);
    addAndMakeVisible (fader);

    startTimerHz (30);
}

PluginEditor::~PluginEditor()
{
    fader.setLookAndFeel (nullptr);
}

void PluginEditor::resized()
{
    // Zona activa del fader: de valor 100 (arriba) a valor 0 (abajo)
    fader.setBounds (215, (int) faderTopY, 85, (int) (faderBottomY - faderTopY));
}

void PluginEditor::timerCallback()
{
    const float target = juce::jlimit (-48.0f, 0.0f, processor.inputLevelDb.load());
    // subida rapida, caida suave
    displayedLevelDb = (target > displayedLevelDb) ? target
                                                   : displayedLevelDb * 0.85f + target * 0.15f;
    repaint();
}

void PluginEditor::drawValueBox (juce::Graphics& g, juce::Rectangle<float> r, const juce::String& text)
{
    g.setColour (juce::Colours::black);
    g.fillRoundedRectangle (r, 5.0f);
    g.setColour (juce::Colour (0xff2a2a2a));
    g.drawRoundedRectangle (r, 5.0f, 1.0f);
    g.setColour (orange);
    g.setFont (juce::FontOptions (13.0f));
    g.drawText (text, r, juce::Justification::centred);
}

void PluginEditor::drawMeterPanel (juce::Graphics& g)
{
    // Panel oscuro con esquinas derechas redondeadas
    juce::Rectangle<float> panel (-10.0f, 105.0f, 173.0f, 281.0f);
    g.setColour (juce::Colour (0xff1e1f21));
    g.fillRoundedRectangle (panel, 14.0f);
    g.setColour (juce::Colour (0xff3a3d41));
    g.drawRoundedRectangle (panel, 14.0f, 1.5f);

    const float top = 134.0f, bottom = 330.0f;
    auto dbToY = [&] (float db) { return top + (0.0f - db) / 48.0f * (bottom - top); };

    // Escala a ambos lados
    g.setFont (juce::FontOptions (11.0f));
    for (int db = 0; db >= -48; db -= 6)
    {
        const float y = dbToY ((float) db);
        g.setColour (scaleGrey.withAlpha (0.55f));
        g.drawLine (48.0f, y, 56.0f, y, 1.0f);
        g.drawLine (111.0f, y, 119.0f, y, 1.0f);
        g.setColour (scaleGrey.withAlpha (0.7f));
        g.drawText (juce::String (db), juce::Rectangle<float> (14.0f, y - 7.0f, 32.0f, 14.0f),
                    juce::Justification::centredRight);
        g.drawText (juce::String (db), juce::Rectangle<float> (122.0f, y - 7.0f, 32.0f, 14.0f),
                    juce::Justification::centredLeft);
    }

    // Marco del medidor
    juce::Rectangle<float> frame (69.0f, 131.0f, 34.0f, 205.0f);
    g.setColour (juce::Colour (0xff0d0d0e));
    g.fillRoundedRectangle (frame, 4.0f);
    g.setColour (juce::Colour (0xff4a4d52));
    g.drawRoundedRectangle (frame, 4.0f, 1.5f);

    // Barra naranja
    const float levelY = dbToY (juce::jlimit (-48.0f, 0.0f, displayedLevelDb));
    juce::Rectangle<float> bar (72.0f, top, 28.0f, bottom - top + 3.0f);
    g.setColour (juce::Colour (0xff262626));
    g.fillRect (bar);
    g.setColour (orange);
    g.fillRect (juce::Rectangle<float> (72.0f, levelY, 28.0f, (bottom + 3.0f) - levelY));

    // Trama de puntos sutil sobre la barra
    g.setColour (juce::Colours::black.withAlpha (0.12f));
    for (float y = top; y < bottom + 3.0f; y += 2.0f)
        g.drawLine (72.0f, y, 100.0f, y, 0.5f);

    drawValueBox (g, { 58.0f, 348.0f, 53.0f, 23.0f },
                  juce::String (displayedLevelDb, 1));
}

void PluginEditor::drawFader (juce::Graphics& g)
{
    // Escalas 0..100 a ambos lados
    g.setFont (juce::FontOptions (14.0f));
    for (int v = 0; v <= 100; v += 10)
    {
        const float y = faderBottomY - (float) v / 100.0f * (faderBottomY - faderTopY);
        g.setColour (scaleGrey.withAlpha (0.55f));
        g.drawLine (215.0f, y, 236.0f, y, 1.0f);
        g.drawLine (277.0f, y, 298.0f, y, 1.0f);
        g.setColour (scaleGrey.withAlpha (0.8f));
        g.drawText (juce::String (v), juce::Rectangle<float> (172.0f, y - 8.0f, 38.0f, 16.0f),
                    juce::Justification::centredRight);
        g.drawText (juce::String (v), juce::Rectangle<float> (301.0f, y - 8.0f, 38.0f, 16.0f),
                    juce::Justification::centredLeft);
    }

    // Carril
    juce::Rectangle<float> rail (245.0f, 42.0f, 24.0f, 353.0f);
    g.setColour (juce::Colour (0xff5d646c));
    g.fillRoundedRectangle (rail, 5.0f);
    g.setColour (juce::Colour (0xff1b1c1e));
    g.fillRoundedRectangle (juce::Rectangle<float> (251.0f, 51.0f, 12.0f, 335.0f), 6.0f);

    // Cabezal (thumb)
    const float value = (float) fader.getValue();
    const float centerY = faderBottomY - value / 100.0f * (faderBottomY - faderTopY);
    juce::Rectangle<float> thumb (faderCenterX - 35.0f, centerY - 52.0f, 70.0f, 105.0f);

    // Sombra
    juce::Path shadow;
    shadow.addRectangle (thumb.translated (6.0f, 8.0f));
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.fillPath (shadow);

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3b3b3d), thumb.getX(), thumb.getY(),
                                             juce::Colour (0xff0b0b0c), thumb.getX(), thumb.getBottom(), false));
    g.fillRect (thumb);

    // Brillo superior e inferior
    g.setColour (juce::Colour (0xffb4b8bd));
    g.fillRect (thumb.removeFromTop (9.0f).reduced (0.0f, 2.0f));

    thumb = juce::Rectangle<float> (faderCenterX - 35.0f, centerY - 52.0f, 70.0f, 105.0f);
    // Estrias horizontales
    g.setColour (juce::Colour (0xff6f7378).withAlpha (0.6f));
    for (int i = 1; i <= 8; ++i)
        g.drawLine (thumb.getX(), thumb.getY() + 10.0f + (float) i * 8.5f,
                    thumb.getRight(), thumb.getY() + 10.0f + (float) i * 8.5f, 1.0f);

    // Linea central naranja (indicador)
    g.setColour (orange);
    g.fillRect (thumb.getX(), centerY - 1.0f, thumb.getWidth(), 2.0f);

    drawValueBox (g, { 231.0f, 442.0f, 53.0f, 23.0f }, juce::String (value, 1));
}

void PluginEditor::drawTitle (juce::Graphics& g)
{
    // Marca superior
    g.setColour (juce::Colour (0xffc9ced4));
    g.setFont (juce::FontOptions (15.0f, juce::Font::bold));
    g.drawText ("LA PAUTA", juce::Rectangle<float> (35.0f, 50.0f, 100.0f, 20.0f), juce::Justification::centred);
    g.setColour (juce::Colour (0xff8f98a3));
    g.setFont (juce::FontOptions (10.0f));
    g.drawText ("AUDIO PLUGINS", juce::Rectangle<float> (35.0f, 68.0f, 100.0f, 14.0f), juce::Justification::centred);

    // Titulo N + caja con 1
    g.setColour (juce::Colour (0xffcdd1d6));
    g.setFont (juce::FontOptions (62.0f, juce::Font::bold));
    g.drawText ("N", juce::Rectangle<float> (38.0f, 400.0f, 60.0f, 70.0f), juce::Justification::centredLeft);

    juce::Rectangle<float> box (84.0f, 407.0f, 38.0f, 45.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff0f1f2), box.getX(), box.getY(),
                                             juce::Colour (0xff8c9096), box.getX(), box.getBottom(), false));
    g.fillRoundedRectangle (box, 5.0f);
    g.setColour (juce::Colour (0xff1a1a1a));
    g.setFont (juce::FontOptions (40.0f, juce::Font::bold));
    g.drawText ("1", box, juce::Justification::centred);

    g.setColour (juce::Colour (0xffb9bec5));
    g.setFont (juce::FontOptions (17.0f));
    g.drawText ("Noise Suppressor", juce::Rectangle<float> (30.0f, 452.0f, 180.0f, 22.0f),
                juce::Justification::centredLeft);
}

void PluginEditor::paint (juce::Graphics& g)
{
    // Fondo gris azulado oscuro, estilo metal
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff4a525b), 0.0f, 0.0f,
                                             juce::Colour (0xff262a30), 0.0f, (float) getHeight(), false));
    g.fillAll();

    // Vineta suave
    g.setGradientFill (juce::ColourGradient (juce::Colours::transparentBlack, getWidth() * 0.5f, getHeight() * 0.45f,
                                             juce::Colours::black.withAlpha (0.35f), 0.0f, 0.0f, true));
    g.fillAll();

    // Linea superior
    g.setColour (juce::Colour (0xff7d858e));
    g.fillRect (0.0f, 3.0f, (float) getWidth(), 2.0f);

    drawTitle (g);
    drawMeterPanel (g);
    drawFader (g);
}
