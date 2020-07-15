#ifndef TITLECOMP_H_INCLUDED
#define TITLECOMP_H_INCLUDED

#include <JuceHeader.h>

class TitleComp : public Component
{
public:
    TitleComp()
    {
        setColour (text1ColourID, Colours::white);
        setColour (text2ColourID, Colours::grey);
    }

    enum ColourIDs
    {
        text1ColourID,
        text2ColourID,
    };

    void paint (Graphics& g) override
    {
        g.setFont (Font ((float) getHeight()).boldened());
        auto font = g.getCurrentFont();
        auto b = getLocalBounds();

        auto drawText = [=, &g, &b] (const String& text)
        {
            auto width = font.getStringWidth (text);
            g.drawFittedText (text, b.removeFromLeft (width), Justification::left, 1);  
        };

        g.setColour (findColour (text1ColourID));
        drawText (String ("CHOW Phaser "));

        g.setColour (findColour (text2ColourID));
        drawText (String ("(Mono)"));
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleComp)
};

class TitleItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (TitleItem)

    TitleItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) :
        foleys::GuiItem (builder, node)
    {
        setColourTranslation ({
            {"text1", TitleComp::text1ColourID},
            {"text2", TitleComp::text2ColourID},
        });

        addAndMakeVisible (comp);
    }

    void update() override
    {
    }

    Component* getWrappedComponent() override
    {
        return &comp;
    }

private:
    TitleComp comp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleItem)
};

#endif // TITLECOMP_H_INCLUDED
