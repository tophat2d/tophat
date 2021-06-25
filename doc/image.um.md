# umka/image.um

```
 Module for loading and operating
 with images.
```

## struct img*
```go
type img* = struct {
	handle: int
}
```

Img is an opaque pointer
to c same as sound. It needs
to be freed by img.delete()


## fn load*
`fn load*(path: str): img`

Loads an image at path.


## fn delete*
`fn (i: ^img) delete*()`

Frees i from memory.


## fn setasicon*
`fn (i: ^img) setasicon*()`

Set's i as window icon.
Works only on linux.
**DEPRECATED**


## fn flipv*
`fn (i: ^img) flipv*()`

Flips i on it's vertical axis.


## fn fliph*
`fn (i: ^img) fliph*()`

Flips i on it's horizontal axis.


## fn draw*
`fn (i: ^img) draw*(x, y, rot: int32, scale: real)`

Draws the image **not** accounting for camera.


## fn validate*
`fn (i: ^img) validate*(): bool`

Returns true, if i is valid.


## fn getdims*
`fn (i: ^img) getdims*(): (int32, int32)`

Returns the dimensions in pixels of i.


## fn crop*
`fn (i: ^img) crop*(x1, y1, x2, y2: int32)`

Crops out the rectangle from image.


## fn fromdata*
`fn fromdata*(data: []uint32, w: int): img`

Creates an image from raw data.
**BUGGY**


## fn copy*
`fn (i: ^img) copy*(): img`

Copies image into a new one.



