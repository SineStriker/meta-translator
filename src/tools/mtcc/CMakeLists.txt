project(mtcc
    VERSION ${MTC_VERSION}
    LANGUAGES CXX
)

add_executable(${PROJECT_NAME})

file(GLOB _src *.h *.cpp)
qm_configure_target(${PROJECT_NAME}
    SOURCES ${_src}
    QT_LINKS Core
    LINKS
    DEFINES APP_VERSION="${PROJECT_VERSION}"
    FEATURES cxx_std_17
)

qm_add_win_rc(${PROJECT_NAME}
    NAME ${MTC_INSTALL_NAME}
    DESCRIPTION "Meta Translator Code Compiler"
    COPYRIGHT ${MTC_PROJECT_COPYRIGHT}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE
)

if(MTC_INSTALL)
    mtc_install_tool(${PROJECT_NAME})
endif()