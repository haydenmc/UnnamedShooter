# Unnamed Shooter

A project to see if I can make a basic 3D shooter game with a custom software
renderer.

Left-handed coordinate system.

# Building + Running

```ps
meson setup builddir
meson devenv -C builddir
meson compile
.\unnamed-shooter.exe
```

# Notes

- OBJ files have a different coordinate system than ours
  (y grows upwards instead of downwards) and we don't translate on import.
  So imported OBJ objects will appear upside-down.
- To regenerate font file and texture:
  ```
  fontbm.exe --font-file upheavtt.ttf --font-size 20 --output upheaval --data-format bin --monochrome --texture-name-suffix none --spacing-vert 1 --spacing-horiz 1 --chars 32-96,123-126
  ```