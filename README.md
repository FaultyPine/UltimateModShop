# UltimateModShop

A Nintendo Switch homebrew project that (will) manage user's installations of mods, and allow users to browse/install mods from Gamebanana.
Built using Natinusala's [Borealis](https://github.com/natinusala/borealis). Huge thanks to them!



## Building
See instructions for building from the borealis readme [here](https://github.com/natinusala/borealis).
And there are some additional compilation instructions at the [borealis wiki](https://github.com/natinusala/borealis/wiki/Additional-compilation-instructions).
(If you need help building it, let me know. Especially building for PC is a bit complicated)



# TODO

- Splash screen while loading initial menu -- on hold since pushing multiple activities to the view stack is borked with the yoga rewrite apparently

- framework for fs stuff... pc & switch.
    - detect/install skyline (initial modding setup) -- need to get unzipping working first
    - make clicking on submissions install the mod. Don't worry about making nice ui for it for now,
        it's fine its just a blocking operation for now
    - (class?) json for installed mods
    - 