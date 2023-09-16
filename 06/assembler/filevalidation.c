#include "includes.h"

struct v 
{
    char *path;
    char *name;
    char *ext;
    int size;
    char *empty;
};

// Extracts the filepath, the name and the extension into the struct.
// Outside output validation is required, as any bad input is replaced by an empty string.
int extract_filepath(struct v *data, char *file, int len) 
{
    // Get the directory division
    int lastslash = -1;
    for (int i = 0; i < len; i++) if (file[i] == '/' || file[i] == '\\') lastslash = i;

    char *lastpoint = strrchr(file, '.');
    // Check that there's an extension
    if (!lastpoint)
    {
        puts("The file has no extension. ");
        goto error;
    }

    // Get the filepath if it exists
    char *filepath;
    if (lastslash != -1) 
    {
        if ((filepath = malloc(sizeof(char) * (lastslash + 1))) == NULL)
        {
            puts("A malloc error has occurred when assigning for 'filepath'.");
            goto error;
        }
        else 
        {
            strncpy(filepath, file, lastslash + 1);
            filepath[lastslash + 1] = '\0';
            data->path = filepath;
        }
    }
    else
    {
        filepath = data->empty;
    }

    // Get the extension 
    data->ext = lastpoint;

    // Get the file name 
    char *file_name;
    lastslash = lastslash != -1 ? lastslash + 1 : 0;
    int name_size = (lastpoint - file) - lastslash;
    // Check if the name actually exists 
    if (name_size == 0 || (file_name = malloc(sizeof(char) * (name_size))) == NULL)
    {
        // If the file is a dotfile, switch the extension with the name
        if (name_size == 0)
        {
            data->name = file_name = lastpoint;
            data->ext = lastpoint = data->empty;
        }
        else
        {
            puts("malloc couldn't allocate memory for 'file_name'.");
            file_name = data->empty;
            goto filepath;
        }
    }
    else 
    {
        strncpy(file_name, &file[lastslash], name_size);
        file_name[name_size] = '\0';
        data->name = file_name;
    }

    if (0)
    {
    filepath:
        free(filepath);
    error:
        return 1;
    }
    return 0;
}