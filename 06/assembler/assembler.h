#include "filevalidation.c"
#include "parser.c"

int extract_filepath(struct v *data, char *filename, int len);

int first_pass(FILE *file);

int second_pass(FILE *file, struct v *data);