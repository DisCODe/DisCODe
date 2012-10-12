# Checking compiler flags
include(CheckCXXCompilerFlag)

# Define place for all created executables and libraries
# Disabled. All binaries are created in their folders.
# Use make install to put everything in proper place
#SET(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/lib)
#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Required on Windows
if(WIN32)
	SET(DLFCN_WIN32 "dlfcn" CACHE PATH "dlfcn-win32 library base folder")
	INCLUDE_DIRECTORIES(${DLFCN_WIN32}/include)
	LINK_DIRECTORIES(${DLFCN_WIN32}/lib)
endif(WIN32)

SET(Boost_DEBUG FALSE)

# Find all required packages
FIND_PACKAGE( Boost 1.41.0 REQUIRED thread filesystem system regex program_options)

# Make makefiles more verbose
SET(CMAKE_VERBOSE_MAKEFILE OFF)

# Set libraries linked with project
SET(DisCODe_LIBRARIES
    ${Boost_THREAD_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

# Specify include directories for the build
include_directories( . Base Core Common Types)

# Try to include Boost as system directory to suppress it's warnings
include_directories( SYSTEM ${Boost_INCLUDE_DIR})

# Specify directories in which the linker will look for libraries
LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )

IF(CMAKE_COMPILER_IS_GNUCXX)

	# Add compiler directives
	add_definitions(-Wall)
	add_definitions(-Wextra)
	add_definitions(-Werror=return-type)
	add_definitions(-pedantic)

	# Checking if compiler supports C++0x standard
	CHECK_CXX_COMPILER_FLAG(-std=c++0x CPP0x)

	# If C++0x is supported then use it
	if(CPP0x)
	    MESSAGE(STATUS "c++0x supported")
	    add_definitions(-std=c++0x)
	else(CPP0x)
	    MESSAGE(STATUS "c++0x NOT supported")
	endif(CPP0x)
ELSEIF(CMAKE_CXX_COMPILER MATCHES ".*icpc")
	add_definitions(-Wall)
	# disable remark #981: operands are evaluated in unspecified order
	add_definitions(-wd981)
ENDIF()

set(WITH_UNIFIED_TIMER OFF CACHE BOOL "Use unified timer (based on ctime)")

IF(WIN32)
    SET(DisCODe_LIBRARIES ${DisCODe_LIBRARIES} dl)
    ADD_DEFINITIONS(-DWIN32)
ELSE(WIN32)
    SET(DisCODe_LIBRARIES ${DisCODe_LIBRARIES} rt dl)
ENDIF(WIN32)

IF(WITH_UNIFIED_TIMER)
    add_definitions(-DUNIFIED_TIMER)
ENDIF(WITH_UNIFIED_TIMER)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -DBOOST_FILESYSTEM_VERSION=2")

IF(WIN32)
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--enable-auto-import")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-auto-import")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")
ELSE(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(WIN32)

# Create Common library
add_subdirectory(Common)
# Use created library
SET(DisCODe_LIBRARIES ${DisCODe_LIBRARIES} Common)

add_subdirectory(Base)
SET(DisCODe_LIBRARIES ${DisCODe_LIBRARIES} Base)

SET(DisCODe_Core_LIBRARIES ${DisCODe_LIBRARIES} Network)

# Add subdirectories to the build - CMake will run CMakeFiles from them
add_subdirectory(Core)

add_subdirectory(Client)

# User interface
set(WITH_GUI ON CACHE BOOL "Build GUI")

IF (WITH_GUI)
    MESSAGE(STATUS "Building GUI")
    add_subdirectory(GUI)
ENDIF(WITH_GUI)

# Small utility programs
set(WITH_TOOLS ON CACHE BOOL "Build tools")

IF (WITH_TOOLS)
    MESSAGE(STATUS "Building tools")
    add_subdirectory(Tools)
ENDIF(WITH_TOOLS)

