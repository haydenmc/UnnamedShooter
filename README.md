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