#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "tgui" for configuration "Debug"
set_property(TARGET tgui APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(tgui PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libtgui-d.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/tgui-d.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS tgui )
list(APPEND _IMPORT_CHECK_FILES_FOR_tgui "${_IMPORT_PREFIX}/lib/libtgui-d.a" "${_IMPORT_PREFIX}/bin/tgui-d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
