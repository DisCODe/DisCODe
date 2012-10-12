
# Add subdirectories to the build - CMake will run CMakeFiles from them

install(FILES DisCODeMacros.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/)

# DCL template
install(FILES Templates/README.md DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/Templates)
install(FILES Templates/CMakeLists.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/Templates)

install(FILES Templates/src/CMakeLists.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/Templates/src)
install(FILES Templates/src/DCLConfig.cmake.in DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/Templates/src)

install(FILES Templates/src/Components/CMakeLists.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/Templates/src/Components)

install(FILES Templates/src/Types/CMakeLists.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/Templates/src/Types)

# Component template
install(FILES Templates/src/Components/Component/Component.cpp DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/Templates/src/Components/Component)
install(FILES Templates/src/Components/Component/Component.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/Templates/src/Components/Component)
install(FILES Templates/src/Components/Component/CMakeLists.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DisCODe/Templates/src/Components/Component)
