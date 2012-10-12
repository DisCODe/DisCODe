# =[ DisCODe ]===========================================================
#
# CMake Build Rules for DisCODe
#
# =[ License ]===========================================================
#
# License information
#
# =[ CMake basic usage ]=================================================
#
# Basic Usage:
#
# For more information about CMake, see http://www.cmake.org
#
# =======================================================================

# Project name
project(DisCODe)

# CMake required version must be >= 2.6
cmake_minimum_required(VERSION 2.6)

# Appends the cmake/modules path inside the MAKE_MODULE_PATH variable which stores the
# directories of additional CMake modules (eg MacroOutOfSourceBuild.cmake):
set(CMAKE_MODULE_PATH ${DisCODe_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# The macro below forces the build directory to be different from source directory:
include(MacroOutOfSourceBuild)

macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")


ENABLE_TESTING()



add_subdirectory(src)

add_subdirectory(tests)

add_subdirectory(doc)

add_subdirectory(share)



# =======================================================================
# DisCODeConfig.cmake preparation
# =======================================================================

#  Current version number:
set(DISCODE_VERSION "1.0.0")

string(REGEX MATCHALL "[0-9]" DISCODE_VERSION_PARTS "${DISCODE_VERSION}")

list(GET DISCODE_VERSION_PARTS 0 DISCODE_VERSION_MAJOR)
list(GET DISCODE_VERSION_PARTS 1 DISCODE_VERSION_MINOR)
list(GET DISCODE_VERSION_PARTS 2 DISCODE_VERSION_PATCH)

# include directories
set(CMAKE_INCLUDE_DIRS_CONFIGCMAKE 
	"${CMAKE_INSTALL_PREFIX}/include/Base" 
	"${CMAKE_INSTALL_PREFIX}/include/Common" 
	"${CMAKE_INSTALL_PREFIX}/include/Types" 
	"${CMAKE_INSTALL_PREFIX}/include"
)

# lib directory
set(CMAKE_LIB_DIRS_CONFIGCMAKE "${CMAKE_INSTALL_PREFIX}/lib")


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/DisCODeConfig.cmake.in" "${CMAKE_BINARY_DIR}/DisCODeConfig.cmake" IMMEDIATE @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/DisCODeConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/)


# =======================================================================
# CPack: binary installer generator
# =======================================================================

#
# CPack variables used when generating binary installer
#
set(CPACK_PACKAGE_NAME "DisCODe")
set(CPACK_PACKAGE_VENDOR "DisCODe")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DisCODe - Distributed Component Oriented Data Processing")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "DisCODe")
#set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\doc_src\\\\discode.bmp")

# License file
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

# Tell CPack all of the components to install. The "ALL"
# refers to the fact that this is the set of components that
# will be included when CPack is instructed to put everything
# into the binary installer (the default behavior).
set(CPACK_COMPONENTS_ALL applications sources processors sinks sdk tasks)

# Applications component contains main DisCODe executable and libraries
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "DisCODe Executable")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION "DisCODe - Distributed Component Oriented Data Processing")
set(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime")

# Sources component contains all data sources
set(CPACK_COMPONENT_SOURCES_DISPLAY_NAME "Data sources")
set(CPACK_COMPONENT_SOURCES_DESCRIPTION "Data sources description")
set(CPACK_COMPONENT_SOURCES_GROUP "Runtime")
set(CPACK_COMPONENT_SOURCES_DEPENDS applications)

# Processors component contains all data processors
set(CPACK_COMPONENT_PROCESSORS_DISPLAY_NAME "Data processors")
set(CPACK_COMPONENT_PROCESSORS_DESCRIPTION "Data processors desctiption")
set(CPACK_COMPONENT_PROCESSORS_GROUP "Runtime")
set(CPACK_COMPONENT_PROCESSORS_DEPENDS applications)

# Processors component contains all data sinks
set(CPACK_COMPONENT_SINKS_DISPLAY_NAME "Data sinks")
set(CPACK_COMPONENT_SINKS_DESCRIPTION "Data sinks desctiption")
set(CPACK_COMPONENT_SINKS_GROUP "Runtime")
set(CPACK_COMPONENT_SINKS_DEPENDS applications)

# Headers component contains all files necessary to develop
# additional DisCODe modules (data sources and processors)
set(CPACK_COMPONENT_SDK_DISPLAY_NAME "Component SDK")
set(CPACK_COMPONENT_SDK_DESCRIPTION "All files necessary to develop additional components (headers and libraries)")
#set(CPACK_COMPONENT_SDK_GROUP "Dev")
set(CPACK_COMPONENT_SDK_DEPENDS applications)

# Headers component contains all files necessary to develop
# additional DisCODe modules (data sources and processors)
set(CPACK_COMPONENT_TASKS_DISPLAY_NAME "Task config files")
set(CPACK_COMPONENT_TASKS_DESCRIPTION "Example tasks")
#set(CPACK_COMPONENT_TASKS_GROUP "Tasks")

# This must always be last!
include(CPack)
