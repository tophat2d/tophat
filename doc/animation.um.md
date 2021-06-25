# umka/animation.um

```
```

## struct anim*
```go
type anim* = struct {
	anim: []image.img
	rot: real
	scale: real
	fps: int
	looping: bool

	px: real
	py: real 

	zerotime: int

	frame: int
}
```

Anim can turn an array of images into an animation.
The difference between anim and ent.animate is that
anim has starting time, so it is really good for
one shot animations.


## fn mk*
`fn mk*(px, py: int32, anm: []image.img, fps: int): anim`

Anim's constructor.


## fn start*
`fn (a: ^anim) start*(t: int)`

Starts animation with start_time of t.


## fn handle*
`fn (a: ^anim) handle*(t: int, cam: rectangle.rect): bool`

This draws the animation. Returns true, if the animation ends.



