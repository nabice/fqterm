// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef FQTERM_IMAGE_COMPAT_H
#define FQTERM_IMAGE_COMPAT_H

#include <QImage>
#include <QString>

namespace FQTerm {

// Fallback image decoding for formats that Qt does not support natively,
// e.g. HEIC/HEIF/AVIF files (Apple photos) that are often renamed to .jpg
// when uploaded to BBS.
//
// The strategy is:
//   1. Try QImage::load() first (Qt decides the real format by magic bytes,
//      so a renamed .jpg/.png is decoded correctly as usual).
//   2. If that fails, sniff the real format from the file header and decode
//      it with libheif when the file is HEIF family (heic/heif/avif).
//
// When built without libheif, step 2 simply returns a null image.

// True if the file content looks like a HEIF container (heic/heif/avif),
// regardless of its file name extension.
bool isHeifByMagic(const QString &fileName);

// Load an image from file, falling back to libheif for Qt-unsupported
// formats.  Returns a null image if the file cannot be decoded.
QImage loadAdaptiveImage(const QString &fileName);

// Load an image with libheif directly.  Returns a null image on failure or
// when fqterm is built without libheif support.
QImage loadHeifImage(const QString &fileName);

}  // namespace FQTerm

#endif  // FQTERM_IMAGE_COMPAT_H
