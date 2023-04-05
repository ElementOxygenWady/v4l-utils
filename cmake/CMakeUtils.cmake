#* Author: Wady <iswady@163.com>
#* Date:   Thu Sep 02 12:00:00 2021

set(COMPILE_SCRIPT "${CMAKE_BINARY_DIR}/Compile${PROJECT_NAME}.sh")
set(CLEAN_SCRIPT "${CMAKE_BINARY_DIR}/Clean${PROJECT_NAME}.sh")
set(INSTALL_SCRIPT "${CMAKE_BINARY_DIR}/Install${PROJECT_NAME}.sh")

macro(utils_write_script CONTENT)
    set(METHOD "APPEND")
    set(WCOMPILE NO)
    set(WCLEAN NO)
    set(WINSTALL NO)

    foreach(arg IN ITEMS ${ARGN})
        if(${arg} STREQUAL "TRUNCATE")
            set(METHOD "WRITE")
        endif()

        if(${arg} STREQUAL "WRITE_COMPILE")
            set(WCOMPILE YES)
        endif()

        if(${arg} STREQUAL "WRITE_CLEAN")
            set(WCLEAN YES)
        endif()

        if(${arg} STREQUAL "WRITE_INSTALL")
            set(WINSTALL YES)
        endif()

        if(${arg} STREQUAL "WRITE_ALL")
            set(WCOMPILE YES)
            set(WCLEAN YES)
            set(WINSTALL YES)
        endif()
    endforeach()

    if(WCOMPILE)
        file(${METHOD} ${COMPILE_SCRIPT} "${CONTENT}\n")
    endif()
    if(WCLEAN)
        file(${METHOD} ${CLEAN_SCRIPT} "${CONTENT}\n")
    endif()
    if(WINSTALL)
        file(${METHOD} ${INSTALL_SCRIPT} "${CONTENT}\n")
    endif()
endmacro()
