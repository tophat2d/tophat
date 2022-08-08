# tophat

Tophat is a friendly 2d game engine for the [Umka](https://github.com/vtereshkov/umka-lang) programming language.
Visit the [homepage](https://th.mrms.cz/) for more info.

## screenshots

![space shooter example](https://th.mrms.cz/images/space-shooter.png)
![logs together jam game](https://th.mrms.cz/images/logs-together.png)

## examples

Creating a moving rectangle is as easy as this:

```rust
import (
	"th.um"
	"rect.um"
	"canvas.um"
	"window.um"
)

fn main() {
	window.setup("my game", 400, 400)

	pos := th.Vf2{200, 200}
	const speed = 0.02

	for window.cycle(rect.mk(0, 0, 400, 400)) {
		if input.isPressed(input.key_left)  { pos.x -= speed * th.delta }
		if input.isPressed(input.key_right) { pos.x += speed * th.delta }
		if input.isPressed(input.key_up)    { pos.y -= speed * th.delta }
		if input.isPressed(input.key_down)  { pos.y += speed * th.delta }

		canvas.drawRect(th.green, rect.mk(pos.x, pos.y 10, 10))
	}
}
```

You can look at more complex examples in the [`examples/`](https://github.com/marekmaskarinec/tophat/tree/main/examples) folder.

## license

Tophat is licensed under the BSD-3 license.
