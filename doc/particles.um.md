# umka/particles.um

```
 Particles allow for *performant*
 and random particle systems.
 TODO: a better constructor
```

## struct particle*
```go
type particle* = struct {
	start_time: int32
	seed: int32
}
```

Particle struct. You
can tweak the start_time
for godot like explossivness.


## struct emitter*
```go
type emitter* = struct {
	px, py: int32 // position
	w, h: int32 // size of the emittion area
	gravity_x, gravity_y: real // gravity
	seed: int32 // **DEPRECATED**
	
	angle_min, angle_max: int32 // angle in which particles are emitted

	lifetime: int32 // lifetime of particles
	lifetime_randomness: real // randomness in %/100

	velocity: real // velocity
	velocity_randomness: real // randomness in %/100

	size: real // size
	size_randomness: real // randomness in %/100
	max_size: real // size at the end of particles lifetime

	rotation: int32
	rotation_randomness: real
	max_rotation: int32

	colors: []uint32 // array of colors, which are interpolated between
	
	particles: []particle // list of particles
}
```

Emitter. This is where
everything is configured.


## fn draw*
`fn (e: ^emitter) draw*(cam: rectangle.rect, t: int32)`

Draws and updates the particles.



