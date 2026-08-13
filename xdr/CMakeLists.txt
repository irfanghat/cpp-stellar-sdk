find_program(XDRC_EXECUTABLE xdrc REQUIRED)

# --------------------------------------------
# Locate libxdrpp library & include headers
# --------------------------------------------
find_path(XDRPP_INCLUDE_DIR NAMES xdrpp/marshal.h REQUIRED)
find_library(XDRPP_LIBRARY NAMES xdrpp REQUIRED)

if(NOT TARGET xdrpp::xdrpp)
    add_library(xdrpp::xdrpp UNKNOWN IMPORTED)
    set_target_properties(xdrpp::xdrpp PROPERTIES
        IMPORTED_LOCATION "${XDRPP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${XDRPP_INCLUDE_DIR}"
    )
endif()

# ---------------------------------------
# Collect Stellar XDR definition files 
# ---------------------------------------
file(GLOB XDR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../external/stellar-xdr/*.x")

if(NOT XDR_FILES)
    message(FATAL_ERROR "No .x files found in external/stellar-xdr. Run 'git submodule update --init --recursive'")
endif()

set(GENERATED_HEADERS)
set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GEN_DIR}")

foreach(XDR_FILE ${XDR_FILES})
    get_filename_component(FIL_WE ${XDR_FILE} NAME_WE)
    
    set(HDR_OUT "${GEN_DIR}/${FIL_WE}.hh")
    
    add_custom_command(
        OUTPUT "${HDR_OUT}"
        COMMAND ${XDRC_EXECUTABLE} -hh -o "${HDR_OUT}" "${XDR_FILE}"
        DEPENDS "${XDR_FILE}"
        COMMENT "Generating C++ XDR header for ${FIL_WE}.x"
        VERBATIM
    )
    
    list(APPEND GENERATED_HEADERS "${HDR_OUT}")
endforeach()

# -------------------------------------------------------
# Custom target to drive header generation during build
# -------------------------------------------------------
add_custom_target(generate-stellar-xdr DEPENDS ${GENERATED_HEADERS})

# -------------------------------------------------------
# Header-only interface target for generated XDR types
# The compiler can only generate C++ header files
# with XDR & RPC program definitions.
# -------------------------------------------------------
add_library(stellar-xdr INTERFACE)
add_library(stellar::xdr ALIAS stellar-xdr)

add_dependencies(stellar-xdr generate-stellar-xdr)

target_include_directories(stellar-xdr INTERFACE
    $<BUILD_INTERFACE:${GEN_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(stellar-xdr INTERFACE xdrpp::xdrpp)