set(CMD_GLUE_GEN_H "${CMAKE_SOURCE_DIR}/src/cmd_glue_gen.h")
set(CMD_GLUE_GEN_C "${CMAKE_SOURCE_DIR}/src/cmd_glue_gen.c")
file(GLOB CMD_C_FILES CONFIGURE_DEPENDS 
"${CMAKE_SOURCE_DIR}/src/commands/*.c"
)
file(GLOB CMD_H_FILES CONFIGURE_DEPENDS
"${CMAKE_SOURCE_DIR}/src/commands/*.h"
)

file(GLOB CMD_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/commands/*.h")
list(SORT CMD_HEADERS)

set(CMD_NAMES "")
set(CMD_ARGMAXES "")
set(CMD_FPTRS "")
set(CMD_INCLUDES "")

set(h_match "")
set(c_match "")


foreach(h ${CMD_HEADERS})
    get_filename_component(name ${h} NAME_WE)
# - Raise error if no source file found
    set(c "${CMAKE_SOURCE_DIR}/src/commands/${name}.c")
    if (NOT EXISTS ${c})
        message(FATALL_ERROR " No source file (${c}) found for commands header file (${h}).")
    endif()

    string(TOUPPER ${name} UPPER_NAME)

# - Cheching if command files have what they need
    file(READ ${h} h_content)
    set(expected_func 
    "int[ \t\n]+${name}[ \t\n]*\\([ \t\n]*int[ \t\n]+argc,[ \t\n]*char\\*\\*[ \t\n]*argv[ \t\n]*\\)")
    
    set(expected_define 
    "#[ \t]*define ${UPPER_NAME}_ARGC_MAX[ \t]+\\(?[ \t]*[0-9]+(u|U|l|L|x[0-9A-Fa-f]+)?[ \t]*\\)?")

# - 1) expected function declaration (int filename(int argc, char** argv))
    string(REGEX MATCH "${expected_func}" h_match "${h_content}")
    if (h_match EQUAL -1)
        message(FATAL_ERROR " ${h} does not have expected function declaration (${expected_func}).")
    endif()

# - 2) expected function definition (int filename(int argc, char** argv))
    string(REGEX MATCH "${expected_func}" c_match "${c_content}")
    if (c_match EQUAL -1)
        message(FATAL_ERROR " ${c} does not have expected function definition (${expected_func}).")
    endif()
    
# - 3) expected ARGC_MAX definition
    string(REGEX MATCH "${expected_define}" h_match "${h_content}")
    if (NOT h_match)
        message(FATAL_ERROR " ${h} does not have expected ARGC_MAX definition (or definition is not numerical).")
    endif()

    string(APPEND CMD_NAMES "\"${name}\", ")
    string(APPEND CMD_ARGMAXES "${UPPER_NAME}_ARGC_MAX, ")
    string(APPEND CMD_FPTRS "${name}, ")
    string(APPEND CMD_INCLUDES "#include \"${h}\"\n")
endforeach()

function(trim_last_chars input num output)
    string(LENGTH "${input}" len)
    if(len GREATER ${num})
        math(EXPR new_len "${len} - ${num}")
        string(SUBSTRING "${input}" 0 ${new_len} result)
    else()
        set(result "")
    endif()

    set(${output} "${result}" PARENT_SCOPE)
endfunction()

trim_last_chars(${CMD_NAMES} 2 CMD_NAMES)
trim_last_chars(${CMD_ARGMAXES} 2 CMD_ARGMAXES)
trim_last_chars(${CMD_FPTRS} 2 CMD_FPTRS)

configure_file("${CMAKE_SOURCE_DIR}/cmake/templates/cmd_glue_gen.h.in" "${CMD_GLUE_GEN_H}" @ONLY)
configure_file("${CMAKE_SOURCE_DIR}/cmake/templates/cmd_glue_gen.c.in" "${CMD_GLUE_GEN_C}" @ONLY)
