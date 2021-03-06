#
# Copyright (c) 2021-2022 Yuriy Zinchenko.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

CMAKE_MINIMUM_REQUIRED (VERSION 2.8.12)

IF (COMMAND CMAKE_POLICY)
	CMAKE_POLICY (SET CMP0069 NEW)
	CMAKE_POLICY (SET CMP0074 NEW)
ENDIF ()

PROJECT (LearnGL)

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET (CMAKE_CODEBLOCKS_EXCLUDE_EXTERNAL_FILES 1)

FIND_PACKAGE (GLEW REQUIRED)
FIND_PACKAGE (SDL2 REQUIRED)

INCLUDE (CheckIPOSupported REQUIRED)
CHECK_IPO_SUPPORTED (RESULT LTO_SUPPORTED)

SET (TARGET_NAME common)
ADD_LIBRARY (${TARGET_NAME} OBJECT common.c common.h)
TARGET_LINK_LIBRARIES (${TARGET_NAME} PUBLIC SDL2::SDL2)

SET (TARGET_NUMBER 1)
SET (TARGET_NAME first_triangle)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

SET (TARGET_NUMBER 2)
SET (TARGET_NAME texture)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

SET (TARGET_NUMBER 3)
SET (TARGET_NAME transform)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

SET (TARGET_NUMBER 4)
SET (TARGET_NAME cube)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

SET (TARGET_NUMBER 5)
SET (TARGET_NAME instances)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

SET (TARGET_NUMBER 6)
SET (TARGET_NAME camera)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

SET (TARGET_NUMBER 7)
SET (TARGET_NAME light)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

SET (TARGET_NUMBER 8)
SET (TARGET_NAME material)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

SET (TARGET_NUMBER 9)
SET (TARGET_NAME light_env)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

SET (TARGET_NUMBER 10)
SET (TARGET_NAME light_point)
ADD_EXECUTABLE (${TARGET_NUMBER}_${TARGET_NAME} ${TARGET_NAME}.c)
TARGET_LINK_LIBRARIES (${TARGET_NUMBER}_${TARGET_NAME} PRIVATE common SDL2::SDL2 SDL2::SDL2main GLEW::glew)

FILE (GLOB_RECURSE RESOURCE_FILES RELATIVE ${CMAKE_SOURCE_DIR} data/*.*)
FOREACH (RESOURCE ${RESOURCE_FILES})
	CONFIGURE_FILE (${CMAKE_SOURCE_DIR}/${RESOURCE} bin/${RESOURCE} COPYONLY)
ENDFOREACH ()
