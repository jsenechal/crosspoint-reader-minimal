#include "MinimalTheme.h"

#include <GfxRenderer.h>

#include <algorithm>
#include <cstdio>

#include "I18n.h"
#include "RecentBooksStore.h"
#include "fontIds.h"
#include "util/BookProgress.h"

namespace {
constexpr int kSidePadding = 28;  // text inset from the screen edge
constexpr int kMarkerWidth = 3;   // selection bar
constexpr int kMarkerGap = 12;    // gap between selection bar and text
}  // namespace

void MinimalTheme::drawRecentBookCover(GfxRenderer& renderer, Rect rect, const std::vector<RecentBook>& recentBooks,
                                       const int selectorIndex, bool& coverRendered, bool& /*coverBufferStored*/,
                                       bool& /*bufferRestored*/, std::function<bool()> /*storeCoverBuffer*/) const {
  if (recentBooks.empty()) {
    const int x = rect.x + kSidePadding + kMarkerWidth + kMarkerGap;
    const int y = rect.y + rect.height / 2 - renderer.getLineHeight(UI_12_FONT_ID) - 2;
    renderer.drawText(UI_12_FONT_ID, x, y, tr(STR_NO_OPEN_BOOK), true, EpdFontFamily::BOLD);
    renderer.drawText(UI_10_FONT_ID, x, y + renderer.getLineHeight(UI_12_FONT_ID) + 4, tr(STR_START_READING), true);
    return;
  }

  const RecentBook& book = recentBooks[0];

  // No cover is drawn or buffered here, so coverRendered only tracks "progress
  // is current for this home screen"; HomeActivity clears it when the card must
  // be refreshed.
  if (!coverRendered || progressPath_ != book.path) {
    progressPercent_ = loadBookProgress(book.path);
    progressPath_ = book.path;
    coverRendered = true;
  }

  const int markerX = rect.x + kSidePadding;
  const int textX = markerX + kMarkerWidth + kMarkerGap;
  const int textWidth = rect.width - (textX - rect.x) - kSidePadding;

  const auto titleLines = renderer.wrappedText(UI_12_FONT_ID, book.title.c_str(), textWidth, 3, EpdFontFamily::BOLD);
  const int titleLineHeight = renderer.getLineHeight(UI_12_FONT_ID);
  const int smallLineHeight = renderer.getLineHeight(UI_10_FONT_ID);
  const bool hasAuthor = !book.author.empty();
  const bool hasProgress = progressPercent_ >= 0;

  const int gap = smallLineHeight / 2;
  int blockHeight = titleLineHeight * static_cast<int>(titleLines.size());
  if (hasAuthor) blockHeight += gap + smallLineHeight;
  if (hasProgress) blockHeight += gap * 2 + smallLineHeight;

  int y = rect.y + (rect.height - blockHeight) / 2;
  const int blockTop = y;

  for (const auto& line : titleLines) {
    renderer.drawText(UI_12_FONT_ID, textX, y, line.c_str(), true, EpdFontFamily::BOLD);
    y += titleLineHeight;
  }
  if (hasAuthor) {
    y += gap;
    const auto author = renderer.truncatedText(UI_10_FONT_ID, book.author.c_str(), textWidth);
    renderer.drawText(UI_10_FONT_ID, textX, y, author.c_str(), true);
    y += smallLineHeight;
  }
  if (hasProgress) {
    y += gap * 2;
    char percent[8];
    snprintf(percent, sizeof(percent), "%d %%", progressPercent_);
    renderer.drawText(UI_10_FONT_ID, textX, y, percent, true);
    y += smallLineHeight;
  }

  if (selectorIndex == 0) {
    renderer.fillRect(markerX, blockTop, kMarkerWidth, y - blockTop, true);
  }
}

void MinimalTheme::drawButtonMenu(GfxRenderer& renderer, Rect rect, int buttonCount, int selectedIndex,
                                  const std::function<std::string(int index)>& buttonLabel,
                                  const std::function<UIIcon(int index)>& /*rowIcon*/) const {
  // Text only: the icon callback is intentionally ignored.
  const int rowHeight = MinimalMetrics::values.menuRowHeight;
  const int rowStep = rowHeight + MinimalMetrics::values.menuSpacing;
  const int lineHeight = renderer.getLineHeight(UI_12_FONT_ID);
  const int markerX = rect.x + kSidePadding;
  const int textX = markerX + kMarkerWidth + kMarkerGap;

  for (int i = 0; i < buttonCount; ++i) {
    const int rowY = rect.y + i * rowStep;
    const int textY = rowY + (rowHeight - lineHeight) / 2;
    const bool selected = selectedIndex == i;
    const std::string label = buttonLabel(i);

    if (selected) {
      renderer.fillRect(markerX, textY, kMarkerWidth, lineHeight, true);
    }
    renderer.drawText(UI_12_FONT_ID, textX, textY, label.c_str(), true,
                      selected ? EpdFontFamily::BOLD : EpdFontFamily::REGULAR);
  }
}
