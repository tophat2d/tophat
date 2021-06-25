# umka/tilemap.um

```
 Tilemaps allow for easy level
 construction. Similar to
 godot or unity. They can
 be loaded from csv or created
 from raw data.
```

## struct tmap*
```go
type tmap* = struct {
	tiles: []image.img // tiles that are used to draw (you can replace them mid game for animation)
	x: int // position
	y: int
	w: int // width of tilemap
	cells: []int32 // all cells (this will draw the tile in tiles with number in cells - 1)
	collmask: []bool // if true, the tile collides
	cellsize: int // cellsize
	scaletype: int // how image scale is handled. 0: stretch, 1: top left
}
```

Tilemap struct


## struct ctmap*
```go
type ctmap* = struct {
	tiles: ^int
	x: int32
	y: int32
	w: int32
	h: int32
	cells: ^int32
	collmask: ^int32
	cellsize: int32
	scaletype: int32
}
```



## fn mk*
`fn mk*(inp: str, images: []image.img): tmap`

Makes a tilemap from csv string


## fn edit*
`fn (t: ^tmap) edit*(x, y, tile: int)`

Sets tile at [x, y] to tile.


## fn draw*
`fn (t: ^tmap) draw*(cam: rectangle.rect)`

Draws the tilemap.


## fn getcoll*
`fn (t: ^tmap) getcoll*(e: entity.ent, rx, ry, tx, ty: ^int32): bool`

Checks, if t collides with e.
[rx, ry] is where the collision
happened (usually one of the
vertices)
[tx, ty] is on which tile it
happened.
You need to pass a valid pointer (TODO)



