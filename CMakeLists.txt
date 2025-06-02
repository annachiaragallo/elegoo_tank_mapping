#
#

cmake_minimum_required(VERSION 3.22.0)

find_package(PkgConfig REQUIRED)

pkg_search_module(GATTLIB REQUIRED gattlib)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(PCRE REQUIRED libpcre)

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(include)
set(elegoo_map_SRCS src/main.c src/draw.c src/ble_connect.c src/elegoo_tank.c)

add_executable(elegoo_main ${elegoo_map_SRCS})
target_link_libraries(elegoo_main ${GATTLIB_LIBRARIES} ${GATTLIB_LDFLAGS}  ${GLIB_LDFLAGS} ${PCRE_LIBRARIES} m pthread)
