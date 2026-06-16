# 🏜️ Sand Simulation — C++ & SFML 3.0

Real-time falling sand simulation with multiple materials.

![Demo](images/demo.png)

## ✨ Materials
- 🟡 Sand — gravity ne khali padte
- 🔵 Water — flow hote
- 🔥 Fire — Wood jalvte + Smoke nighte  
- ⬛ Stone — solid wall
- 🟤 Wood — Fire ne jalte
- 🌋 Lava — Water shet Stone bante

## 🎮 Controls
| Key | Action |
|-----|--------|
| 1-6 | Material select |
| Left Click | Place material |
| Right Click | Erase |
| C | Clear all |

## screenshot
https://raw.githubusercontent.com/snehal-thombare08/sand-simulation-cpp/b0e89d8335f1ef0df1f3fa2488e27ca9bd12cfdf/Screenshot%202026-06-16%20174542.png

## 🛠️ Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
mingw32-make
```
