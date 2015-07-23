# @file		CMakeLists.txt
# @author	Lukasz Wrzosek (l.wrzosek@samsung.com)
# @version	1.0
# @brief
#

# Check minimum CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Project name
PROJECT(voice-control-elm)

SET(LIBRARY_NAME "vc-elm")
#SET(VOICE_CONTROL_ELM_VERSION_MAJOR 0)
#SET(VOICE_CONTROL_ELM_VERSION_MINOR 2)
#SET(VOICE_CONTROL_ELM_VERSION_PATCH 0)
SET(VERSION 0.2.0)

# pkg config tool
INCLUDE(FindPkgConfig)

# Build type
SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -fprofile-arcs -ftest-coverage -D_GNU_SOURCE")

IF (LIBDIR MATCHES "/usr/lib64")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLIB64")
ENDIF (LIBDIR)

# CMake settings
MESSAGE(STATUS "========================================")
MESSAGE(STATUS "CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
MESSAGE(STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH})
MESSAGE(STATUS "LIBRARY_OUTPUT_PATH: " ${LIBRARY_OUTPUT_PATH})
MESSAGE(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
MESSAGE(STATUS "CMAKE_COMMAND: " ${CMAKE_COMMAND})
MESSAGE(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT})
MESSAGE(STATUS "CMAKE_CURRENT_LIST_FILE: " ${CMAKE_CURRENT_LIST_FILE})
MESSAGE(STATUS "CMAKE_CURRENT_LIST_LINE: " ${CMAKE_CURRENT_LIST_LINE})
MESSAGE(STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH})
MESSAGE(STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH})
MESSAGE(STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM})
MESSAGE(STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})
MESSAGE(STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION})
MESSAGE(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
MESSAGE(STATUS "UNIX: " ${UNIX})
MESSAGE(STATUS "WIN32: " ${WIN32})
MESSAGE(STATUS "APPLE: " ${APPLE})
MESSAGE(STATUS "MINGW: " ${MINGW})
MESSAGE(STATUS "CYGWIN: " ${CYGWIN})
MESSAGE(STATUS "BORLAND: " ${BORLAND})
MESSAGE(STATUS "MSVC: " ${MSVC})
MESSAGE(STATUS "MSVC_IDE: " ${MSVC_IDE})
MESSAGE(STATUS "MSVC60: " ${MSVC60})
MESSAGE(STATUS "MSVC70: " ${MSVC70})
MESSAGE(STATUS "MSVC71: " ${MSVC71})
MESSAGE(STATUS "MSVC80: " ${MSVC80})
MESSAGE(STATUS "CMAKE_COMPILER_2005: " ${CMAKE_COMPILER_2005})
MESSAGE(STATUS "CMAKE_SKIP_RULE_DEPENDENCY: " ${CMAKE_SKIP_RULE_DEPENDENCY})
MESSAGE(STATUS "CMAKE_SKIP_INSTALL_ALL_DEPENDENCY: " ${CMAKE_SKIP_INSTALL_ALL_DEPENDENCY})
MESSAGE(STATUS "CMAKE_SKIP_RPATH: " ${CMAKE_SKIP_RPATH})
MESSAGE(STATUS "CMAKE_VERBOSE_MAKEFILE: " ${CMAKE_VERBOSE_MAKEFILE})
MESSAGE(STATUS "CMAKE_SUPPRESS_REGENERATION: " ${CMAKE_SUPPRESS_REGENERATION})
MESSAGE(STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS})
MESSAGE(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
MESSAGE(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
MESSAGE(STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS})
MESSAGE(STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER})
MESSAGE(STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
MESSAGE(STATUS "CMAKE_COMPILER_IS_GNUCC: " ${CMAKE_COMPILER_IS_GNUCC})
MESSAGE(STATUS "CMAKE_COMPILER_IS_GNUCXX : " ${CMAKE_COMPILER_IS_GNUCXX})
MESSAGE(STATUS "CMAKE_AR: " ${CMAKE_AR})
MESSAGE(STATUS "CMAKE_RANLIB: " ${CMAKE_RANLIB})
MESSAGE(STATUS "========================================")

# Warning flags
#ADD_DEFINITIONS("-Werror")		# Generate all warnings as errors
ADD_DEFINITIONS("-Wall")		# Generate all warnings
ADD_DEFINITIONS("-Wextra")		# Generate even more extra warnings
ADD_DEFINITIONS("-pedantic")		# Accept only pedantic code
ADD_DEFINITIONS("-Wwrite-strings")	# Do not accept writing to constant string memory
ADD_DEFINITIONS("-Winit-self")		# Do not accept initializing variable with itself
ADD_DEFINITIONS("-Wcast-align")		# Do not accept misaligning with casting
ADD_DEFINITIONS("-Wcast-qual")		# Do not accept removing qualifiers with casting
ADD_DEFINITIONS("-Wpointer-arith")	# Warn about void pointer arthimetic
ADD_DEFINITIONS("-Wstrict-aliasing")	# Ensure strict aliasing
ADD_DEFINITIONS("-Wuninitialized")	# Do not accept uninitialized variables
ADD_DEFINITIONS("-Wlong-long")		# Do not allow using long long
ADD_DEFINITIONS("-Wunreachable-code")	# Warn about unreachable code
ADD_DEFINITIONS("-Wfloat-equal")	# Do not accept comparing floating points with equal operator
ADD_DEFINITIONS("-Wabi")		# Warn about possible ABI problems
ADD_DEFINITIONS("-Wswitch-enum")	# Check switch enumeration
ADD_DEFINITIONS("-Wformat=2")		# Check printf formatting
ADD_DEFINITIONS("-Wundef")		# Warn if an undefined identifier is evaluated in an @if directive.
ADD_DEFINITIONS("-Wshadow")		# Warn whenever a local variable shadows another local variable, parameter or global variable or whenever a built-in function is shadowed
ADD_DEFINITIONS("-Wconversion")		# Warn for implicit conversions that may alter a value
ADD_DEFINITIONS("-Wlogical-op")		# Warn about suspicious uses of logical operators in expressions
ADD_DEFINITIONS("-Wmissing-field-initializers") # Warn if a structure's initializer has some fields missing.
ADD_DEFINITIONS("-Wredundant-decls")	# Warn if anything is declared more than once in the same scope, even in cases where multiple declaration is valid and changes nothing.
ADD_DEFINITIONS("-Wswitch-default")	# Warn whenever a switch statement does not have a default case.
ADD_DEFINITIONS("-Wsync-nand")		# Warn when __sync_fetch_and_nand and __sync_nand_and_fetch built-in functions are used. These functions changed semantics in GCC 4.4.
ADD_DEFINITIONS("-Wunused")		# All the above -Wunused options combined.
ADD_DEFINITIONS("-Wstrict-overflow=5")	# Also warn about cases where the compiler reduces the magnitude of a constant involved in a comparison.


SET(TARGET_VC_ELM_LIBRARY ${LIBRARY_NAME})

SET(VC_ELM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(include)

# INCLUDE FOR BUILD & INSTALL .PO FILES
ADD_SUBDIRECTORY(data/po)

ADD_CUSTOM_TARGET(voice-control-elm.edj
	COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/data
	${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.edc ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.edj
	)
ADD_DEPENDENCIES(${LIBRARY_NAME} ${PROJECT_NAME}.edj)

INSTALL(FILES	 ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.edj
		DESTINATION share/edje)

INSTALL(FILES	${PROJECT_SOURCE_DIR}/data/image.png
		${PROJECT_SOURCE_DIR}/data/image_test.png
	DESTINATION share/voice/vc-elm)

INSTALL(FILES	${PROJECT_SOURCE_DIR}/voice-control-elm-config.xml
	DESTINATION ${LIBDIR}/voice/vc-elm/1.0)

