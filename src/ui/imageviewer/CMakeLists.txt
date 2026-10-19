set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(IMAGE_USE_PICFLOW)
  set(IMGVIEW_SRCS
    imageviewer.h
    imageviewer.cpp)
else(IMAGE_USE_PICFLOW)
  set(IMGVIEW_SRCS
    imageviewer_origin.cpp
    imageviewer_origin.h)
endif(IMAGE_USE_PICFLOW)

set(fqterm_imageviewer_SRCS
  fqtermimage.cpp
  fqtermimage.h
  fqterm_canvas.cpp
  fqterm_canvas.h
  pictureflow.cpp
  pictureflow.h
  imagecompat.cpp
  imagecompat.h
  ${IMGVIEW_SRCS})

# Optional libheif support: enables decoding of HEIC/HEIF/AVIF images
# (e.g. Apple photos renamed to .jpg when uploaded to BBS).
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(LIBHEIF QUIET libheif)
endif()
if(NOT LIBHEIF_FOUND)
  find_path(LIBHEIF_INCLUDE_DIR libheif/heif.h)
  find_library(LIBHEIF_LIBRARY NAMES heif libheif)
  if(LIBHEIF_INCLUDE_DIR AND LIBHEIF_LIBRARY)
    set(LIBHEIF_INCLUDE_DIRS ${LIBHEIF_INCLUDE_DIR})
    set(LIBHEIF_LIBRARIES ${LIBHEIF_LIBRARY})
    set(LIBHEIF_FOUND TRUE)
  endif()
endif()

include_directories(
  ${QT_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR}
  ${QT_QTGUI_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../common
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(fqterm_imageviewer
  ${fqterm_imageviewer_SRCS})

target_link_libraries(fqterm_imageviewer Qt5::Core Qt5::Widgets)

if(LIBHEIF_FOUND)
  message(STATUS "libheif found: HEIC/HEIF/AVIF image decoding enabled")
  target_compile_definitions(fqterm_imageviewer PRIVATE HAVE_LIBHEIF)
  target_include_directories(fqterm_imageviewer PRIVATE ${LIBHEIF_INCLUDE_DIRS})
  target_link_libraries(fqterm_imageviewer ${LIBHEIF_LIBRARIES})
else()
  message(STATUS "libheif not found: building WITHOUT HEIC/HEIF/AVIF image decoding")
endif()
