# umka/rawdraw.um

```
 Raw canvas library named after
 cnlohr's rawdraw library,
 which is what tophat uses.
```

## fn drawtext*
`fn drawtext*(text: str, x: int32, y: int32, color: uint32, size: real)`

Draws text. Fonts are comming
eventually.


## fn textsize*
`fn textsize*(text: str, scale: real): (int, int)`

Precomputes a text dimensions.


## fn setup*
`fn setup*(title: str, w, h: int)`

Sets up a window (use tophat's functions instead).


## fn setbgcolor*
`fn setbgcolor*(color: int)`

Sets bg color of the window (already in tophat.cycle)


## fn clearframe*
`fn clearframe*()`

Clears the screen (already in tophat.cycle)


## fn setcolor*
`fn setcolor*(color: int)`

Sets drawing color.


## fn getdimensions*
`fn getdimensions*(w, h: ^int32)`

Returns raw window dimension (already in tophat.cycle)


## fn swapbuffers*
`fn swapbuffers*()`

Finishes the cycle (already in tophat.cycle)


## fn handleinput*
`fn handleinput*()`

Handles input (already in tophat.cycle)


## fn drawrect*
`fn drawrect*(x1, y1, x2, y2: real)`

Draws a rectangle.


## fn drawline*
`fn drawline*(x1, y1, x2, y2: int32, thickness: real, color: uint32)`

Draws a line.


## fn cdrawimage*
`fn cdrawimage*(img: int, rot: int32, scale: real, x, y: int32)`

Ignore.



