set(CMAKE_AUTOMOC ON)

set(fqterm_utilities_SRCS
  fqterm_convert.h
  fqterm_http.h
  fqterm_ip_location.h
  fqterm_t2s.h
  fqterm_convert.cpp
  fqterm_http.cpp
  fqterm_ip_location.cpp
  fqterm_t2s.cpp
)

set(fq_SRCS
  fqwcwidth.h
  fqwcwidth.cpp
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../common
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(fqterm_utilities
  ${fqterm_utilities_SRCS}
  ${fq_SRCS}
)

add_dependencies(fqterm_utilities
  fqterm_common
)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

# OpenCC: optional dependency used for Traditional -> Simplified conversion.
find_path(OPENCC_INCLUDE_DIR NAMES opencc/SimpleConverter.hpp)
find_library(OPENCC_LIBRARY NAMES opencc)
if (OPENCC_INCLUDE_DIR AND OPENCC_LIBRARY)
  message(STATUS "OpenCC found: ${OPENCC_LIBRARY} - Traditional-to-Simplified conversion enabled")
  add_definitions(-DFQTERM_HAVE_OPENCC)
  include_directories(${OPENCC_INCLUDE_DIR})
  target_link_libraries(fqterm_utilities Qt5::Core Qt5::Widgets Qt5::Network ${OPENCC_LIBRARY})
else (OPENCC_INCLUDE_DIR AND OPENCC_LIBRARY)
  message(STATUS "OpenCC not found - building without Traditional-to-Simplified conversion")
  target_link_libraries(fqterm_utilities Qt5::Core Qt5::Widgets Qt5::Network)
endif (OPENCC_INCLUDE_DIR AND OPENCC_LIBRARY)
