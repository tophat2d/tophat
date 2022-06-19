#include <stddef.h>
#include <stdlib.h>

#include "tophat.h"

#include <GL/gl.h>
#include <chew.h>

void th_gl_init() {
#ifdef _WIN32
	chewInit();
#endif
}

GLuint th_gl_compile_shader(const char **src, GLenum type) {
	GLuint obj = glCreateShader(type);
	if (!obj) {
		th_error("Could not create a shader object.");
		goto fail;
	}

	glShaderSource(obj, 1, src, NULL);
	glCompileShader(obj);

	GLint success = 0;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLint msg_siz = 0;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &msg_siz);

		char *buf = calloc(1, msg_siz);
		if (!buf) {
			th_error("Could not allocate memory for error message.");
			goto fail;
		}
		glGetShaderInfoLog(obj, msg_siz, NULL, buf);

		th_error("Compiling shader: %s", buf);
		free(buf);
		goto fail;
	}

	return obj;

fail:
	glDeleteShader(obj);
	return 0;
}

GLuint th_gl_create_prog(const char *vert_src, const char *frag_src, const char **attribs, int nattribs) {
	GLuint vert = th_gl_compile_shader(&vert_src, GL_VERTEX_SHADER);
	if (!vert)
		return 0;
	GLuint frag = th_gl_compile_shader(&frag_src, GL_FRAGMENT_SHADER);

	GLuint prog = glCreateProgram();
	if (!prog) {
		th_error("Could not create a program.");
		goto fail;
	}

	glAttachShader(prog, vert);
	glAttachShader(prog, frag);

	for (int i=0; i < nattribs; i++)
		glBindAttribLocation(prog, i, attribs[i]);

	glLinkProgram(prog);

	GLint is_linked = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &is_linked);
	if (!is_linked) {
		GLint msg_siz = 0;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &msg_siz);

		char *buf = calloc(1, msg_siz);
		if (!buf) {
			th_error("Could not allocate memory for error message.");
			goto fail;
		}
		glGetProgramInfoLog(prog, msg_siz, NULL, buf);

		th_error("Linking shader program: %s", buf);
		free(buf);
		goto fail;
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
	glUseProgram(prog);
	return prog;

fail:
	if (!vert)
		glDeleteShader(vert);
	if (!frag)
		glDeleteShader(frag);
	if (!prog)
		glDeleteShader(prog);
	return 0;
}

void th_gl_free_prog(GLuint prog) {
	glDeleteShader(prog);
}
