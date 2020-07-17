#ifndef TITLECOMP_H_INCLUDED
#define TITLECOMP_H_INCLUDED

#include <JuceHeader.h>

class TitleComp : public Component,
                  public SettableTooltipClient
{
public:
    TitleComp();

    enum ColourIDs
    {
        text1ColourID,
        text2ColourID,
    };

    void paint (Graphics& g) override;
    void setStrings (String newTitle, String newSubtitle);

private:
    String title;
    String subtitle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleComp)
};

class TitleItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (TitleItem)

    static const Identifier title;
    static const Identifier subtitle;

    TitleItem (foleys::MagicGUIBuilder& builder, const ValueTree& node);

    void update() override;
    std::vector<foleys::SettableProperty> getSettableProperties() const override;

    Component* getWrappedComponent() override { return &comp; }

private:
    TitleComp comp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleItem)
};

#endif // TITLECOMP_H_INCLUDED
