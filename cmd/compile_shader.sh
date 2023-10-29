#!/bin/sh

sokol-shdc --input etc/shader.glsl --output src/shader.glsl.h --slang glsl330
sokol-shdc --input etc/shader.glsl --output src/shader-web.glsl.h --slang glsl300es
