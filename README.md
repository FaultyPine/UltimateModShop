# UltimateModShop

A Nintendo Switch homebrew project that (will) manage user's installations of mods, and allow users to browse/install mods from Gamebanana.
Built using Natinusala's [Borealis](https://github.com/natinusala/borealis). Huge thanks to them!



## Building
See instructions for building from the borealis readme [here](https://github.com/natinusala/borealis).

    The important part there is the parts under `Building the demo for Switch` and `Building the demo for PC`.

There are some additional compilation instructions you'll need to follow at the [borealis wiki](https://github.com/natinusala/borealis/wiki/Additional-compilation-instructions).

    (if you're using WSL, which for this, you probably should) The important parts there are under `Building a native Linux executable`.


For building/running on PC (at least on windows... not sure abt linux), you'll also need [VcXsrv](https://sourceforge.net/projects/vcxsrv/) for GLFW to display the window to.
Once you have VcXsrv, make sure this environment variable is set: `DISPLAY=localhost:0`. If you're using WSL, add `export DISPLAY=localhost:0` to the bottom of your .bashrc file. If building with windows/cmd, [add that environment variable](https://docs.oracle.com/en/database/oracle/machine-learning/oml4r/1.5.1/oread/creating-and-modifying-environment-variables-on-windows.html#GUID-DD6F9982-60D5-48F6-8270-A27EC53807D0) accordingly.

Then launch VcXsrv. It will prompt you to configure some display settings, choose `Multiple Windows`, make sure the display number is `-1`, use `Start no client`, and leave the `extra settings` window as is.

(If you need help building it, let me know. Especially building for PC is a bit complicated)

# TODO

https://trello.com/b/8c35iBqC


-----
As a side note, I know I should be using smart/shared pointers instead of tossing raw ones around. I'm purposely not switching to using those as a personal learning experience.
