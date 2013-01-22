#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

void appInit();
void appDisplay();
void appIdle();

void reshape(int w, int h);