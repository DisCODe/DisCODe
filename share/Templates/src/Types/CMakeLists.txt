SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# If DCL provides any additional libraries - add them here

# # Get soource files of library 
# FILE(GLOB lib_src *.cpp)
# ADD_LIBRARY(TEMPLATE_ARG_DCL_NAMETypes SHARED ${lib_src})
# # Link with other libraries
# TARGET_LINK_LIBRARIES(TEMPLATE_ARG_DCL_NAMETypes ${OpenCV_LIBRARIES})
#
# # Install library
# INSTALL(
#   TARGETS CvBlobsTypes
#   RUNTIME DESTINATION bin COMPONENT applications
#   LIBRARY DESTINATION lib COMPONENT applications
#   ARCHIVE DESTINATION lib COMPONENT sdk
# )

# If DCL provides any additional headers to be used from outside of it, add them

# # Get list of header files
# FILE(GLOB headers *.hpp)
#
# # Install them to include subdirectory
# install(
#     FILES ${headers}
#     DESTINATION include/Types
#     COMPONENT sdk
# )