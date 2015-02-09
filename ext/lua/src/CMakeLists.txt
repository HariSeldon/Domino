set(CORE "lapi.c" "lcode.c" "lctype.c" "ldebug.c" "ldo.c" "ldump.c" "lfunc.c" "lgc.c" "llex.c"
         "lmem.c" "lobject.c" "lopcodes.c" "lparser.c" "lstate.c" "lstring.c" "ltable.c" 
         "ltm.c" "lundump.c" "lvm.c" "lzio.c")

set(LIB "lauxlib.c" "lbaselib.c" "lbitlib.c" "lcorolib.c" "ldblib.c" "liolib.c"
        "lmathlib.c" "loslib.c" "lstrlib.c" "ltablib.c" "loadlib.c" "linit.c")

find_library(DL_LIB_PATH dl
             DOC "The dl library")
if(EXISTS ${DL_LIB_PATH})
  message(STATUS "Looking for dl: found")
else(EXISTS ${DL_LIB_PATH})
  message(FATAL_ERROR "Looking for dl: not found")
endif(EXISTS ${DL_LIB_PATH})

find_library(READ_LINE_LIB_PATH readline
             DOC "The readline library")
if(EXISTS ${READ_LINE_LIB_PATH})
  message(STATUS "Looking for readline: found")
else(EXISTS ${READ_LINE_LIB_PATH})
  message(FATAL_ERROR "Looking for readline: not found")
endif(EXISTS ${READ_LINE_LIB_PATH})

add_library(${LUA_LIB} ${CORE} ${LIB})
