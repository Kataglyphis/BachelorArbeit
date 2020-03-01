# Bachelorthesis

--------
## About me
--------
 - undergraduate IT student interested in :
    - modern graphics hardware
    - real time applications 
    - path tracing

## Prerequisites

- Falcor 
- VS 2017/19
- DirectXRaytracing (DXR)

--------------------------
## Purpose of this project
--------------------------
Rendering using hardware-assisted ray tracing and associated techniques are currently gaining in importance in real-time computer graphics.
Despite this new hardware support, only a little computing time is given to calculate one
single image. Along with this short computing time, there are fewer ray paths with accordingly fewer
Length. Previous work already has shown how to counteract the resulting image noise,
included the blue noise error distributions and emphasized and clarified their importance in increasing the perceptible image quality.
This work explains a temporally stable algorithm based on this technique. In contrast to the previous approaches, we want to apply an error redistribution directly in the image space, and so one accordingly to get correlated pixel sequence. The algorithm achieves all of this without significant additional computing effort.
 
--------------------
## Algorithmic Analysis
--------------------
- Simulated Annealing
- Path Tracing

--------------------
### Installing
--------------------

```
Give the example
```

And repeat

```
until finished
```
--------------------
## Running the program
--------------------
Falcor Project

![Get your shit together ... link your pictures right Jonas](Bilder/Anleitung.png?raw=true "UI")

Red - Toogle the retarget pass, will only be affective if white noise disabled

Green - add temporal reprojection to your retargeting pass 

Blue - Toogle to a conventional approach with a white noise 

Yellow - toogle to sorting pass, will only be affective if white noise is disabled 

RetargetingGui

Gui support is still under development therefore use the 

Retargeting

Under the main file one finds the different tasks to run (i.e. Simulated Anneling Tester, Visualizer, etc.) 

--------------------
## Built With
--------------------

* [Falcor](https://developer.nvidia.com/falcor) - The real-time rendering framework used
* [Matplotlib](https://github.com/lava/matplotlib-cpp) - C++ Wrapper Class for the python lib

--------------------
## Authors
--------------------

* **Jonas Heinle** - [Kataglyphis](https://github.com/Kataglyphis)

--------------------
## License
--------------------

This project is licensed under the BSD License - see the [LICENSE.md](LICENSE.md) file for details

--------------------
## Acknowledgments
--------------------

This work was inspired by previous work on blue noise. Especially to mention:
* https://eheitzresearch.wordpress.com/772-2/
* https://dl.acm.org/doi/10.1145/2897839.2927430
