/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Demos/JUCEDemos.h"

struct DemoContent;
struct CodeContent;

//==============================================================================
class DemoContentComponent final : public TabbedComponent
{
public:
    DemoContentComponent (Component& mainComponent, std::function<void (bool)> demoChangedCallback);
    ~DemoContentComponent() override;

    void resized() override;

    void setDemo (const String& category, int selectedDemoIndex);
    void clearCurrentDemo();
    int getCurrentDemoIndex() const noexcept      { return currentDemoIndex; }

    bool isShowingHomeScreen() const noexcept;
    void showHomeScreen();

    void setTabBarIndent (int indent) noexcept    { tabBarIndent = indent; }

private:
    std::function<void (bool)> demoChangedCallback;

    std::unique_ptr<DemoContent> demoContent;

   #if ! (JUCE_ANDROID || JUCE_IOS)
    std::unique_ptr<CodeContent> codeContent;
   #endif

    String currentDemoCategory;
    int currentDemoIndex = -1;
    int tabBarIndent = 0;

    //==============================================================================
    void updateLookAndFeel();
    void lookAndFeelChanged() override;

    String trimPIP (const String& fileContents);
    void ensureDemoIsShowing();
};
