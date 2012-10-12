# =[ DisCODeMacros.cmake ]======================================================
#
# Additional macro definitions for use in DCL.
#
# =[ Author ]===================================================================
#
# Maciej Stefańczyk
#
# ==============================================================================


# ==============================================================================
# Ensures that we do an out of source build
# ==============================================================================

MACRO(MACRO_ENSURE_OUT_OF_SOURCE_BUILD MSG)

    STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource)
    GET_FILENAME_COMPONENT(PARENTDIR ${CMAKE_SOURCE_DIR} PATH)
    STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PARENTDIR}" insourcesubdir)
    
    IF(insource OR insourcesubdir)
        MESSAGE(FATAL_ERROR "${MSG}")
    ENDIF(insource OR insourcesubdir)
    
ENDMACRO(MACRO_ENSURE_OUT_OF_SOURCE_BUILD)


# ==============================================================================
# Search for DCL, check if it's compiled and include proper directories
# (if it's necessary, i.e. when DCL exports something).
# ==============================================================================
MACRO(DISCODE_FIND_DCL DCL_NAME)

    # check, if module is already found
    IF( DISCODE_${DCL_NAME}_FOUND )
    
    ELSE( DISCODE_${DCL_NAME}_FOUND )

        IF (NOT DEFINED DISCODE_DCL_DIR)
            MESSAGE(STATUS "Looking for DISCODE_DCL_DIR...")
            SET(DISCODE_DCL_DIR $ENV{DISCODE_DCL_DIR} CACHE INTERNAL "DisCODe DCL directory")
        ENDIF (NOT DEFINED DISCODE_DCL_DIR)
        
        # if DISCODE_DCL_DIR is not set
        IF( DISCODE_DCL_DIR STREQUAL "" OR NOT DEFINED DISCODE_DCL_DIR)
            UNSET( DISCODE_DCL_DIR )
            MESSAGE(FATAL_ERROR "DISCODE_DCL_DIR not set!")
        ELSE( DISCODE_DCL_DIR STREQUAL "" OR NOT DEFINED DISCODE_DCL_DIR)
            MESSAGE(STATUS "DISCODE_DCL_DIR=[${DISCODE_DCL_DIR}]")
            MESSAGE(STATUS "Looking for ${DCL_NAME} DCL...")
    
            # check, if there is proper directory in DCL folder
            IF(EXISTS ${DISCODE_DCL_DIR}/${DCL_NAME})
            
                MESSAGE(STATUS "${DCL_NAME} found.")
                SET( DISCODE_${DCL_NAME}_FOUND 1 CACHE INTERNAL "${DCL_NAME} already found")
            
            ENDIF(EXISTS ${DISCODE_DCL_DIR}/${DCL_NAME})
    
        ENDIF( DISCODE_DCL_DIR STREQUAL "" OR NOT DEFINED DISCODE_DCL_DIR)
    
    ENDIF( DISCODE_${DCL_NAME}_FOUND )
    
    # second check, if found then add apropriate include dirs
    IF( DISCODE_${DCL_NAME}_FOUND )
        # check, if config file exists
        IF (EXISTS ${DISCODE_DCL_DIR}/${DCL_NAME}/dist/${DCL_NAME}Config.cmake)
            # include it if it exists
            INCLUDE(${DISCODE_DCL_DIR}/${DCL_NAME}/dist/${DCL_NAME}Config.cmake)
        ELSE (EXISTS ${DISCODE_DCL_DIR}/${DCL_NAME}/dist/${DCL_NAME}Config.cmake)
            # print error message and stop compilation
            MESSAGE( FATAL_ERROR "${DCL_NAME} DCL found, but ${DCL_NAME}Config.cmake not found" )
        ENDIF (EXISTS ${DISCODE_DCL_DIR}/${DCL_NAME}/dist/${DCL_NAME}Config.cmake)
    ELSE( DISCODE_${DCL_NAME}_FOUND )
        MESSAGE(FATAL_ERROR "Couldn't find ${DCL_NAME} DCL!")
    ENDIF( DISCODE_${DCL_NAME}_FOUND )

ENDMACRO(DISCODE_FIND_DCL DCL_NAME)

# ==============================================================================
# Add variable controlling whether to build given corresponding component or not
# and - if set to TRUE - include component's subdirectory
#
# Optionally you can give additional argument to specify platform on which
# component can be built (use WIN32 for Windows-only and !WIN32 for *nix-only)
# ==============================================================================
MACRO(ADD_COMPONENT COMP_NAME)
	if ( ${ARGC} LESS 2 )
		set( ARGV1 ${CMAKE_SYSTEM_NAME} )
	endif (${ARGC} LESS 2 )

	if ( ${CMAKE_SYSTEM_NAME} MATCHES ${ARGV1} )
		set(COMPONENT_${COMP_NAME} ON CACHE BOOL "Build ${COMP_NAME} component")
		if (COMPONENT_${COMP_NAME})
			MESSAGE(STATUS "\t${COMP_NAME}")
			ADD_SUBDIRECTORY(${COMP_NAME})
		endif(COMPONENT_${COMP_NAME})
	endif( ${CMAKE_SYSTEM_NAME} MATCHES ${ARGV1} )
ENDMACRO(ADD_COMPONENT)

# ==============================================================================
# Set component's install directory and group
# ==============================================================================
SET(MORELIBDIRS ${DisCODe_DIR}/lib ${CMAKE_INSTALL_PREFIX}/lib ${ADDITIONAL_LIB_DIRS})
MESSAGE(STATUS "${MORELIBDIRS}")
SET(LIBDIRS ${ADDITIONAL_LIB_DIRS} ${CMAKE_INSTALL_PREFIX}/lib)
MACRO(INSTALL_COMPONENT COMP_NAME)
	SET_TARGET_PROPERTIES(${COMP_NAME} PROPERTIES
		BUILD_WITH_INSTALL_RPATH TRUE
		INSTALL_RPATH_USE_LINK_PATH TRUE
		INSTALL_RPATH "${MORELIBDIRS}"
	)

	install(
	    TARGETS ${COMP_NAME}
		RUNTIME DESTINATION bin COMPONENT components
		LIBRARY DESTINATION lib COMPONENT components
		ARCHIVE DESTINATION lib
	)
ENDMACRO(INSTALL_COMPONENT)

# ==============================================================================
# Install additional headers
# ==============================================================================
MACRO(INSTALL_HEADERS HEADERS_DIR HEADERS_LIST)

    MESSAGE(STATUS "${HEADERS_LIST}")
    install(
        FILES ${HEADERS_LIST}
        DESTINATION include/${HEADERS_DIR}
        COMPONENT sdk
    )
        
ENDMACRO(INSTALL_HEADERS)