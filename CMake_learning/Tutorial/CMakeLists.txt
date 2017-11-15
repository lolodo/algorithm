# should we use our own math functions?
option(USE_MYMATH "User tutorial provided math implementation" ON)
cmake_minimum_required (VERSION 2.6)
project (Tutorial)

# The version number
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"	
	"${PROJECT_BINARY_DIR}/TutorialConfig.h"	
)

#add the install targets
install (TARGETS MathFunctions DESTINATION bin)
install (FILES MathFunctions DESTINATION include)

#add the install targets
install (TARGETS Tutorial DESTINATION bin)
install (FILES "$(PROJECT_BINARY_DIR)/TutorialConfig.h"
	DESTINATION include)

#add the binary tree to the search path for include files, so that we will find TutorialConfig.h
include_directories("$(PROJECT_BINARY_DIR)")
include_directories ("$(PROJECT_SOURCE_DIR)/MathFunctions")
add_library(MathFunctions mysqrt.c)
add_executable(Tutorial tutorial.c)
target_link_libraries(Tutorial MathFunctions)

include(Ctest)

#does the application run
add_test(tutorialRuns Tutorial 25)

#does it sqrt of 25
add_test(TutorialComp25 Tutorial 25)
set_tests_properties(TutorialComp25 PROPERTIES PASS_REGULAR_EXPRESSION "25 is 5")

#does it handle negative numbers
add_test(TutorialNegative Tutorial -25)
set_tests_properties(TutorialNegative PROPERTIES PASS_REGULAR_EXPRESSION "-25 is 0")

#does it handle small numbers
add_test(TutorialSmall Tutorial 0.0001)
set_tests_properties(TutorialSmall PROPERTIES PASS_REGULAR_EXPRESSION "0.0001 is 0.1")

#does the usage message work?
add_test(TutorialUsage Tutorial)
set_tests_properties(TutorialUsage PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*number")

#define a macro to simplify adding tests, then use it
macro(do_test arg result)
	add_test (TutorialCom${arg} Tutorial ${arg})
	set_tests_properties(TutorialComp${arg} PROPERTIES PASS_REGULAR_EXPRESSION ${result})
endmacro(do_test)

#do a bunch of result based tests
do_test(25 "25 is 5")
do_test(-25 "-25 is 0")

#does this system provice the log and exp functions?
include (CheckFunctionExists)
check_function_exists (log HAVE_LOG)
check_function_exists (exp HAVE_EXP)
