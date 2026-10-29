![borealis logo](https://github.com/natinusala/borealis/blob/main/resources/img/borealis_96.png?raw=true)
# borealis

Controller and TV oriented UI library for PC and Nintendo Switch (libnx).

⚠️ Warning: the project is a WIP - See the Projects tab to follow the journey towards a stable version!

The code for the old version is available in the `legacy` branch.

⚠️ The wiki only contains the documentation for the old version of the library, it has yet to be updated!

- Mimicks the Nintendo Switch system UI, but can also be used to make anything else painlessly
- Hardware acceleration and vector graphics with automatic scaling for TV usage (powered by nanovg)
- Can be ported to new platforms and graphics APIs by providing a nanovg implementation
- Powerful layout engine using flex box as a base for everything (powered by Yoga Layout)
- Automated navigation paths for out-of-the-box controller navigation
- Out of the box touch support
- Define user interfaces using XML and only write code when it matters
- Use and restyle built-in components or make your own from scratch
- Display large amount of data efficiently using recycling lists
- Integrated internationalization and storage systems
- Integrated toolbox (logger, animations, timers, background tasks...)

## Building the demo for Switch

To build for Switch, a standard development environment must first be set up. In order to do so, [refer to the Getting Started guide](https://devkitpro.org/wiki/Getting_Started).

```bash
(sudo) (dkp-)pacman -S switch-glfw switch-mesa switch-glm
make -j
nxlink -s borealis_demo.nro
```

## Building the demo for PC

To build for PC, the following components are required:

- meson/ninja build system
- A C++ compiler supporting the C++17 standard
- GLFW version 3.3 or higher (as a static library)
- GLM version 0.9.8 or higher

Please refer to the usual sources of information for your particular operating system. Usually the commands needed to build this project will look like this:

```bash
meson build
ninja -C build
./build/borealis_demo
```

Also, please note that the `resources` folder must be available in the working directory, otherwise the program will fail to find the shaders.

### Building the demo for Windows using msys2

msys2 provides all packages needed to build this project:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-ninja mingw-w64-x86_64-pkg-config mingw-w64-x86_64-glfw mingw-w64-x86_64-glm
meson build
ninja -C build
./build/borealis_demo
```

### Including in your project (TL;DR: see the demo makefile in this repo)
0. Your project must be built as C++17 (`-std=c++1z`). You also need to remove `-fno-rtti` and `-fno-exceptions` if you have them
1. Use a submodule (or even better, a [subrepo](https://github.com/ingydotnet/git-subrepo)) to clone this repository in your project
2. Copy the `resources` folder to the root of your project
3. For PC (meson):
    1. take a standard meson file
    2. use `subdir` to import the library folder
    3. use the `borealis_files`, `borealis_dependencies`, `borealis_include` and `borealis_cpp_args` variables for respectively objects to build, dependencies (glfw...), includes directory and cpp args
    4. add a `BRLS_RESOURCES` define pointing to the resources folder at runtime (so `resources`)
4. For Switch:
    1. take a standard deko3d homebrew makefile (from the switch-examples repo)
    2. add a `BOREALIS_PATH` variable containing the subfolder you put the library in
    3. set `ROMFS` to the resources folder - borealis resources path is set to `romfs:/` in borealis.mk
    4. use `include` to load `borealis.mk` (after `LIBDIRS` and `BOREALIS_PATH`)
    5. it's important for `OUT_SHADERS` to be set to `shaders` since nanovg will look for the shaders there
    6. if you have issues with the `BRLS_RESOURCES` define (missing quote terminator), change the `@$(MAKE)` line to `@MSYS2_ARG_CONV_EXCL="-D;$(MSYS2_ARG_CONV_EXCL)" $(MAKE)` in the Makefile (as in the demo Makefile)

### Fonts

On Switch, the system shared fonts are used: the font matching the system language is used as the regular font, and the other ones are added as fallbacks following the system order (Chinese and Korean included). When running as an application every shared font is loaded, in applet mode only the ones needed by the current locale are. To save RAM, define one of these when building:

- `SW_STANDARD_FONT_ONLY`: only load the Standard (Latin / Japanese) and symbols shared fonts
- `SW_DISABLE_FONT_FALLBACK`: only load the shared fonts needed by the current locale, even as an application

On PC, the following optional fonts can be put in the resources folder:

- `User-Regular.ttf`: replaces the regular font (Inter)
- `User-Fallback.ttf`: used for the glyphs missing from the regular font, for instance a CJK font
- `User-Switch-Icons.ttf`: Switch buttons icons font

### Touch

Touch screens are supported on Switch. On PC, the mouse acts as a touch screen: clicks are taps and the wheel scrolls. Dragging to scroll is disabled on PC by the `NO_TOUCH_SCROLLING` define set in `library/meson.build`.

## Fork notes

This fork is maintained for personal use on top of the last upstream `main` (April 2021). Changes compared to upstream:

- Builds with recent compilers (GCC 13) and recent libnx
- Merged upstream pull requests: natinusala/borealis#77 (touch control), natinusala/borealis#153 (recycler view), natinusala/borealis#79 (Switch shared fonts fallback), natinusala/borealis#162 (images from memory / RGBA data), natinusala/borealis#165 (German translation), natinusala/borealis#176 (getMarginLeft fix), natinusala/borealis#177 (newer libnx support), with fixes on top
- Fixed upstream issues: natinusala/borealis#116 (highlight pulsation), natinusala/borealis#126 (BoundView::setView), natinusala/borealis#128 (views and Yoga nodes leaks), natinusala/borealis#129 (sticks navigation), natinusala/borealis#147 (focus lost event), natinusala/borealis#154 (framerate counter), natinusala/borealis#171 (libnx build), natinusala/borealis#173 (buttons repetition speed)
