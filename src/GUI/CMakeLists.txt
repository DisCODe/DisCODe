# Source files
SET(GUI_CXX_SRCS
    main.cpp
    MainWindow.cpp
	ComponentWidget.cpp
	HostSelector.cpp
	WelcomePage.cpp
	TaskWidget.cpp
	ComponentItem.cpp
	Connecting.cpp
	TextRoller.cpp
	SubtaskItem.cpp
)

# Class headers that have to be MOCed (i.e. classes derived from QObject)
SET(GUI_MOC_SRCS
    MainWindow.hpp
	ComponentWidget.hpp
	HostSelector.hpp
	WelcomePage.hpp
	TaskWidget.hpp
	ComponentItem.hpp
	Connecting.hpp
	TextRoller.hpp
	SubtaskItem.hpp
)

# UI files
SET(GUI_UI
	MainWindow.ui
	HostSelector.ui
	WelcomePage.ui
	TaskWidget.ui
	ComponentItem.ui
	Connecting.ui
	SubtaskItem.ui
)

# Resource files
SET(GUI_QRC
	gui.qrc
)

# ############################################
# Try to find and setup Qt4 for this project
# ############################################
FIND_PACKAGE(Qt4)

IF(QT_FOUND)

MESSAGE(STATUS "\tUsing Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH} (${QT_QMAKE_EXECUTABLE})")
MESSAGE(STATUS "\tIncludes in ${QT_INCLUDE_DIR}")

# add Qt modules here, the include will setup QT_LIBRARIES
#
# SET(QT_USE_XXX TRUE)
#
# Possible values are:
# QT_DONT_USE_QTCORE
# QT_DONT_USE_QTGUI
# QT_USE_QT3SUPPORT
# QT_USE_QTASSISTANT
# QT_USE_QAXCONTAINER
# QT_USE_QAXSERVER
# QT_USE_QTDESIGNER
# QT_USE_QTMOTIF
# QT_USE_QTMAIN
# QT_USE_QTNETWORK
# QT_USE_QTNSPLUGIN
# QT_USE_QTOPENGL
# QT_USE_QTSQL
# QT_USE_QTXML
# QT_USE_QTSVG
# QT_USE_QTTEST
# QT_USE_QTUITOOLS
# QT_USE_QTDBUS
# QT_USE_QTSCRIPT
# QT_USE_QTASSISTANTCLIENT
# QT_USE_QTHELP
# QT_USE_QTWEBKIT
# QT_USE_QTXMLPATTERNS
# QT_USE_PHONON

INCLUDE(${QT_USE_FILE})

# Include current directory, necessary for proper UI headers to work
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Generate MOC files
QT4_WRAP_CPP(GUI_CXX_SRCS ${GUI_MOC_SRCS})

# Generate UI include files
QT4_WRAP_UI(GUI_CXX_SRCS ${GUI_UI})

QT4_ADD_RESOURCES(GUI_CXX_SRCS ${GUI_QRC})

# Create GUI executable
ADD_EXECUTABLE(discode_gui ${GUI_CXX_SRCS})

# Link executable with QT libraries
TARGET_LINK_LIBRARIES(discode_gui ${QT_LIBRARIES} DisCODeClient Network)


SET_TARGET_PROPERTIES(discode_gui PROPERTIES
   BUILD_WITH_INSTALL_RPATH ON
   INSTALL_RPATH_USE_LINK_PATH 1
   INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
)

install(
  TARGETS discode_gui
  RUNTIME DESTINATION bin COMPONENT applications
)


ELSE(QT_FOUND)

# ##########################
# Find Qt 5
# ##########################
FIND_PACKAGE(Qt5Widgets)

IF(Qt5Widgets_FOUND)

SET(CMAKE_AUTOMOC ON)
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})

QT5_WRAP_UI(GUI_UI_HEADERS ${GUI_UI})
QT5_ADD_RESOURCES(GUI_QRC_SOURCES ${GUI_QRC})

ADD_EXECUTABLE(discode_gui
	${GUI_CXX_SRCS}
#	${GUI_MOC_SRCS}
	${GUI_UI_HEADERS}
	${GUI_QRC_SOURCES}
)

# Link executable with QT libraries
TARGET_LINK_LIBRARIES(discode_gui ${Qt5Widgets_LIBRARIES} DisCODeClient Network)
#QT5_USE_MODULES(discode_gui Widgets)


SET_TARGET_PROPERTIES(discode_gui PROPERTIES
   BUILD_WITH_INSTALL_RPATH ON
   INSTALL_RPATH_USE_LINK_PATH 1
   INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
)

install(
  TARGETS discode_gui
  RUNTIME DESTINATION bin COMPONENT applications
)


ELSE(Qt5Widgets_FOUND)

    MESSAGE("Qt not found! GUI will not be built")

ENDIF(Qt5Widgets_FOUND)

ENDIF(QT_FOUND)
