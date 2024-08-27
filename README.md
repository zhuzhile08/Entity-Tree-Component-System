# Entity-Tree-Component-System

ETCS (Entity-Tree-Component-System) is a small, simple and fast-ish Archetype-ECS for >= `C++23`.

## Introduction

### Features and design choices

- All exposed entities (`etcs::Entity`) are in reality entity "views", and the real entity data is packed in an dense array
- Entities support hierarcies and lookups by default
- Cache-friendly component storage due to the archetype implementation
- Simple-to-understand and small codebase
- One global `etcs::World` which may be created using `etcs::init()` and destroyed using `etcs::quit()` for basic usage, more can be created when required

ETCS is still quite basic right now, so it doesn't support more advanced features like tags, thread safety and advanced queries.

### Example

```c++
#include <ETCS/ETCS.h> // includes all components of the library
#include <cstdio>

struct Position {
public:
    double x = { }, y = { };
};

struct Velocity {
public:
    double x = { }, y = { };
};

int main() {
    etcs::init();

    auto root = etcs::insertEntity();


    for (etcs::object_id i = 0; i < 1024; i++) {
        auto e = root.insertChild(std::to_string(i)); // use lsd::toString if you have it as a dependency
        
        if (i % 1 == 0) e.insertComponent<Position>(
		    Position { static_cast<double>(i), static_cast<double>(i) / 2}
		);
        if (i % 2 == 0) e.insertComponent<Velocity>(
		    Velocity { static_cast<double>(i * i), static_cast<double>(i) / 3}
		);
        if (i % 3 == 0) e.disable();
    }


    auto system = etcs::insertSystem<const Velocity, Position>();
    system.each([](etcs::Entity e, const Velocity& v, Position p){
        p.x += v.x;
        p.y += v.y;

        std::printf("Hello, Entity number %s at position (%.0f, %.0f)!\n", e.name().data(), p.x, p.y);
    });

    etcs::quit();
}
```

## Integration using CMake

This project was designed to be used with CMake. Other build systems have not been tested.

### Basic linkage

Adding the Library as a CMake dependency defines the targets `ETCS::ETCS-<build_type>` and `ETCS::Headers`, which you can link to your project.

You can set the build type by configuring the option `ETCS_STATIC`

### Optional dependencies and extensions

ETCS was designed to be used with the [Lyra-Standard-Library](https://github.com/zhuzhile08/Lyra-Standard-Library), although it can be used without it, since the required structures have been seperatly implemented using the standard library. You can include LSD in your base project and add it as a CMake depdendency **before** adding ETCS so that it can be found.

ETCS also has some [basic premade components](https://github.com/zhuzhile08/Entity-Tree-Component-System/tree/main/include/ETCS/Components) available by default, although they require [GLM](https://github.com/g-truc/glm) as a dependency and have to be enabled by setting the CMake option `ETCS_ENABLE_COMPONENTS_EXT` (you can find it in the [CMakeLists.txt](https://github.com/zhuzhile08/Entity-Tree-Component-System/blob/main/CMakeLists.txt)).

## Contributing

If you find anything you can contribute about the project, you can just open an issue or a PR, although I can't guarantee that I will respond to it, since this project is not my main focus, but everybody is welcome.
