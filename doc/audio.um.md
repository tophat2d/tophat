# umka/audio.um

```
 Sound module for tophat.
 It serves for all kinds of
 sound operation.
```

## struct sound*
```go
type sound* = struct {
	handle: int
}
```

Sound type. It is an opaque
pointer to c, so don't create
it by hand.


## fn setsounds*
`fn setsounds*(ss: []sound)`



## fn load*
`fn load*(path: str): sound`

Loads sound at path relative to main.um


## fn setsounds*
`fn setsounds*(ss: []sound)`

Sets the array of sounds that are handled
in the background. **DEPRECATED**


## fn looping*
`fn (s: ^sound) looping*(l: bool)`

Sets the sound as looping. Off by default.


## fn play*
`fn (s: ^sound) play*()`

Plays the sound. If it is already
playing, it won't do anything.
I plan on making it replay the
sound if playing.


## fn stop*
`fn (s: ^sound) stop*()`

Stops the sound and **resets** the progress.


## fn vol*
`fn (s: ^sound) vol*(vol: real)`

Set's the volume of the sound as a
multiplier of it's volume.
2 => 2 times the volume
1 => original
0.5 => half the volume
0 => silent


## fn validate*
`fn (s: ^sound) validate*(): bool`

Returns true, if the sound is
valid. That doesn't mean, it
is not corrupted, but it generally
means it was loaded.



