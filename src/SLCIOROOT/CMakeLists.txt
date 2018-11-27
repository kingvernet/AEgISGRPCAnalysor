#*************************************************************#
# CMakeLists file for building SLCIOROOT
# Authors : K. VERNET, P. NEDELEC
#*************************************************************#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(SLCIOROOT)

# ----- SLCIOROOT executable ----------------------------------------------------
INCLUDE_DIRECTORIES(include)
FILE(COPY ./include/ DESTINATION ${PROJECT_INCLUDE_DIR})
FILE(GLOB 
	SRCS 
	src/*.cxx
)
ADD_EXECUTABLE(SLCIOROOT ${SRCS})
TARGET_LINK_LIBRARIES(SLCIOROOT ${ROOT_LIBRARIES} lcio grpc_utility tinyxml)
INSTALL(TARGETS SLCIOROOT DESTINATION bin)
