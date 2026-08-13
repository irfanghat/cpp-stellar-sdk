# xdr/CMakeLists.txt

find_program(XDRC_EXECUTABLE xdrc REQUIRED)
find_package(xdrpp REQUIRED)

# List of Stellar XDR definition files 
set(XDR_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../external/stellar-xdr/Stellar-types.x"
    "${CMAKE_CURRENT_SOURCE_DIR}/../external/stellar-xdr/Stellar-ledger-entries.x"
    "${CMAKE_CURRENT_SOURCE_DIR}/../external/stellar-xdr/Stellar-transaction.x"
    "${CMAKE_CURRENT_SOURCE_DIR}/../external/stellar-xdr/Stellar-ledger.x"
    "${CMAKE_CURRENT_SOURCE_DIR}/../external/stellar-xdr/Stellar-overlay.x"
)

set(GENERATED_HEADERS)
set(GENERATED_SOURCES)
set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GEN_DIR}")

foreach(XDR_FILE ${XDR_FILES})
    get_filename_component(FIL_WE ${XDR_FILE} NAME_WE)
    
    set(HDR_OUT "${GEN_DIR}/${FIL_WE}.hh")
    set(SRC_OUT "${GEN_DIR}/${FIL_WE}.cc")
    
    add_custom_command(
        OUTPUT "${HDR_OUT}" "${SRC_OUT}"
        COMMAND ${XDRC_EXECUTABLE} -hh "${HDR_OUT}" -cc "${SRC_OUT}" "${XDR_FILE}"
        DEPENDS "${XDR_FILE}"
        COMMENT "Generating C++ XDR code for ${FIL_WE}.x"
        VERBATIM
    )
    
    list(APPEND GENERATED_HEADERS "${HDR_OUT}")
    list(APPEND GENERATED_SOURCES "${SRC_OUT}")
endforeach()

# -----------------------------------------------------
# Create a static library target for generated XDR code
# -----------------------------------------------------
add_library(stellar-xdr STATIC ${GENERATED_SOURCES} ${GENERATED_HEADERS})

target_include_directories(stellar-xdr PUBLIC
    $<BUILD_INTERFACE:${GEN_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(stellar-xdr PUBLIC xdrpp::xdrpp)