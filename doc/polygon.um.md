# umka/polygon.um

```
 Polygon type and functions.
```

## struct poly*
```go
type poly* = struct {
    x: real // position
    y: real
    v: []int32 // vertices [x, y, x, y]. Might be soon converted to real.
		rv: []int32 // don't touch
		rw: int32
		rh: int32 // end of don't touch
    w: int32 // width **TO BE REMOVED**
    h: int32 // height **TO BE REMOVED**
    vc: int32 // vertex count **TO BE REMOVED**
}
```

Polygon type. Mainly
used by ent for collisions
and for fallback drawing
Don't create by hand.


## struct cpoly*
```go
type cpoly* = struct {
	x: int32
	y: int32
	v: ^int32
	w: int32
	h: int32
	vc: int32
}
```

Representation of poly in c.


## fn mk*
`fn mk*(x, y: real, v: []int32): poly`

Makes a polygon. *USE* this.


## fn transform*
`fn (p: ^poly) transform*(sx, sy: real, rot: int32)`

Rotates and scales the polygon (scales first)


## fn cdrawpoly*
`fn cdrawpoly*(color: uint32, p: ^polygon.cpoly)`



## fn draw*
`fn (p: ^poly) draw*(color: uint32)`

Draws this without accounting for camera.



