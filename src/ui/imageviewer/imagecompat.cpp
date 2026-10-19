// SPDX-License-Identifier: GPL-2.0-or-later

#include "imagecompat.h"

#include <QFile>
#include <QImageReader>

#ifdef HAVE_LIBHEIF
#include <libheif/heif.h>
#endif

#include "fqterm_trace.h"

namespace FQTerm {

namespace {

// Reads a portion of a file (the whole file if size < 0).
QByteArray readFileHeader(const QString &fileName, qint64 size) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    return QByteArray();
  }
  if (size < 0) {
    return file.readAll();
  }
  return file.read(size);
}

// HEIF family files are ISO-BMFF containers: bytes 4..7 are "ftyp",
// bytes 8..11 are a brand like heic/heix/hevc/mif1/msf1/avif/heim.
bool isHeifMagic(const QByteArray &header) {
  return header.size() >= 12 && header.mid(4, 4) == "ftyp";
}

#ifdef HAVE_LIBHEIF

// Decodes raw HEIF family bytes into a QImage through libheif.
// Orientation (irot/imir) transformations are applied by libheif itself.
QImage decodeHeifData(const QByteArray &data) {
  if (data.isEmpty()) {
    return QImage();
  }

  heif_context *ctx = heif_context_alloc();
  if (ctx == NULL) {
    return QImage();
  }

  QImage result;

  // Avoid filename encoding issues (e.g. on Windows) by feeding the raw
  // bytes from memory instead of heif_context_read_from_file().
  // heif_context_read_from_memory() makes its own copy of the data.
  heif_error err = heif_context_read_from_memory(
      ctx, data.constData(), data.size(), NULL);

  if (err.code == heif_error_Ok) {
    heif_image_handle *handle = NULL;
    err = heif_context_get_primary_image_handle(ctx, &handle);

    if (err.code == heif_error_Ok && handle != NULL) {
      bool hasAlpha = heif_image_handle_has_alpha_channel(handle);
      heif_colorspace colorspace = heif_colorspace_RGB;
      heif_chroma chroma = hasAlpha ? heif_chroma_interleaved_RGBA
                                    : heif_chroma_interleaved_RGB;

      heif_image *heifImg = NULL;
      heif_error decodeErr = heif_decode_image(handle, &heifImg,
                                               colorspace, chroma, NULL);

      if (decodeErr.code == heif_error_Ok && heifImg != NULL) {
        int stride = 0;
        const uint8_t *plane = heif_image_get_plane_readonly(
            heifImg, heif_channel_interleaved, &stride);
        int width = heif_image_get_width(heifImg, heif_channel_interleaved);
        int height = heif_image_get_height(heifImg, heif_channel_interleaved);

        QImage::Format format =
            hasAlpha ? QImage::Format_RGBA8888 : QImage::Format_RGB888;

        if (plane != NULL && stride > 0 && width > 0 && height > 0) {
          // Deep copy: libheif owns the plane memory, and the row stride
          // does not necessarily match what QImage expects.
          result = QImage(plane, width, height, stride, format).copy();
        }
        heif_image_release(heifImg);
      } else {
        FQ_TRACE("imagecompat", 1) << "libheif decode failed: "
                                   << decodeErr.message;
      }
      heif_image_handle_release(handle);
    } else {
      FQ_TRACE("imagecompat", 1) << "libheif get primary handle failed: "
                                 << err.message;
    }
  } else {
    FQ_TRACE("imagecompat", 1) << "libheif read failed: " << err.message;
  }

  heif_context_free(ctx);
  return result;
}

#endif  // HAVE_LIBHEIF

}  // namespace

bool isHeifByMagic(const QString &fileName) {
  return isHeifMagic(readFileHeader(fileName, 12));
}

QImage loadHeifImage(const QString &fileName) {
#ifdef HAVE_LIBHEIF
  if (!isHeifByMagic(fileName)) {
    return QImage();
  }
  return decodeHeifData(readFileHeader(fileName, -1));
#else
  return QImage();
#endif
}

QImage loadAdaptiveImage(const QString &fileName) {
  QImageReader reader(fileName);
  // Apple images usually carry an EXIF orientation tag; let Qt rotate them.
  reader.setAutoTransform(true);
  QImage image = reader.read();

  if (!image.isNull()) {
    return image;
  }

  FQ_TRACE("imagecompat", 5) << "Qt can't load image, trying libheif: "
                             << fileName << ", reason: "
                             << reader.errorString();

#ifdef HAVE_LIBHEIF
  if (isHeifByMagic(fileName)) {
    return decodeHeifData(readFileHeader(fileName, -1));
  }
#endif

  return QImage();
}

}  // namespace FQTerm
