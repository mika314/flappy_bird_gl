#pragma once

#define GL_CHECK_ERROR() checkError(__FILE__, __LINE__)

void checkError(const char* file, int line);
