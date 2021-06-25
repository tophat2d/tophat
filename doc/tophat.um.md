# umka/tophat.um

```
 Module for interacting with
 the engine.
 Useful variables:
   delta: length of last cycle
   platform: either "linux" or "windows"
```

## fn gettime*
`fn gettime*(): int`

Gets time since the game started **DEPRECATED**


## fn setup*
`fn setup*(title: str, w, h: int32)`

Set's up the engine and opens
a window.


## fn setwindowtitle*
`fn setwindowtitle*(title: str)`

Sets window title on runtime


## fn cycle*
`fn cycle*(w, h: ^int32, bgcolor: uint32, cam: rectangle.rect)`

Function called each cycle.
W and H will return the
window size, bgcolor will
be the background color
of the window and cam
is used for in engine
stuff.



