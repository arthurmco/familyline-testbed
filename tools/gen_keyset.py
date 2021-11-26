#!/usr/bin/env python

# Generate the key string to key value map in the CommandTable keymap

chars = "qwertyuiopasdfghjklzxcvbnm1234567890"
kp = range(10)
fns = range(1, 15)

for c in chars:
    print("{", end="")
    print("\"{0}\", SDLK_{0}".format(c), end="")
    print("},")

for c in kp:
    print("{", end="")
    print("\"kp-{0}\", SDLK_KP_{0}".format(c), end="")
    print("},")

for c in fns:
    print("{", end="")
    print("\"<f{0}>\", SDLK_F{0}".format(c), end="")
    print("},")

