#include "includes.h"
#include "code.c"

#define MAX_RAM 24576
#define PREDEFS_COUNT 5

// The global RAM counter for the variables
int ram_location = 16;

// The symbol table
int hash_map[sizeof(int) * MAX_RAM];
char *predefs[] = {"SP\0", "LCL\0", "ARG\0", "THIS\0", "THAT\0"};
int hash(char *string);

// Set predefined symbols in the table
void set_defs()
{
    char *rams = malloc(sizeof(char) * 4);
    rams[0] = 'R';
    rams[1] = '0';
    rams[2] = '\0';
    rams[3] = '\0';
    for (int i = 0; i < 16; i++)
    {
        if (i < 10) rams[1] = (char)(i + '0');
        else 
        {
            rams[1] = '1';
            rams[2] = (char)((i % 10) + '0');
        }
        int index = hash(rams);
        hash_map[index] = i;
    }
    free(rams);

    for (int i = 0; i < PREDEFS_COUNT; i++)
    {
        hash_map[hash(predefs[i])] = i;
    }

    hash_map[hash("SCREEN\0")] = 16348;
    hash_map[hash("KBD\0")] = MAX_RAM;
    return;
}

// Shitty hash function - needs some optimizations.
int hash(char *string)
{
    int i = 0, hash_value = 0;
    for (; string[i] != '\0'; i++)
    {
        hash_value += string[i];
    }
    hash_value *= i;
    hash_value %= MAX_RAM;
    if (hash_value < 0) hash_value = 0 - hash_value;

    return hash_value;
}

// Counts the number of lines and meaningful lines, plus adds the label info into 
// the hash map. Works good enough.
int first_pass(FILE *file)
{
    
    set_defs(hash_map);

    // Error printing and debugging
    int return_value = 0;
    char *error_s = "empty filler error message";
    char *tip = "empty filler tip message";

    fseek(file, 0, SEEK_SET);

    // Some relevant variables 
    char buffer;
    int line_count = 0;
    int lines = 0;

    // To check whether a comment is in a single line or trailing.
    // Might be useless
    char last_char = '\0';

    // Label checking
    char *string_buffer = malloc(65 * sizeof(char));
    string_buffer[64] = '\0';
    char *last_parenthesis;

    while ((buffer = fgetc(file)) != EOF)
    {
        switch (buffer)
        {
            case '/' :
                if (last_char == '/')
                {
                    // Exhaust all characters until a newline or EOF is found
                    while ((buffer = fgetc(file)) != '\n' && buffer != EOF);
                    if (buffer != EOF) 
                    {
                        lines++;
                    }
                }
                break;
            case '\n' : 
                lines++;
                break;
            case '(' :
                fscanf(file, "%s)", string_buffer);
                last_parenthesis = strrchr(string_buffer, ')');
                *last_parenthesis = '\0';
                int hashed = hash(string_buffer);
                if (hash_map[hashed] == 0)
                {
                    hash_map[hashed] = line_count;
                }
            default :
                if (last_char == '/' && buffer != '/')
                {
                    error_s = "Unknown object '/' found";
                    tip = "maybe a comment?";
                    printf("ERROR: %s at line %d (%s)\n", error_s, lines + 1, tip);
                    return_value = 6;
 
                }
                // Exhaust rest of line until a comment is found or a newline
                else if (buffer != '(') line_count++;
                while ((buffer = fgetc(file)) != '/' && buffer != '\n' && buffer != EOF);
                if (buffer != '/' && buffer != EOF) 
                {
                    lines++; 
                }
        }
        last_char = buffer;
    }
    lines++;

    free(string_buffer);
    return return_value;
}

