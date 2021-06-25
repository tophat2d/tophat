# umka/entity.um

```
 Module for entity functions.
 Entity is basically the main
 object type you will be working
 with.
```

## struct ent*
```go
type ent* = struct {
	p: polygon.poly
	image: image.img
	sx: real
	sy: real
	rot: real 
	color: uint32
	id: int32
}
```

Entity struct. It has polygon,
image, scale, rotation, color
and id. Scale and rotation is
set using functions.
If image is invalid (or missing),
the polygon is drawn with the defined
color. Id is used for collisions.


## struct cent*
```go
type cent* = struct {
	p: ^polygon.cpoly
	image: image.img
	sx: real
	sy: real
	rot: int32	
	color: uint32
	id: int32
}
```



## fn draw*
`fn (e: ^ent) draw*(c: rectangle.rect)`

Draws the entity with camera c in mind.


## fn mk*
`fn mk*(p: polygon.poly, color: uint32, id: int32): ent`

ent's constructor


## fn setrot*
`fn (e: ^ent) setrot*(rot: real)`

Sets absolute rotation of the entity.
It affects the polygon too
and by extension the collision
if you don't want to affect that,
just edit the fields.


## fn setscale*
`fn (e: ^ent) setscale*(sx, sy: real)`

Same as setrot, but for scale.


## fn getcoll*
`fn (e: ^ent) getcoll*(s: []^ent): int32`

Checks, if e collides with any
of the entities in s and returns it's
id. **NOTE**: this function will recieve
a lot of changes. First of all, it will
take a new `entitable` interface, which
needs `getent(): ^entity.ent`. Other is
(and this one might break something)
that it won't return the id, but the
index of the ent, which collided.


## fn retrievecolldata*
`fn retrievecolldata*(): (int32, int32)`

Returns, where the last collision happened.


## fn animate*
`fn (e: ^ent) animate*(fps: int, anim: ^[]image.img)`

Animates the entity's image with one of
the `anim` array. **Won't** begin on the
first frame.


## fn ysort*
`fn ysort*(ents: ^[]entity.ent)`

Performs a y sort on entities.



