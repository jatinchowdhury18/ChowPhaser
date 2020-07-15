#ifndef MYLNF_H_INCLUDED
#define MYLNF_H_INCLUDED

#include <JuceHeader.h>

class MyLNF : public LookAndFeel_V4
{
public:
    MyLNF() = default;

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override
    {
        int diameter = (width > height)? height : width;
        if (diameter < 16) return;

        juce::Point<float> centre (x + std::floor (width * 0.5f + 0.5f), y + std::floor (height * 0.5f + 0.5f));
        diameter -= (diameter % 2)? 9 : 8;
        float radius = diameter * 0.5f;
        x = int (centre.x - radius);
        y = int (centre.y - radius);

        const auto bounds = juce::Rectangle<int> (x, y, diameter, diameter).toFloat();   

        auto b = knob->getBounds().toFloat();
        knob->setTransform (AffineTransform::rotation (MathConstants<float>::twoPi * ((sliderPos - 0.5f) * 300.0f / 360.0f),
            b.getCentreX(), b.getCentreY()));

        auto knobBounds = (bounds * 0.75f).withCentre (centre);
        knob->drawWithin (g, knobBounds, RectanglePlacement::stretchToFit, 1.0f);

        const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        const juce::Colour fill = slider.findColour (juce::Slider::rotarySliderFillColourId);
        constexpr float arcFactor = 0.85f;

        Path valueArc;
        valueArc.addPieSegment (bounds, rotaryStartAngle, rotaryEndAngle, arcFactor);
        g.setColour (Colour (0xff484856));
        g.fillPath (valueArc);
        valueArc.clear();

        valueArc.addPieSegment (bounds, rotaryStartAngle, toAngle, arcFactor);
        ColourGradient gr (Colour (0xff4f4fb7), { centre.x - radius, centre.y },
                           Colour (0xffb54fb7), { centre.x + radius, centre.y }, false);
        g.setGradientFill (gr);
        g.fillPath (valueArc);
    }

private:
    std::unique_ptr<Drawable> knob = Drawable::createFromImageData (BinaryData::knob_svg, BinaryData::knob_svgSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyLNF)
};

#endif // MYLNF_H_INCLUDED
