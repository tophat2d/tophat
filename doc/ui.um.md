# umka/ui.um

```
 Small and modulat ui module useful
 for stuff like menus.
 set any element as
 ui.mainelement and it
 will be the root.
```

## interface element*
```go
type element* = interface {
	draw() // draws the element
	calculate() // calculates it's dimensions
	handle() // handle cycle
	gspan(): ^int // returns pointer to it's span
	gpadding(): ^rectangle.rect // returns pointer to it's padding
	gdimensions(): ^rectangle.rect // returns pointer to it's dimensions
}
```

Elements is the interface for all
ui elements. You can make your
own elements.


## struct box*
```go
type box* = struct {
	children: []element
	padding, dimensions: rectangle.rect
	span: int // how many elements in box it spans
	vertical: bool // false: hbox

	image: image.img
	imagemode: int // CENTER, STRETCH, SCALE
	imgs: int // ignore
	imgx, imgy: int // ignore
}
```

Box can be used to oranize
elements in a v/hbox style or
to draw image (or both).
It has children, which are organized
on it's calculate call.


## struct label*
```go
type label* = struct {
	text: str
	textsize: real
	textcolor: uint32
	valign: int // alignment
	halign: int
	bgcolor: uint32
	padding, dimensions: rectangle.rect
	span: int
}
```

Label can be used to draw text.


## struct button*
```go
type button* = struct {
	label: label // Label (this is what is actually drawn)
	toggled: bool // is toggled
	pressed: bool // is pressed
	justpressed: bool // is just pressed
	justreleased: bool // is just release

	padding, dimensions: rectangle.rect
	span: int

	onpress, onjustpress, onrelease: fn() // callbacks
	ontoggle: fn(toggled: bool)
}
```

Button checks for clicks on it's
handle function. Clicks can be recieved
the input way, or as callbacks.


## fn mkbox*
`fn mkbox*(c: []element, vertical: bool): box`



## fn calculate*
`fn (b: ^box) calculate*()`



## fn draw*
`fn (hb: ^box) draw*()`



## fn gspan*
`fn (hb: ^box) gspan*(): ^int`



## fn gpadding*
`fn (hb: ^box) gpadding*(): ^rectangle.rect`



## fn gdimensions*
`fn (hb: ^box) gdimensions*(): ^rectangle.rect`



## fn handle*
`fn (hb: ^box) handle*()`



## fn mklabel*
`fn mklabel*(text: str, textsize: real, textcolor: uint32): label`



## fn calculate*
`fn (l: ^label) calculate*()`



## fn draw*
`fn (l: ^label) draw*()`



## fn gspan*
`fn (l: ^label) gspan*(): ^int`



## fn gpadding*
`fn (l: ^label) gpadding*(): ^rectangle.rect`



## fn gdimensions*
`fn (l: ^label) gdimensions*(): ^rectangle.rect`



## fn handle*
`fn (l: ^label) handle*()`



## fn mkbutton*
`fn mkbutton*(l: label): button`



## fn calculate*
`fn (b: ^button) calculate*()`



## fn draw*
`fn (b: ^button) draw*()`



## fn gspan*
`fn (b: ^button) gspan*(): ^int`



## fn gpadding*
`fn (b: ^button) gpadding*(): ^rectangle.rect`



## fn gdimensions*
`fn (b: ^button) gdimensions*(): ^rectangle.rect`



## fn handle*
`fn (b: ^button) handle*()`



## fn update*
`fn update*(w, h: int32)`

no need to call this


## fn handle*
`fn handle*(w, h: int32)`

call this every loop



