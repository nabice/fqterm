// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef FQTERM_T2S_H
#define FQTERM_T2S_H

#include <QString>

namespace FQTerm {

// Traditional Chinese -> Simplified Chinese conversion based on OpenCC.
//
// Returns true when the converter is both compiled in (FQTERM_HAVE_OPENCC)
// and successfully initialized (dictionary/config data found at runtime).
bool isT2SAvailable();

// Converts traditional Chinese text to simplified. Every character that is
// not convertible (ASCII, already simplified, unknown) is kept unchanged.
// When OpenCC is unavailable the input is returned unchanged.
QString TraditionalToSimplified(const QString &text);

}  // namespace FQTerm

#endif  // FQTERM_T2S_H
