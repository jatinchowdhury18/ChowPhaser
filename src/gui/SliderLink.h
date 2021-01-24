#ifndef SLIDERLINK_H_INCLUDED
#define SLIDERLINK_H_INCLUDED

#include <JuceHeader.h>

namespace foleys
{
/** Copied from foleys::SliderItem class with shift key link to another parameter */
class SliderLink : public GuiItem,
                   private juce::Slider::Listener
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (SliderLink)

    static const juce::Identifier pSliderType;
    static const juce::StringArray pSliderTypes;

    static const juce::Identifier pSliderTextBox;
    static const juce::StringArray pTextBoxPositions;

    static const juce::Identifier pValue;
    static const juce::Identifier pMinValue;
    static const juce::Identifier pMaxValue;

    // @JATIN change
    static const juce::Identifier linkParam;

    SliderLink (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
    {
        setColourTranslation (
            { { "slider-background", juce::Slider::backgroundColourId },
              { "slider-thumb", juce::Slider::thumbColourId },
              { "slider-track", juce::Slider::trackColourId },
              { "rotary-fill", juce::Slider::rotarySliderFillColourId },
              { "rotary-outline", juce::Slider::rotarySliderOutlineColourId },
              { "slider-text", juce::Slider::textBoxTextColourId },
              { "slider-text-background", juce::Slider::textBoxBackgroundColourId },
              { "slider-text-highlight", juce::Slider::textBoxHighlightColourId },
              { "slider-text-outline", juce::Slider::textBoxOutlineColourId } });

        addAndMakeVisible (slider);

        // @JATIN change
        slider.addListener (this);
        slider.addMouseListener (this, false);
        linkFlag.store (false);
        linkGesture.store (false);
    }

    virtual ~SliderLink()
    {
        if (linkGesture.load())
            linkParamHandle->endChangeGesture();
    }

    void update() override
    {
        attachment.reset();

        auto type = getProperty (pSliderType).toString();
        slider.setAutoOrientation (type.isEmpty() || type == pSliderTypes[0]);

        if (type == pSliderTypes[1])
            slider.setSliderStyle (juce::Slider::LinearHorizontal);
        else if (type == pSliderTypes[2])
            slider.setSliderStyle (juce::Slider::LinearVertical);
        else if (type == pSliderTypes[3])
            slider.setSliderStyle (juce::Slider::Rotary);
        else if (type == pSliderTypes[4])
            slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        else if (type == pSliderTypes[5])
            slider.setSliderStyle (juce::Slider::IncDecButtons);

        auto textbox = getProperty (pSliderTextBox).toString();
        if (textbox == pTextBoxPositions[0])
            slider.setTextBoxStyle (juce::Slider::NoTextBox, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else if (textbox == pTextBoxPositions[1])
            slider.setTextBoxStyle (juce::Slider::TextBoxAbove, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else if (textbox == pTextBoxPositions[3])
            slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else if (textbox == pTextBoxPositions[4])
            slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());
        else
            slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, slider.getTextBoxWidth(), slider.getTextBoxHeight());

        double minValue = getProperty (pMinValue);
        double maxValue = getProperty (pMaxValue);
        if (maxValue > minValue)
            slider.setRange (minValue, maxValue);

        auto valueID = configNode.getProperty (pValue, juce::String()).toString();
        if (valueID.isNotEmpty())
            slider.getValueObject().referTo (getMagicState().getPropertyAsValue (valueID));

        paramID = configNode.getProperty (IDs::parameter, juce::String()).toString();
        if (paramID.isNotEmpty())
            attachment = getMagicState().createAttachment (paramID, slider);

        // @JATIN changes
        linkParamHandle = getMagicState().getParameter (getProperty (linkParam).toString());
        slider.setNumDecimalPlacesToDisplay (2);
    }

    std::vector<SettableProperty> getSettableProperties() const override
    {
        std::vector<SettableProperty> properties;

        properties.push_back ({ configNode, IDs::parameter, SettableProperty::Choice, {}, magicBuilder.createParameterMenu() });
        properties.push_back ({ configNode, pSliderType, SettableProperty::Choice, pSliderTypes[0], makePopupMenu (pSliderTypes) });
        properties.push_back ({ configNode, pSliderTextBox, SettableProperty::Choice, pTextBoxPositions[2], makePopupMenu (pTextBoxPositions) });
        properties.push_back ({ configNode, pValue, SettableProperty::Property, 1.0f, {} });
        properties.push_back ({ configNode, pMinValue, SettableProperty::Number, 0.0f, {} });
        properties.push_back ({ configNode, pMaxValue, SettableProperty::Number, 2.0f, {} });

        // @JATIN change
        properties.push_back ({ configNode, linkParam, SettableProperty::Choice, {}, magicBuilder.createParameterMenu() });

        return properties;
    }

    juce::Component* getWrappedComponent() override
    {
        return &slider;
    }

    // @JATIN changes start
    bool isLinked (juce::Slider* s) const
    {
        return s == &slider && linkParamHandle != nullptr
               && paramID.isNotEmpty() && linkFlag.load();
    }

    void sliderValueChanged (juce::Slider* s) override
    {
        if (! isLinked (s))
            return;

        linkParamHandle->setValueNotifyingHost (getMagicState().getParameter (paramID)->getValue());
    }

    void sliderDragStarted (juce::Slider* s) override
    {
        if (! isLinked (s))
            return;

        if (! linkGesture.load())
        {
            linkParamHandle->beginChangeGesture();
            linkGesture.store (true);
        }
    }

    void sliderDragEnded (juce::Slider* s) override
    {
        if (! isLinked (s))
            return;

        if (linkGesture.load())
        {
            linkParamHandle->endChangeGesture();
            linkGesture.store (false);
        }
    }

    void setLinkFlag (const MouseEvent& e)
    {
        if (e.mods.isShiftDown())
            linkFlag.store (true);
        else
            linkFlag.store (false);
    }

    void mouseDown (const MouseEvent& e) override { setLinkFlag (e); }
    void mouseDrag (const MouseEvent& e) override { setLinkFlag (e); }
    // @JATIN changes end

private:
    AutoOrientationSlider slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    // @JATIN changes start
    juce::String paramID;
    std::atomic_bool linkFlag;
    std::atomic_bool linkGesture;
    juce::AudioProcessorParameter* linkParamHandle = nullptr;
    // @JATIN changes end

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderLink)
};
const juce::Identifier SliderLink::pSliderType { "slider-type" };
const juce::StringArray SliderLink::pSliderTypes { "auto", "linear-horizontal", "linear-vertical", "rotary", "rotary-horizontal-vertical", "inc-dec-buttons" };
const juce::Identifier SliderLink::pSliderTextBox { "slider-textbox" };
const juce::StringArray SliderLink::pTextBoxPositions { "no-textbox", "textbox-above", "textbox-below", "textbox-left", "textbox-right" };
const juce::Identifier SliderLink::pValue { "value" };
const juce::Identifier SliderLink::pMinValue { "min-value" };
const juce::Identifier SliderLink::pMaxValue { "max-value" };

// @JATIN change
const juce::Identifier SliderLink::linkParam { "link-param" };

} // namespace foleys

#endif // SLIDERLINK_H_INCLUDED
