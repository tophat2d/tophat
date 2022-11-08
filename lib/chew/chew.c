/*
The MIT License (MIT)

Copyright (c) 2010-2020 <>< CNLohr
Copyright (c) 2004-2008 Joshua Allen (portions of CNFG3D)
Copyright (c) 2011-2013 Luc Verhaegen <libv@skynet.be> (Android)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#if defined( WIN32 ) || defined( WINDOWS ) || defined( WIN64 )
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <stdio.h>
#include "chew.h"

#define TABLEONLY
#undef CHEWTYPEDEF
#undef CHEWTYPEDEF2
#define CHEWTYPEDEF( ret, name, retcmd, parameters, ... ) name##_t name##fnptr;
#define CHEWTYPEDEF2( ret, name, usename, retcmd, parameters, ... ) usename##_t usename##fnptr;
#include "chew.h"

#define TABLEONLY
#undef CHEWTYPEDEF
#undef CHEWTYPEDEF2
#define CHEWTYPEDEF( ret, name, retcmd, parameters, ... ) ret name( __VA_ARGS__ ) { retcmd name##fnptr parameters; }
#define CHEWTYPEDEF2( ret, name, usename, retcmd, parameters, ... ) ret usename( __VA_ARGS__ ) { retcmd usename##fnptr parameters ; }
#include "chew.h"

#define TABLEONLY
#undef CHEWTYPEDEF
#undef CHEWTYPEDEF2
#define CHEWTYPEDEF( ret, name, retcmd, parameters, ... ) #name, 
#define CHEWTYPEDEF2( ret, name, usename, retcmd, parameters, ... ) #name, 
const char * symnames[] = {
	#include "chew.h"
};

#define TABLEONLY
#undef CHEWTYPEDEF
#undef CHEWTYPEDEF2
#define CHEWTYPEDEF( ret, name, retcmd, parameters, ... ) (void**)&name##fnptr, 
#define CHEWTYPEDEF2( ret, name, usename, retcmd, parameters, ... ) (void**)&usename##fnptr, 
void ** syms[] = {
	#include "chew.h"
};

#if defined( WIN32 ) || defined( WINDOWS ) || defined( WIN64 )

//From https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions
void * chewGetProcAddress(const char *name)
{
	void *p = (void *)wglGetProcAddress(name);
	if(p == 0 ||
		(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
		(p == (void*)-1) )
	{
		static HMODULE module;
		if( !module ) module = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(module, name);
	}
	return p;
}

#else

void * chewGetProcAddress(const char *name)
{
	//Tricky use RTLD_NEXT first so we don't accidentally link against ourselves.
	void * v1 = dlsym( (void*)((intptr_t)-1) /*RTLD_NEXT = -1*/ /*RTLD_DEFAULT = 0*/, name );
	//printf( "%s = %p\n", name, v1 );
	if( !v1 ) v1 = dlsym( 0, name );
	return v1;
}

#endif



void chewInit() {
	int i;
	for( i = 0; i < sizeof(symnames) / sizeof(symnames[0]); i++ )
	{
		void * v = *syms[i] = (void*)chewGetProcAddress( symnames[i] );
		if( !v ) fprintf( stderr, "Warning: Can't find \"gl%s\"\n", symnames[i] );
		//else printf( "%p = %s\n", v, symnames[i] );
	}
}

