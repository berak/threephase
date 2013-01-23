#pragma once

#ifdef _WIN32
 #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>


//
// app interface
//
void appInit();
void appDisplay();
void appIdle();


// 
// main should have this
//
void reshape(int w, int h);