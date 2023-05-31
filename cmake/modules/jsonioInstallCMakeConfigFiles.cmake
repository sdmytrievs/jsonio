# The path where cmake config files are installed
set(JSONIO_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/jsonio17)

install(EXPORT jsonioTargets
    FILE jsonioTargets.cmake
    NAMESPACE jsonio17::
    DESTINATION ${JSONIO_INSTALL_CONFIGDIR}
    COMPONENT cmake)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/jsonioConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/jsonioConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/jsonioConfig.cmake
    INSTALL_DESTINATION ${JSONIO_INSTALL_CONFIGDIR}
    PATH_VARS JSONIO_INSTALL_CONFIGDIR)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/jsonioConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/jsonioConfigVersion.cmake
    DESTINATION ${JSONIO_INSTALL_CONFIGDIR})
