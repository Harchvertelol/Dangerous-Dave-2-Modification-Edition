#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "tgui" for configuration "Release"
set_property(TARGET tgui APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tgui PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libtgui.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/tgui.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS tgui )
list(APPEND _IMPORT_CHECK_FILES_FOR_tgui "${_IMPORT_PREFIX}/lib/libtgui.a" "${_IMPORT_PREFIX}/bin/tgui.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
