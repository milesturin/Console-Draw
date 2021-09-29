# Console-Draw

A Windows console based graphics library. Utilizes 24-bit color and 16-bit ANSI characters. The library itself provides only basic graphics functions, namely setting single characters. Examples of complex graphics are included (previews below). 

A simulation of elastic collisions between 2d orbs.
![ASCII Orbs](/orbs.gif)

"ASCII" representation of webcam output with color (webcam interfaced via [escapi](https://github.com/jarikomppa/escapi)).
![Color ASCII Footage](/rgb_example.gif)

A black and white version
![Black and White ASCII Footage](/bnw_example.gif)

I added [dithering](https://en.wikipedia.org/wiki/Dither) to increase visual fidelity. It is especially apparent in the 1-bit display case.
![Dithered 1-bit Footage](/dithering_example.gif)

You can find the source code [here](https://github.com/milesturin/Console-Draw/tree/main/ConsoleDraw/ConsoleDraw).

Library:
* Canvas.hpp
* Canvas.cpp

Demos:
* Orbs.cpp
* Camera.cpp
* (WIP) Fluid.cpp
