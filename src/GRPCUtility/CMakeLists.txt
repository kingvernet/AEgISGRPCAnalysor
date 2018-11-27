#*************************************************************#
# CMakeLists file for building libgrpc_utility
# Authors : K. VERNET, P. NEDELEC
#*************************************************************#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# ----- libgrpc_utility library ----------------------------------------------------
INCLUDE_DIRECTORIES(include)
FILE(COPY ./include/ DESTINATION ${PROJECT_INCLUDE_DIR})

FILE(GLOB 
	SRCS 
	src/*.cxx
)
ADD_LIBRARY(grpc_utility ${SRCS})
#TARGET_LINK_LIBRARIES(grpc_utility ${ROOT_LIBRARIES})
#INSTALL(TARGETS grpc_utility DESTINATION ${EXECUTABLE_PATH}/lib)
