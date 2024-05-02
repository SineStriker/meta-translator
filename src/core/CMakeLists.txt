project(mtccore
    VERSION ${MTC_VERSION}
    LANGUAGES CXX
)

file(GLOB_RECURSE _src *.h *.cpp)
mtc_add_library(${PROJECT_NAME} STATIC
    SOURCES ${_src}
    FEATURES cxx_std_17
    LINKS
    INCLUDE_PRIVATE *
    PREFIX MTC_CORE
)