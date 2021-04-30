# UltimateModShop

A Nintendo Switch homebrew project that (will) manage user's installations of mods, and allow users to browse/install mods from Gamebanana.
Built using Natinusala's [Borealis](https://github.com/natinusala/borealis). Huge thanks to them!



## Building
See instructions for building from the borealis readme [here](https://github.com/natinusala/borealis).
And there are some additional compilation instructions at the [borealis wiki](https://github.com/natinusala/borealis/wiki/Additional-compilation-instructions).
(If you need help building it, let me know. Especially building for PC is a bit complicated)



# TODO

- Splash screen while loading initial menu -- implemented, but thread stuff creates a lotta UB with brls right now, so this is on hold until Tasks are implemented natively.

- Reimplement LayerView -- done
- Unzipping that supports all types of compression that gb supports
- Installed mods menu
    - show all installed mods from json, and add to it when downloading new ones -- like a quarter of the way done
    - Uninstall mods
    - enable/disable mods