#ifndef TOOLTIPCOMP_H_INCLUDED
#define TOOLTIPCOMP_H_INCLUDED

#include <JuceHeader.h>

class TooltipComponent : public Component,
                         private Timer
{
public:
    TooltipComponent();

    void paint (Graphics& g) override;
    void timerCallback() override;
    String getTipFor (Component& c);

private:
    String name, tip;
    std::atomic_bool showTip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipComponent)
};

class TooltipItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (TooltipItem)

    TooltipItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) :
        foleys::GuiItem (builder, node)
    {
        addAndMakeVisible (tooltipComp);
    }

    void update() override
    {
    }

    Component* getWrappedComponent() override
    {
        return &tooltipComp;
    }

private:
    TooltipComponent tooltipComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipItem)
};

#endif // TOOLTIPCOMP_H_INCLUDED
