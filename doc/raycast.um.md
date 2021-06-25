# umka/raycast.um

```
 Rays allow for vroom fast
 collisions.
```

## struct ray*
```go
type ray* = struct {
	x: real // position
	y: real
	l: int32 // length
	r: real // rotation
}
```

Ray type.


## fn mk*
`fn mk*(x, y, l, r: int32): ray`

Makes a ray. Just for consistency.


## fn getcoll*
`fn (r: ^ray) getcoll*(s: []^entity.ent): int32`

Same as ent.getcoll. Same changes will apply.


## fn retrievecolldata*
`fn retrievecolldata*(): (int32, int32)`

Returns, where the last collision happened.



