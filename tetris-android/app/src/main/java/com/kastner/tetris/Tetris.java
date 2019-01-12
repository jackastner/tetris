package com.kastner.tetris;

import org.libsdl.app.SDLActivity;

public class Tetris extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] {
                "SDL2",
                "SDL2_ttf",
                "main"
        };
    }
}
