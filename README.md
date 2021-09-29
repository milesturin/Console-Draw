# Console-Draw

A Windows console based graphics library. Utilizes 24-bit color and 16-bit ANSI characters. The library itself provides only basic graphics functions, namely setting single characters. Examples of complex graphics are included (previews below). 

A simulation of elastic collisions between 2d orbs.
![ASCII Orbs](/orbs.gif)

ASCII representation of webcam output with color (webcam interfaced via [escapi](https://github.com/jarikomppa/escapi)).
![Color ASCII Footage](/rgb_example.gif)

A black and white version.
![Black and White ASCII Footage](/bnw_example.gif)

[Dithering](https://en.wikipedia.org/wiki/Dither) is used to increase visual fidelity in the camera demos. It is especially apparent in the 2 character (1-bit) case.
![Dithered 1-bit Footage](/dithering_example.gif)

You can find the source code [here](https://github.com/milesturin/Console-Draw/tree/main/ConsoleDraw/ConsoleDraw).

Library:
* Canvas.hpp
* Canvas.cpp

Demos:
* Orbs.cpp
* Camera.cpp
* (WIP) Fluid.cpp
