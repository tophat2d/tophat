# tophat

Tophat is a friendly 2d game library for the
[Umka](https://github.com/vtereshkov/umka-lang) programming language.  Visit
the [homepage](https://tophat2d.dev) for more info.  You can browse the API
reference [here](https://docs.tophat2d.dev)

## screenshots

<p align="center">
	<img
		src="https://th.mrms.cz/images/space-shooter.png"
		alt="space shooter example"
	>
	<img
		src="https://th.mrms.cz/images/logs-together.png"
		alt="logs together jam game"
	>
	<img
		src="https://th.mrms.cz/images/tetris.png"
		alt="tetris game"
	>
	<img
		src="https://th.mrms.cz/images/pomodoro.png"
		alt="pomodoro app"
	>
</p>

## examples

Creating a moving rectangle is as easy as this:

```go
import ("th.um"; "rect.um"; "input.um"; "canvas.um"; "window.um")

fn main() {
	const speed = 100

	window.setup("my game", 400, 400)
	window.setViewport(th.Vf2{200, 200})

	pos := th.Vf2{100, 100}

	for window.cycle() {
		var change: th.Vf2

		// Handle input
		if input.isPressed(input.key_left)  { change.x -= 1 }
		if input.isPressed(input.key_right) { change.x += 1 }
		if input.isPressed(input.key_up)    { change.y -= 1 }
		if input.isPressed(input.key_down)  { change.y += 1 }

		// Apply movement
		pos = pos.add(change.norm().mulf(speed * th.delta / 1000.0))

		// Draw!
		canvas.drawRect(th.green, rect.mk(pos.x, pos.y, 10, 10))
	}
}
```

Draw an image to the screen using this:

```go
import ("window.um"; "image.um"; "th.um")
fn main() {
    window.setup("image drawing", 400, 400)
    img := image.load("my-image.png")
    
    for window.cycle() {
        img.draw(th.mkTransform(th.Vf2{ 0, 0 }))
    }
}
```

You can look at more complex examples in the
[`examples/`](https://git.sr.ht/~mrms/tophat/tree/main/item/examples) folder.

## license

Tophat is licensed under the BSD-3 license.
