# Find dependencies

# OpenCV
find_package(OpenCV REQUIRED)
list(APPEND EMEOCV_LINKER_LIBS ${OpenCV_LIBS})
include_directories(${OpenCV_INCLUDE_DIRS})

# RRDtool
find_package(RrdTool REQUIRED)
list(APPEND EMEOCV_LINKER_LIBS ${RRDTOOL_LIBRARY} ${RRDTOOL_LIBRARIES})
include_directories(${RRDTOOL_INCLUDE_DIR})

# Log4Cpp
find_package(Log4cpp REQUIRED)
list(APPEND EMEOCV_LINKER_LIBS ${LOG4CPP_LIBRARY})
include_directories(${LOG4CPP_INCLUDE_DIRS})



