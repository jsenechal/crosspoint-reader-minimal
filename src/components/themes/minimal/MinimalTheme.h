#pragma once

#include <string>

#include "components/themes/BaseTheme.h"

class GfxRenderer;

// Minimal theme metrics: Classic geometry with flat, borderless lists, a
// left-aligned header without rule, and a roomy home screen.
namespace MinimalMetrics {
constexpr ThemeMetrics values = [] {
  ThemeMetrics m = BaseMetrics::values;
  m.verticalSpacing = 16;
  m.listRowHeight = 44;
  m.listWithSubtitleRowHeight = 62;
  m.listInset = 20;
  m.listSidePadding = 8;
  m.listSelectionStyle = 2;  // underline
  m.headerUnderlineSize = 0;
  m.headerTitleAlign = 0;  // left
  m.menuRowHeight = 56;
  m.menuSpacing = 0;
  m.homeTopPadding = 40;
  // The home card shows no cover, but HomeActivity still generates a thumb at
  // this height; keep it tiny so that costs next to nothing.
  m.homeCoverHeight = 40;
  m.homeCoverTileHeight = 200;
  m.homeMenuTopOffset = 16;
  return m;
}();
}  // namespace MinimalMetrics

class MinimalTheme : public BaseTheme {
 public:
  void drawRecentBookCover(GfxRenderer& renderer, Rect rect, const std::vector<RecentBook>& recentBooks,
                           const int selectorIndex, bool& coverRendered, bool& coverBufferStored, bool& bufferRestored,
                           std::function<bool()> storeCoverBuffer) const override;
  void drawButtonMenu(GfxRenderer& renderer, Rect rect, int buttonCount, int selectedIndex,
                      const std::function<std::string(int index)>& buttonLabel,
                      const std::function<UIIcon(int index)>& rowIcon) const override;

 private:
  // Saved progress of the featured book, reloaded only when the home screen
  // marks its card dirty (coverRendered == false) or the book changes, since
  // loading it opens the book's cache on the SD card.
  mutable std::string progressPath_;
  mutable int progressPercent_ = -1;
};
