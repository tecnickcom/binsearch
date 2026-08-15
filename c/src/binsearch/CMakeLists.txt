# binsearch is a header-only library, so it is modelled as an INTERFACE target:
# it carries the include directory and nothing is compiled or linked.
add_library(binsearch INTERFACE)

target_include_directories(binsearch INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(FILES binsearch.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tecnickcom/binsearch
)
