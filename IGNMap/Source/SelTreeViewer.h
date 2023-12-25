//==============================================================================
//    SelTreeViewer.h
//
//    Created: 14 Mar 2022 3:50:54pm
//    Author:  FBecirspahic
//==============================================================================

#pragma once

#include <JuceHeader.h>

class XGeoBase;
class XGeoVector;

class SelTreeItem : public juce::TreeViewItem {
public:
  SelTreeItem() { m_Base = nullptr; m_Feature = nullptr; }
  SelTreeItem(XGeoBase* base, XGeoVector* feature) { m_Base = base; m_Feature = feature; }
  SelTreeItem(std::string name, std::string value) { m_Base = nullptr; m_Feature = nullptr; m_AttName = name; m_AttValue = value; }

  void SetBase(XGeoBase* base) { clearSubItems(); m_Base = base; setOpen(true); }

  bool mightContainSubItems() override { if (m_Base != nullptr) return true; return false; }
  void itemOpennessChanged(bool isNowOpen) override;
  void paintItem(juce::Graphics& g, int width, int height) override;
  void itemClicked(const juce::MouseEvent&) override;
  void itemDoubleClicked(const juce::MouseEvent&) override;
  int getItemWidth() const override;
  juce::String getTooltip() override;

private:
  XGeoBase* m_Base;
  XGeoVector* m_Feature;
  juce::String m_AttName;
  juce::String m_AttValue;
  static int const m_Margin = 100;
};

class SelTreeViewer : public juce::TreeView, public juce::ActionBroadcaster {
public:
  SelTreeViewer();
  ~SelTreeViewer() override { setRootItem(nullptr); }

  void SetBase(XGeoBase* base);

private:
  std::unique_ptr<SelTreeItem> m_rootItem;
  juce::TooltipWindow m_toolTipWindow;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelTreeViewer)
};
