Game Engine
====
Simple implementation of a scrolling shooter game using an over featured game engine.

![](Docs/Demo.gif)

Notable features:
- Font loading and rendering using signed distance fields.
- Cache system for storing expensive calculations between application's executions.
- Very efficient 2D sprite rendering achieved with batching and instancing.
- Scripting Lua engine where the entire game logic resides.
- Entity/Component system for all game objects.
- Elegant logging system using stream operators.
- Quake like console with input and variables.

How to Build
====
1. Download and extract [Boost 1.61](https://sourceforge.net/projects/boost). (Forgive me...)
2. Execute ```bootstrap``` and ```b2 --with-filesystem``` inside Boost directory.
3. Define ```BOOST_ROOT``` environmental variable that points to Boost directory.
4. Run one of the batch files located in Project directory. (e.g. ```Project\CMake-VS2015.bat```)
5. Open the solution file in Build directory.
6. Build and run the application.

License
====

The MIT License (MIT)

Copyright (c) 2016 Piotr Doan

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
