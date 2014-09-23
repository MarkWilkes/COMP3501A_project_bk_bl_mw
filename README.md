# COMP3501A Game Project

**Fall 2014 (September - December)**

### Authors
- Brandon Keyes, ID: 100897196
- Bernard Llanos, ID: 100793648
- Mark Wilkes, ID: 100884169

## Description
An outer space-themed game. (Refer to project proposal for more details.)

## Building and Operating Instructions
The program should build and run with Visual Studio 2012 or higher,
on Windows 7 or higher. Development was undertaken primarily on Windows 7
with Visual Studio 2013.

For Visual Studio 2012, it will be necessary to 'downgrade' the projects
in the solution by changing their 'Platform Toolset' values (`Configuration Properties > General`).

Any broken links, �File not found�-type errors, (e.g. �Cannot open��) can normally be resolved
by removing the files in question from the project using the 'Solution Explorer' pane and then
using the `Project > Add Existing Item` wizard to reinsert them into the project under the
correct file paths.

## Code References
The main project code, the Visual Studio 'engine' project,
was adapted from the following sources:
- Bernard Llanos, Alec McGrail, Benjamin Smith,
  group project for the course COMP2501A (Winter 2014)
  - This was, in turn, based on the course tutorials,
    which were based on the Rastertek tutorials.
	(http://www.rastertek.com, currently unavailable)

The game engine relies on framework code, the Visual Studio 'win32_base' project.
This code was written by Bernard Llanos from May to September 2014 and is further
described in `win32_base\README.md`.
Presently, it is also available at https://github.com/bllanos/win32_base

The following works were consulted for ideas during development:
- Luna, Frank D. _3D Game Programming with DirectX 11._
  Dulles: Mercury Learning and Information, 2012.
- Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

## Sources of Assets
There are no assets, currently.