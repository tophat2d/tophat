#!/bin/sh

sokol-shdc --input etc/shader.glsl --output src/shader.glsl.h --slang glsl410
sokol-shdc --input etc/shader.glsl --output src/shader-web.glsl.h --slang glsl300es
