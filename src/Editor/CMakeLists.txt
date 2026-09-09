file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
# main.cpp belongs to the standalone exe; EntryPoint.cpp is compiled into each
# editor exe by batap_add_editor (an object in a static lib whose symbols
# nothing references would be dropped by the linker).
list(REMOVE_ITEM EDITOR_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/EntryPoint.cpp"
)

set(IMGUI_STDLIB "${PROJECT_SOURCE_DIR}/include/imgui/misc/cpp/imgui_stdlib.cpp")

add_library(Batap_EditorLib STATIC
  ${EDITOR_SOURCES}
  "${IMGUI_STDLIB}"
)

target_include_directories(Batap_EditorLib PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"          # src/Editor
)

target_link_libraries(Batap_EditorLib PUBLIC
  Batap_Engine
)

# An editor executable: the given sources + EntryPoint.cpp, linked against
# the editor lib. A game passes an editor_main.cpp that includes its
# GameComponents.h — that's what puts its components in the editor binary.
function(batap_add_editor name)
  set(entry "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/EntryPoint.cpp")
  if(WIN32)
    # WIN32 = sous-système fenêtré (wWinMain) ; l'icône vient du .rc
    add_executable(${name} WIN32
      ${ARGN}
      "${entry}"
      "${PROJECT_SOURCE_DIR}/resources.rc"
    )
  else()
    add_executable(${name}
      ${ARGN}
      "${entry}"
    )
  endif()
  target_link_libraries(${name} PRIVATE Batap_EditorLib)
endfunction()

batap_add_editor(Batap_Editor "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
