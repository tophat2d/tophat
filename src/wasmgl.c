#ifdef EMSCRIPTEN

#include <GL/gl.h>
#include <emscripten.h>

EM_JS(GLuint, js__th_compile_shader, (const char *src, GLenum type), {
	var obj = window.gl.createShader(type);
	window.gl.shaderSource(obj, Module.UTF8ToString(src));
	window.gl.compileShader(obj);
	if (!window.gl.getShaderParameter(obj, window.gl.COMPILE_STATUS)) {
		console.log("failed compiling a shader", window.gl.getShaderInfoLog(obj));
		return 0;
	}

	return obj;
})

GLuint th_gl_compile_shader(const char *src, GLenum type) {
	return EM_ASM_INT({
		return js__th_compile_shader($0, $1);
	}, src[0], type);
}


EM_JS(GLuint, th_gl_create_prog, (const char *pvert_src, const char *pfrag_src, const char **pattribs, int nattrib), {
	var vert = window.gl.createShader(window.gl.VERTEX_SHADER);
	window.gl.shaderSource(vert, Module.UTF8ToString(pvert_src));
	window.gl.compileShader(vert);
	if (!window.gl.getShaderParameter(vert, window.gl.COMPILE_STATUS)) {
		console.log("failed compiling vertex shader", window.gl.getShaderInfoLog(vert));
		return 0;
	}

	var frag = window.gl.createShader(window.gl.FRAGMENT_SHADER);
	window.gl.shaderSource(frag, Module.UTF8ToString(pfrag_src));
	window.gl.compileShader(frag);
	if (!window.gl.getShaderParameter(frag, window.gl.COMPILE_STATUS)) {
		console.log("failed compiling fragment shader", window.gl.getShaderInfoLog(frag));
		return 0;
	}
	
	var prog = window.gl.createProgram();
	window.gl.attachShader(prog, vert);
	window.gl.attachShader(prog, frag);

	// TODO attribs
	for (var i=0; i < nattrib; i++) {
		window.gl.bindAttribLocation(prog, i, Module.UTF8ToString(HEAP32[(pattribs>>2) + i]));
	}
	
	window.gl.linkProgram(prog);
	if (!window.gl.getProgramParameter(prog, window.gl.LINK_STATUS)) {
		console.log("error linking program");
		return prog;
	}

	window.gl.validateProgram(prog);
	if (!window.gl.getProgramParameter(prog, window.gl.VALIDATE_STATUS)) {
		console.error("error validating program");
		return prog;
	}

	window.gl.useProgram(prog);

	window.progs.push(prog);
	return window.progs.length-1;
})

void th_gl_free_prog(GLuint prog) { }

EM_JS(void, glGenVertexArrays, (int count, GLuint *ret), {
	HEAP32[ret>>2] = window.VAOs.length;
	window.VAOs.push(window.gl.createVertexArray());
})

EM_JS(void, glGenBuffers, (int count, GLuint *ret), {
	HEAP32[ret>>2] = window.VBOs.length;
	window.VBOs.push(window.gl.createBuffer());
})

EM_JS(void, glBindVertexArray, (GLuint vao), {
	window.gl.bindVertexArray(window.VAOs[vao]);
})

EM_JS(void, glBindBuffer, (GLenum type, GLuint vbo), {
	window.gl.bindBuffer(type, window.VBOs[vbo]);
})

EM_JS(void, glBufferData, (GLenum type, GLsizeiptr size, void *ptr, GLenum usage), {
	window.gl.bufferData(type, size, usage);
})

EM_JS(void, glVertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset), {
	window.gl.vertexAttribPointer(index, size, type, normalized, stride, offset);
})

EM_JS(void, glEnableVertexAttribArray, (GLuint idx), {
	window.gl.enableVertexAttribArray(idx);
})

EM_JS(void, glBufferSubData, (GLenum type, GLintptr offset, GLsizeiptr size, const GLvoid *data), {
	window.gl.bufferSubData(type, offset, HEAP8.slice(data, data + size), 0);
})

EM_JS(void, glUseProgram, (GLuint prog), {
	window.gl.useProgram(window.progs[prog]);
})

EM_JS(void, glDrawArrays, (GLenum mode, GLint first, GLsizei count), {
	window.gl.drawArrays(mode, first, count);
})

#endif
