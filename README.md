I've always wanted to build an emulator: specifically for the gameboy, 
but time is limited, and I've never built one before, so in my spare time
this summer I'm going to be building a CHIP-8 emulator, so hopefully I can 
do some other projects I've been wanting to do as well. Inspired by this 
article http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

Resources I've used so far for learning:
- https://github.com/xyproto/sdl2-examples/blob/master/c11/main.c
- https://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world
- https://wiki.libsdl.org/APIByCategory
- https://matthewstyles.com/writing-an-sdl2-game-loop/
- https://lazyfoo.net/tutorials/SDL/46_multithreading/index.php
- https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description
- http://mattmik.com/files/chip8/mastering/chip8.html
- https://github.com/AfBu/haxe-CHIP-8-emulator/wiki/(Super)CHIP-8-Secrets



Random things I've learned:

- macOS will not show the SDL Window with just a delay, you have to have an actual loop
that gives it time to show up
- include path should be "/usr/local/include" on macOS, so this needed to be added to "C_Cpp.default.includePath" in VSCode to make it recognize SDL2