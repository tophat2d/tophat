
class Rect {
	ptr
	static new(x, y, w, h) {
		ptr = cnew(x, y, w, h)
	}
	foreign static cnew(x, y, w, h)
	static setpos(x, y) {
		csetpos(ptr, x, y)
	}
	foreign static csetpos(r, x, y)
}


