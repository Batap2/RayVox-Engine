#include "GameComponents.h"

#include "EditorApp.h"

batap::EditorConfig batap::editorConfig()
{
    return {.window_ = {.title = "GameExemple Editor", .fpsInTitle = true, .transparent = true}};
}
