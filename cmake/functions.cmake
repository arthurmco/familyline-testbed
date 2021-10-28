 
function(add_sanitizers target)  
  if (FLINE_DO_CHECK_ASAN)
    if (WIN32)
      message(WARNING "No support for windows + address sanitizer yet")
    else() 
      message(STATUS "Address sanitizer enabled for ${target}")
      target_compile_options("${target}" PUBLIC "-fsanitize=address")
      target_link_libraries("${target}" PUBLIC "-fsanitize=address")
      target_compile_options("${target}" PUBLIC "-fno-omit-frame-pointer")
      target_link_libraries("${target}" PUBLIC "-fno-omit-frame-pointer")
    endif()
  else()
    message(STATUS "Address sanitizer DISABLED for ${target}")          
  endif()


  if (FLINE_DO_CHECK_UBSAN)
    if (WIN32)
      message(WARNING "No support for windows + undefined behavior sanitizer yet")
    else()
      message(STATUS "Undefined behavior sanitizer enabled for ${target}")
      target_compile_options("${target}" PUBLIC "-fsanitize=undefined")
      target_link_libraries("${target}" PUBLIC "-fsanitize=undefined")
    endif()
  else()
    message(STATUS "Undefined behavior sanitizer DISABLED for ${target}")
  endif()
endfunction()


function(add_coverage target)
  if (FLINE_SET_COVERAGE)
    message(STATUS "Compiling ${target} with code coverage information")
    target_compile_options("${target}" PUBLIC "-fprofile-arcs")
    target_compile_options("${target}" PUBLIC "-ftest-coverage")
    target_compile_options("${target}" PUBLIC "-O0")
    target_link_libraries("${target}" PUBLIC "-fprofile-arcs")
    target_link_libraries("${target}" PUBLIC "-ftest-coverage")
    target_link_libraries("${target}" PUBLIC "gcov")
    target_link_libraries("${target}" PUBLIC "--coverage")
  else()
    message(STATUS "NOT compiling ${target} with code coverage information")
  endif()
endfunction()

# Copy the asset folders to the required folder when building.
function(copy_files_to_build directory)
  file(COPY "${CMAKE_SOURCE_DIR}/shaders" DESTINATION "${directory}")
  file(COPY "${CMAKE_SOURCE_DIR}/icons" DESTINATION "${directory}")
  file(COPY "${CMAKE_SOURCE_DIR}/materials" DESTINATION "${directory}")
  file(COPY "${CMAKE_SOURCE_DIR}/models" DESTINATION "${directory}")
  file(COPY "${CMAKE_SOURCE_DIR}/textures" DESTINATION "${directory}")
  file(COPY "${CMAKE_SOURCE_DIR}/terrain_test.flte" DESTINATION "${directory}")
  file(COPY "${CMAKE_SOURCE_DIR}/assets.yml" DESTINATION "${directory}")
endfunction()


function(fill_build_information target)
  # Retrieve commit number (useful when getting logs from git clones)
  execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE )

  # Retrieve the user who compiled
  execute_process(COMMAND whoami OUTPUT_VARIABLE USERNAME
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  target_compile_definitions(${target} PUBLIC VERSION="${PROJECT_VERSION}"
    COMMIT="${COMMIT_HASH}" USERNAME="${USERNAME}")
endfunction()
