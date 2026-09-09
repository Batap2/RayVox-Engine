#include "GameComponents.h"
#include "MyGame.h"

#include "EditorApp.h"

batap::EditorConfig batap::editorConfig()
{
    return {.window_ = {.title = "GameExemple Editor", .fpsInTitle = true, .transparent = true},
            .makeGame_ = batap::makeGame<batap::MyGame>,
            .gameExe_ = "GameExemple"};
}
