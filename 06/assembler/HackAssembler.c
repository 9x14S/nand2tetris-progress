#include "assembler.h"

#define ASM_EXTENSION ".asm"
#define HACK_EXTENSION ".hack"

int main(int argc, char *argv[]) 
{
    int first_retval = 0, second_retval = 0, return_value = 0;

    // Check that a file has been passed correctly
    if (argc != 2) 
    {
        printf("Usage: %s <filename> \n", argv[0]);
        return_value = 1;
        goto justreturn;
    }

    // Split the file name
    struct v data;
    data.name = data.ext = data.path = '\0';
    data.empty = malloc(sizeof(char));
    *(data.empty) = '\0';
    if (extract_filepath(&data, argv[1], strlen(argv[1]))) 
    {
        return_value = 4;
        goto emptyreturn;
    }

    // Compare the extension
    int result = strcmp(ASM_EXTENSION, data.ext);
    if (result) 
    {
        puts("Invalid file extension. Has to be \".asm\"");
        return_value = 2;
        goto free_data;
    }

    // Open the file for parsing
    FILE *file = fopen(argv[1], "r");

    // File check
    if (file == NULL)
    {
        puts("Couldn't open file.");
        goto free_data;
    }

    first_retval = first_pass(file);
    if (first_retval)
    {
        puts("An error has been encountered, aborting.");
        goto fpasserr;
    }
    
    data.size = 
    (
        (data.ext ? strlen(data.ext) : 0) + 
        (data.name ? strlen(data.name) : 0) + 
        (data.path ? strlen(data.path) : 0)
    );

    second_retval = second_pass(file, &data);
    if (first_retval || second_retval) return_value = 5;
    
fpasserr:
    fclose(file);

free_data:
    free(data.name);
    free(data.path);
emptyreturn:
    free(data.empty);
justreturn:
    return return_value;
}

