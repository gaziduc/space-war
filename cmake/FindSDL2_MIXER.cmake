
FIND_PATH(SDL2_MIXER_INCLUDE_DIR SDL2/SDL_mixer.h
	HINTS
	${SDL2}
	$ENV{SDL2}
	$ENV{SDL2_MIXER}
	PATH_SUFFIXES include SDL2
	i686-w64-mingw32/include/SDL2
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local/include/SDL2
	/usr/include/SDL2
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${DEPS_DIR}/SDL2_mixer/x64
)

# Lookup the 64 bit libs on x64
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(SDL2_MIXER_LIBRARY_TEMP
		NAMES SDL2_mixer
		HINTS
		${SDL2}
		$ENV{SDL2}
		$ENV{SDL2_MIXER}
		PATH_SUFFIXES lib64 lib
		lib/x64
		x86_64-w64-mingw32/lib
		PATHS
		/sw
		/opt/local
		/opt/csw
		/opt
		${DEPS_DIR}/SDL2_mixer/x64
	)
# On 32bit build find the 32bit libs
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(SDL2_MIXER_LIBRARY_TEMP
		NAMES SDL2_mixer
		HINTS
		${SDL2}
		$ENV{SDL2}
		$ENV{SDL2_MIXER}
		PATH_SUFFIXES lib
		lib/x86
		i686-w64-mingw32/lib
		PATHS
		/sw
		/opt/local
		/opt/csw
		/opt
		${DEPS_DIR}/SDL2_mixer/x86
	)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

SET(SDL2_MIXER_FOUND "NO")
	IF(SDL2_MIXER_LIBRARY_TEMP)
	# Set the final string here so the GUI reflects the final state.
	SET(SDL2_MIXER_LIBRARY ${SDL2_MIXER_LIBRARY_TEMP} CACHE STRING "Where the SDL2_mixer Library can be found")
	# Set the temp variable to INTERNAL so it is not seen in the CMake GUI
	SET(SDL2_MIXER_LIBRARY_TEMP "${SDL2_MIXER_LIBRARY_TEMP}" CACHE INTERNAL "")
	SET(SDL2_MIXER_FOUND "YES")
ENDIF(SDL2_MIXER_LIBRARY_TEMP)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2_MIXER REQUIRED_VARS SDL2_MIXER_LIBRARY SDL2_MIXER_INCLUDE_DIR)