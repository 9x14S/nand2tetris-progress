#include "includes.h"
#include "code.c"

#define MAX_RAM 24576
#define PREDEFS_COUNT 5

// The global RAM counter for the variables
int ram_location = 16;

struct node 
{
    int value;
    struct node *next;
};

// The symbol table
struct node *hash_map[sizeof(struct node) * MAX_RAM];
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
        hash_map[index]->value = i;
    }
    free(rams);

    for (int i = 0; i < PREDEFS_COUNT; i++)
    {
        hash_map[hash(predefs[i])]->value = i;
    }

    hash_map[hash("SCREEN\0")]->value = 16384;
    hash_map[hash("KBD\0")]->value = MAX_RAM;
    return;
}

// Shitty hash function - works for now. 
// TODO: Keep an eye on it, it is a source of hash collisions 
int hash(char *string)
{
    int i = 0, hash_value = 0;
    for (; string[i] != '\0'; i++)
    {
        srand(i);
        hash_value += string[i] * rand();
    }
    srand(i);
    hash_value *= rand();
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
    // Instruction counter
    int line_count = 0;
    // Line counter
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
                fscanf(file, "%s)\n", string_buffer);
                last_parenthesis = strrchr(string_buffer, ')');
                *last_parenthesis = '\0';
                int hashed = hash(string_buffer);
                int val = hash_map[hashed]->value;
                if (!val) hash_map[hashed]->value = line_count;
                else 
                {
                    for (struct node *next = hash_map[hashed]->next; next != NULL; next = next->next)
                    {
                        if (!next->value) 
                        {
                            hash_map[hashed]->value = line_count;
                            break;
                        }
                    }
                }

                lines++;
                break;
            default :
                // The rest, which might be either broken comments, random data or instructions

                // If broken comment
                if (last_char == '/' && buffer != '/')
                {
                    error_s = "Unknown object '/' found";
                    tip = "maybe a comment?";
                    printf("ERROR: %s at line %d (%s)\n", error_s, lines + 1, tip);
                    return_value = 6;
                    return return_value;
                }

                line_count++;

                // Exhaust rest of line until a comment is found or a newline
                while ((buffer = fgetc(file)) != '/' && buffer != '\n' && buffer != EOF);

                if (buffer == '\n') lines++; 
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
                lines++;
                bool switch_flag = false;
                while ((inst_buffer[max] = fgetc(file)) != EOF && max < 64) 
                {
                    if (isspace(inst_buffer[max]))
                    {
                        switch_flag = true;
                        break;
                    }
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

                    hashed = hash(inst_buffer);

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

                    // If the label isn't already in the symbol table,
                    // consider it a variable
                    if (!hash_map[hashed]->value && !tmp && stmp)
                    {
                        hash_map[hashed]->value = ram_location;
                        ram_location++;
                    }
                    else 
                    {
                        for (struct node *next = hash_map[hashed]->next; next != NULL; next = next->next)
                        {
                            if (!next->value) 
                            {
                                next->value = ram_location;
                                ram_location++;
                                break;
                            }
                        }
                    }

                    // Convert to binary and write to file
                    ainst = inttob(hash_map[hashed]->value, 16);
                    fprintf(outfile, "%s\n", ainst);
                    free(ainst);
                }
                last_char = inst_buffer[max - 1];
                break;
            case ' ' :
                break;
            default :
                int i = 0;
                lines++;
            
                // Load data in the instruction buffer 
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
                    line_count++; 
                }

                if (inst_buffer[0] == '(');
                else if (inst_buffer[0] != '\0') 
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