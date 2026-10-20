// SPDX-License-Identifier: GPL-2.0-or-later

#include "fqterm_t2s.h"

#ifdef FQTERM_HAVE_OPENCC
#include <opencc/SimpleConverter.hpp>
#endif

namespace FQTerm {

namespace {

#ifdef FQTERM_HAVE_OPENCC

// "tw2sp.json" converts Traditional Chinese (Taiwan standard) to Simplified
// Chinese with phrase-level conversion (e.g. 滑鼠->鼠标, 軟體->软件,
// 伺服器->服务器), which reads best for Taiwan BBS sites such as PTT.
// Use "t2s.json" instead for plain character-level conversion.
const char kT2SConfig[] = "tw2sp.json";

// Lazy-initialized OpenCC converter. The static-local instance() is
// thread-safe (C++11 magic statics); SimpleConverter::Convert() is const and
// safe for concurrent use.
class T2SConverter {
 public:
  static T2SConverter &instance() {
    static T2SConverter converter;
    return converter;
  }

  bool isValid() const { return valid_; }

  QString convert(const QString &text) const {
    if (!valid_ || text.isEmpty()) {
      return text;
    }
    try {
      const QByteArray utf8 = text.toUtf8();
      return QString::fromUtf8(converter_->Convert(utf8.constData()).c_str());
    } catch (...) {
      // Never let conversion errors break the terminal display.
      return text;
    }
  }

 private:
  T2SConverter() {
    try {
      converter_ = new opencc::SimpleConverter(kT2SConfig);
      valid_ = true;
    } catch (...) {
      // Missing config/data files (e.g. libopencc-data not installed).
      valid_ = false;
    }
  }

  ~T2SConverter() {
    delete converter_;
  }

  T2SConverter(const T2SConverter &);
  T2SConverter &operator=(const T2SConverter &);

  opencc::SimpleConverter *converter_ = nullptr;
  bool valid_ = false;
};

#endif  // FQTERM_HAVE_OPENCC

}  // namespace

bool isT2SAvailable() {
#ifdef FQTERM_HAVE_OPENCC
  return T2SConverter::instance().isValid();
#else
  return false;
#endif
}

QString TraditionalToSimplified(const QString &text) {
#ifdef FQTERM_HAVE_OPENCC
  return T2SConverter::instance().convert(text);
#else
  return text;
#endif
}

}  // namespace FQTerm
