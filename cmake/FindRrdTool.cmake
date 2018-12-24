# - Find RRDtool
# Find the native RRDtool includes and library
#
#  RRDTOOL_INCLUDE_DIR - where to find rrd.h, etc.
#  RRDTOOL_LIBRARIES - List of libraries needed when using RRDtool.
#  RRDTOOL_FOUND - True if RRDtool found.

if(RRDTOOL_INCLUDE_DIR)
	# Already in cache, be silent
	set(RRDTOOL_FIND_QUIETLY TRUE)
endif()

FIND_PATH(RRDTOOL_INCLUDE_DIR 
	NAMES rrd.h
	PATHS /usr/include /usr/local/include /opt/local/include
	)

set(RRDTOOL_NAMES rrd_th rrd)
FIND_LIBRARY(RRDTOOL_LIBRARY 
	NAMES rrd_th rrd
	PATHS /usr/lib /usr/local/lib /opt/local/lib
	)

if(RRDTOOL_LIBRARY AND RRDTOOL_INCLUDE_DIR)
	set(RRDTOOL_FOUND TRUE)
	set(RRDTOOL_LIBRARIES ${RRDTOOL_LIBRARY} CACHE INTERNAL "The libraries needed to use RRDTOOL" FORCE)
	set(RRDTOOL_INCLUDE_DIRS ${RRDTOOL_INCLUDE_DIR} CACHE INTERNAL "The include directories needed to use RRDTOOL" FORCE)

else()
	set(RRDTOOL_FOUND FALSE CACHE INTERNAL "" FORCE)
	set(RRDTOOL_LIBRARY "" CACHE INTERNAL "" FORCE)
	set(RRDTOOL_LIBRARIES "" CACHE INTERNAL "" FORCE)
	set(RRDTOOL_INCLUDE_DIR "" CACHE INTERNAL "" FORCE)
	set(RRDTOOL_INCLUDE_DIRS "" CACHE INTERNAL "" FORCE)
	if (Rrdtool_FIND_REQUIRED)
		message(FATAL_ERROR "Could NOT find rrdtool")
	endif()
endif()

if(RRDTOOL_FOUND)
	if(NOT RRDTOOL_FIND_QUIETLY)
		message(STATUS "Found RRDtool: ${RRDTOOL_LIBRARIES}")
	endif()
else()
	if(RRDTOOL_FIND_REQUIRED)
		message(STATUS "Looked for RRDtool libraries named ${RRDTOOL_NAMES}")
		message(FATAL_ERROR "Could NOT find RRDtool library")
	endif()
endif()

MARK_AS_ADVANCED(
	RRDTOOL_INCLUDE_DIR 
	RRDTOOL_LIBRARIES 
	RRDTOOL_LIBRARY)
