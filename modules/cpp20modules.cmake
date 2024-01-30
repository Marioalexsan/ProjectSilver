block(PROPAGATE CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API)
  set(API_VER "${CMAKE_VERSION}")
  set(API_ID "")
  
  if (API_VER VERSION_GREATER_EQUAL "3.28.0")
    message("CMake version ${CMAKE_VERSION} >= 3.28.0 has official module support.'")
  elseif (API_VER VERSION_GREATER_EQUAL "3.27.0" AND API_VER VERSION_LESS_EQUAL "3.27.4")
    set(API_ID "aa1f7df0-828a-4fcd-9afc-2dc80491aca7")
  elseif (API_VER VERSION_GREATER_EQUAL "3.26.0" AND API_VER VERSION_LESS_EQUAL "3.26.5")
    set(API_ID "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
  elseif (API_VER VERSION_GREATER_EQUAL "3.25.0" AND API_VER VERSION_LESS_EQUAL "3.25.3")
    set(API_ID "3c375311-a3c9-4396-a187-3227ef642046")
  else()
    message(FATAL_ERROR "CMake version ${CMAKE_VERSION} does not support modules.'")
  endif()

  if (NOT API_ID STREQUAL "")
    message("CMake C++20 Modules API UUID set to ${API_ID}.")
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "${API_ID}")
  endif()
endblock()