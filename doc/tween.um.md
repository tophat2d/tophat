# umka/tween.um

```
```

## struct twn*
```go
type twn* = struct {
	val: ^real
	lenght: int
	endval: real
	starttime: int
	startval: real
}
```



## fn mk*
`fn mk*(val: ^real, endval: real, lenght: int): twn`



## fn start*
`fn (t: ^twn) start*(time: int)`



## fn get*
`fn (t: ^twn) get*(time: int): (bool, real)`