int second_pass(FILE *file, struct v *data)
{
    // Important variables
    int retval = 0;
    fseek(file, 0, SEEK_SET);
    int empty_ram = 16;

    // Some tool variables
    char buffer, last_char;
    int line_count = 0, lines = 0;

    // Address translation variables
    int inst_addr;
    char *inst_buffer = malloc(sizeof(char) * 65);
    inst_buffer[64] = '\0';
    char *ainst;

    int hashed;

    // Output file name, with (optionally) a path
    char *outfile_name = malloc(data->size + 2);
    char *of_copy = outfile_name;
    outfile_name[data->size + 1] = '\0';
    if (data->path != NULL)
    {
        strcpy(outfile_name, data->path);
        of_copy = outfile_name + strlen(data->path);
    }

    strcpy(of_copy, data->name);
    strcpy(of_copy + strlen(data->name), ".hack");

    // Create output file
    FILE *outfile = fopen(outfile_name, "w");
    free(outfile_name);
    if (!outfile)
    {
        puts("Error opening output file.\n");
        goto outfile_err;
    }

    while ((buffer = fgetc(file)) != EOF)
    {
        switch (buffer)
        {
            case '/' :
                if (last_char == '/')
                {
                    // Exhaust all characters until a newline or EOF is found
                    while ((buffer = fgetc(file)) != '\n' && buffer != EOF);
                    if (buffer != EOF) 
                    {
                        lines++;
                    }
                }
                break;
            case '\n' : 
                lines++;
                break;
            case '@' :
                int max = 0;
                bool switch_flag = false;
                while ((inst_buffer[max] = fgetc(file)) != EOF && max < 6) 
                {
                    switch (inst_buffer[max])
                    {
                        case '/' :
                        case ' ' : 
                        case '\n' : 
                        case '\t' :
                            switch_flag = true;
                            break;
                    }
                    if (switch_flag) break;
                    max++;
                }
                if (switch_flag) inst_buffer[max] = '\0';
                else inst_buffer[max - 1] = '\0';

                // If the A-Instruction is numeric, convert it to int
                if (inst_buffer[0] >= '0' && inst_buffer[0] <= '9')
                {
                    inst_addr = atoi(inst_buffer);

                    // Convert the @ instruction to binary string and write it
                    ainst = inttob(inst_addr, 16);
                    fprintf(outfile, "%s\n", ainst);
                    free(ainst);
                }
                else if (inst_buffer[0] != '\0')
                {
                    int tmp = 0;
                    int stmp = 0;
                    // If the string isn't a previously parsed label, consider it a new variable

                    // Check it isn't a predefined label
                    for (int i = 0; i < PREDEFS_COUNT; i++)
                    {
                        stmp = strcmp(predefs[i], inst_buffer);
                        if (!stmp) break;
                    }
                    if (inst_buffer[0] == 'R') 
                    {
                        tmp = atoi(inst_buffer + 1);
                        if (tmp <= 16 && tmp >= 0) tmp = 1;
                        else tmp = 0;
                    }
                    hashed = hash(inst_buffer);
                    if (!hash_map[hashed] && !tmp && stmp)
                    {
                        hash_map[hashed] = empty_ram;
                        empty_ram++;
                    }

                    // Convert to binary and write to file
                    ainst = inttob(hash_map[hashed], 16);
                    fprintf(outfile, "%s\n", ainst);
                    free(ainst);
                }
                last_char = inst_buffer[max - 1];
                break;
            default :
                int i = 0;
            
                while (buffer != '/' && buffer != '\n' && buffer != EOF && i < 64) 
                {
                    if (buffer != ' ') 
                    {
                        inst_buffer[i] = buffer;
                        i++;
                    }

                    buffer = fgetc(file);
                }
            
                inst_buffer[i] = '\0';
                if (buffer != '/' && buffer != EOF && buffer != '(') 
                {
                    lines++; 
                }

                else if (inst_buffer != NULL && inst_buffer[0] != '\0') 
                {
                    char *cinstp = cinst(inst_buffer, line_count);
                    fprintf(outfile, "%s\n", cinstp);
                    free(cinstp);
                }
        }
        last_char = buffer;
    }
    fclose(outfile);
    free(inst_buffer);
    if (0)
    {
    outfile_err:
        retval = 6;
    }
    return retval;
}