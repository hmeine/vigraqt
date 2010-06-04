# This module finds an installed Vigra package.
#
# It sets the following variables:
#  VIGRA_FOUND         - Set to false, or undefined, if vigra isn't found.
#  VIGRA_INCLUDE_DIR   - The CppUnit include directory.
#  VIGRA_IMPEX_LIBRARY - Vigra's impex library

FIND_PATH(VIGRA_INCLUDE_DIR vigra/matrix.hxx)
FIND_LIBRARY(VIGRA_IMPEX_LIBRARY NAMES vigraimpex)

IF (VIGRA_INCLUDE_DIR AND VIGRA_IMPEX_LIBRARY)
    SET(VIGRA_FOUND TRUE)
ENDIF()

IF(VIGRA_IMPEX_LIBRARY)
    SET(VIGRA_IMPEX_LIBRARY_FOUND TRUE)
ENDIF()

IF(VIGRA_FOUND)
    # show which Vigra was found only if not quiet
    IF (NOT Vigra_FIND_QUIETLY)
      MESSAGE(STATUS "Found Vigra")
      MESSAGE(STATUS "  > includes:      ${VIGRA_INCLUDE_DIR}")
      MESSAGE(STATUS "  > impex library: ${VIGRA_IMPEX_LIBRARY}")
    ENDIF (NOT Vigra_FIND_QUIETLY)
ELSE()
    # fatal error if CppUnit is required but not found
    IF (Vigra_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find Vigra")
    ENDIF (Vigra_FIND_REQUIRED)
ENDIF()
