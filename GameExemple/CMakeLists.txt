# The example game: first consumer of the engine that is not the editor.
# It only sees Batap_Engine's public interface — if something is missing
# here, the fix is to grow the engine API, never to reach into src/Editor.
add_executable(GameExemple
  main.cpp
)

target_link_libraries(GameExemple PRIVATE
  Batap_Engine
)

# The game's editor: the editor lib + this game's components (editor_main.cpp
# includes GameComponents.h — static init registers them into this binary).
batap_add_editor(GameExemple_Editor editor_main.cpp)

# Dev mode: the same game as a DLL, hot-reloadable by any editor via --game.
add_library(GameExemple_Game SHARED
  game_module.cpp
)
target_link_libraries(GameExemple_Game PRIVATE
  Batap_Engine
)
