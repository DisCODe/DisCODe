DisCODe - Distributed Component Oriented Data Processing 
========================================================

Introduction
------------

This project contains sources of DisCODe - a framework facilitating the development of sensorical data processing algorithms. DisCODE is written mostly in C++ (in an objective manner) and composing of a components library with patterns for their usage. Those patterns impose general implementation method diverse, multistage data processing algorithms.

This software was developed at:

* Robot Control and Pattern Recognition Group
* Institute of Control and Computation Engineering
* Warsaw University of Technology, Poland

This software is distributed under license:

* MIT license

Main authors:

* Tomasz Kornuta
* Maciej Stefańczyk
  
  
Build instructions
------------------

Prepare `build` directory and create makefiles 
	mkdir build && cd build && cmake .. 

Build project (from the `build` directory, binaries will be put into the `build/bin`)
	make

Build documentation (from the `build` directory, documentation will be put into the `build/doc/html`)
	make doc 

