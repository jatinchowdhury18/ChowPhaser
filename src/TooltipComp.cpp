#include "TooltipComp.h"

TooltipComponent::TooltipComponent()
{
    name = "NAME";
    tip = "This is a test tip";
    showTip.store (false);
    startTimer (123);
}

void TooltipComponent::paint (Graphics& g)
{
    g.fillAll (Colours::red);

    if (showTip.load())
    {
        auto b = getLocalBounds();

        if (name.isNotEmpty())
        {
            auto width = g.getCurrentFont().getStringWidth (name) + 10;
            g.setColour (Colours::black);
            g.drawFittedText (name + ":", b.removeFromLeft (width), Justification::centred, 1);
        }

        g.setColour (Colours::white);
        g.drawFittedText (tip, b, Justification::left, 1);
    }
}

String TooltipComponent::getTipFor (Component& c)
{
    if (Process::isForegroundProcess()
         && ! ModifierKeys::currentModifiers.isAnyMouseButtonDown())
    {
        if (auto* ttc = dynamic_cast<TooltipClient*> (&c))
            if (! c.isCurrentlyBlockedByAnotherModalComponent())
                return ttc->getTooltip();
    }

    return {};
}

void TooltipComponent::timerCallback()
{
    auto& desktop = Desktop::getInstance();
    auto mouseSource = desktop.getMainMouseSource();

    auto* newComp = mouseSource.isTouch() ? nullptr : mouseSource.getComponentUnderMouse();

    bool needsRepaint = false;
    if (newComp != nullptr)
    {
        auto newTip = getTipFor (*newComp);
        needsRepaint = newTip != tip;

        tip = newTip;
        name = newComp->getName();

        if (! showTip.load())
        {
            showTip.store (true);
            needsRepaint = true;
        }
    }
    else
    {
        if (showTip.load())
        {
            showTip.store (false);
            needsRepaint = true;
        }
    }

    if (needsRepaint)
        repaint();
}
