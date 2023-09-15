#pragma once

#define TO_STRING(text) #text
// X TO STRING for macro expansion
#define X_TO_STRING(X) TO_STRING(X)
#define WARN_IF(condition) {if (condition) fprintf(stderr, "Warning: " #condition "\n");}
#define WARN(s) {fprintf(stderr, "Warning: " #s "\n");}
#define CONCAT_TO_STRING(x, y) #x #y
#define CONCAT(x, y) x##y
#define PRINT_ERROR(s) fprintf(stderr, "%s \n", s)

// not used
struct color
{
	float r, g, b, a;
};