# Try to find MiniUPnPc headers and library.
#
# Usage of this module as follows:
#
#     find_package(MiniUPnPc)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  MINIUPNPC_ROOT_DIR        Set this variable to the root installation of
#                            MINIUPNPC if the module has problems finding the
#                            proper installation path.
#
# Variables defined by this module:
#
#  MINIUPNPC_FOUND           System has MiniUPnPc library/headers.
#  MINIUPNPC_LIBRARIES       The MiniUPnPc library.
#  MINIUPNPC_INCLUDE_DIRS    The location of MiniUPnPc headers.

find_path(MINIUPNPC_ROOT_DIR
    NAMES include/miniupnpc/miniupnpc.h
)

find_library(MINIUPNPC_LIBRARIES
    NAMES miniupnpc
    HINTS ${MINIUPNPC_ROOT_DIR}/lib
)

find_path(MINIUPNPC_INCLUDE_DIRS
    NAMES miniupnpc.h
    HINTS ${MINIUPNPC_ROOT_DIR}/include/miniupnpc
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MiniUPnPc DEFAULT_MSG
    MINIUPNPC_LIBRARIES
    MINIUPNPC_INCLUDE_DIRS
)

mark_as_advanced(
    MINIUPNPC_ROOT_DIR
    MINIUPNPC_LIBRARIES
    MINIUPNPC_INCLUDE_DIRS
)
