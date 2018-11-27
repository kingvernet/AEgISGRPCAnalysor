#*************************************************************#
# CMakeLists file for building TRACEROOT
# Authors : K. VERNET, P. NEDELEC
#*************************************************************#
CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
PROJECT(TRACEROOT)

# ----- TRACEROOT executable ----------------------------------------------------
INCLUDE_DIRECTORIES(include)
FILE(COPY ./include/ DESTINATION ${PROJECT_INCLUDE_DIR})
FILE(GLOB 
	SRCS 
	src/*.cxx
)
ADD_EXECUTABLE(TRACEROOT ${SRCS})
TARGET_LINK_LIBRARIES(TRACEROOT ${ROOT_LIBRARIES} grpc_utility tinyxml)
INSTALL(TARGETS TRACEROOT DESTINATION bin)
