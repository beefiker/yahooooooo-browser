cmake_minimum_required(VERSION 3.21)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Project name
project(yahooooooo-browser)

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform check
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(FATAL_ERROR "This project currently supports macOS only")
endif()

# CEF variables
set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_build")

# Check if CEF is built
if(EXISTS "${CEF_ROOT}/Release" OR EXISTS "${CEF_ROOT}/Debug")
    message(STATUS "🎉 CEF binaries found! Enabling CEF integration...")
    
    # Add CEF cmake modules to path
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
    
    # Find CEF package
    find_package(CEF REQUIRED)
    
    # Include the libcef_dll_wrapper target
    add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

    # Include shared resources
    set(SHARED_RESOURCES_SRCS
        src/resources/multiprocess_test.html
    )

    # Source files for main browser - CEF 137 compliant
    set(BROWSER_SRCS
        src/main.cpp
        src/app.cpp
        src/client.cpp
    )
    
    # Add macOS-specific files
    if(APPLE)
        list(APPEND BROWSER_SRCS src/client_mac.mm)
    endif()

    # Headers
    set(BROWSER_HEADERS
        src/app.h
        src/client.h
    )

    # Helper executable sources for macOS - CEF 137 compliant
    set(BROWSER_HELPER_MAIN_SRCS
        src/helper_main.cpp
    )

    # Resources
    set(BROWSER_RESOURCES_SRCS
        ${SHARED_RESOURCES_SRCS}
    )

    # Target executable names
    set(BROWSER_TARGET "yahooooooo-browser")
    set(BROWSER_HELPER_TARGET "yahooooooo-browser_Helper")
    set(BROWSER_HELPER_OUTPUT_NAME "yahooooooo-browser Helper")

    # Output path for the main app bundle
    set(BROWSER_APP "${CMAKE_CURRENT_BINARY_DIR}/${BROWSER_TARGET}.app")

    # Variables referenced from Info.plist
    set(EXECUTABLE_NAME "${BROWSER_TARGET}")
    set(PRODUCT_NAME "${BROWSER_TARGET}")

    # Create main app bundle target
    add_executable(${BROWSER_TARGET} MACOSX_BUNDLE ${BROWSER_SRCS} ${BROWSER_RESOURCES_SRCS})
    
    # Set executable properties
    SET_EXECUTABLE_TARGET_PROPERTIES(${BROWSER_TARGET})
    
    # Add dependencies and linking
    add_dependencies(${BROWSER_TARGET} libcef_dll_wrapper)
    
    # Link the CEF framework for macOS
    if(APPLE)
        target_link_libraries(${BROWSER_TARGET} 
            libcef_dll_wrapper
            ${CEF_STANDARD_LIBS}
            "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
        )
    else()
        target_link_libraries(${BROWSER_TARGET} 
            libcef_dll_wrapper
            ${CEF_STANDARD_LIBS}
        )
    endif()

    # Set target properties
    set_target_properties(${BROWSER_TARGET} PROPERTIES
        RESOURCE "${BROWSER_RESOURCES_SRCS}"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/mac/Info.plist"
    )

    # Use sandbox if available
    # DISABLED: Sandbox library causes symbol issues with CEF 137
    # if(USE_SANDBOX)
    #     ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    #     target_link_libraries(${BROWSER_TARGET} cef_sandbox_lib)
    # endif()

    # Copy the CEF framework into the Frameworks directory
    add_custom_command(
        TARGET ${BROWSER_TARGET}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
                "${BROWSER_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
        VERBATIM
    )

    # Create the multiple Helper app bundle targets
    foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
        # Convert to a list and extract the suffix values
        string(REPLACE ":" ";" _suffix_list ${_suffix_list})
        list(GET _suffix_list 0 _name_suffix)
        list(GET _suffix_list 1 _target_suffix)
        list(GET _suffix_list 2 _plist_suffix)

        # Define Helper target and output names
        set(_helper_target "${BROWSER_HELPER_TARGET}${_target_suffix}")
        set(_helper_output_name "${BROWSER_HELPER_OUTPUT_NAME}${_name_suffix}")

        # Create Helper-specific variants of the helper-Info.plist file
        set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${_target_suffix}.plist")
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/src/mac/helper-Info.plist" _plist_contents)
        string(REPLACE "\${EXECUTABLE_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
        string(REPLACE "\${PRODUCT_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
        string(REPLACE "\${BUNDLE_ID_SUFFIX}" "${_plist_suffix}" _plist_contents ${_plist_contents})
        file(WRITE ${_helper_info_plist} ${_plist_contents})

        # Create Helper executable target - CEF 137 compliant
        add_executable(${_helper_target} MACOSX_BUNDLE ${BROWSER_HELPER_MAIN_SRCS})
        SET_EXECUTABLE_TARGET_PROPERTIES(${_helper_target})
        add_dependencies(${_helper_target} libcef_dll_wrapper)
        target_link_libraries(${_helper_target} 
            libcef_dll_wrapper
            ${CEF_STANDARD_LIBS}
        )
        
        set_target_properties(${_helper_target} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${_helper_info_plist}
            OUTPUT_NAME ${_helper_output_name}
        )

        # Use sandbox if available
        # DISABLED: Sandbox library causes symbol issues with CEF 137
        # if(USE_SANDBOX)
        #     target_link_libraries(${_helper_target} cef_sandbox_lib)
        # endif()

        # Add the Helper as a dependency of the main executable target
        add_dependencies(${BROWSER_TARGET} "${_helper_target}")

        # Copy the Helper app bundle into the Frameworks directory
        add_custom_command(
            TARGET ${BROWSER_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_CURRENT_BINARY_DIR}/${_helper_output_name}.app"
                    "${BROWSER_APP}/Contents/Frameworks/${_helper_output_name}.app"
            VERBATIM
        )
    endforeach()

    message(STATUS "✅ Browser configured with CEF support")
    message(STATUS "🎯 Ready to build: ${BROWSER_TARGET}")

else()
    message(STATUS "⏳ CEF binaries not found yet. Creating basic test build...")
    message(STATUS "📍 Looking for CEF in: ${CEF_ROOT}")
    message(STATUS "🔄 Run this again after CEF build completes")
    
    # Create a basic test executable without CEF
    add_executable(${PROJECT_NAME}-test src/main.cpp)
    target_compile_definitions(${PROJECT_NAME}-test PRIVATE CEF_NOT_READY=1)
    
    message(STATUS "📝 Basic test build created: ${PROJECT_NAME}-test")
endif()

# Print configuration info
message(STATUS "Building yahooooooo-browser for macOS")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "CEF Root: ${CEF_ROOT}") 