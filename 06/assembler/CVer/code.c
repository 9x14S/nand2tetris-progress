#include "includes.h"

// Convert number into binary string
char *inttob(int num, int width)
{
    int max = (int) pow(2, width);

    if (width < 3 || num > max)
    {
        puts("Error: Width provided too small");
        return NULL;
    }
    
    char *string = malloc(width + 1);
    string[width] = '\0';
    char oneorzero = 0;

    if (num == 0) oneorzero = '0';
    else if (num == max) oneorzero = '1';
    
    if (oneorzero)
    {
        for (int i = 0; i < width; i++) 
        {
            string[i] = oneorzero;
        }
    }
    else 
    {
        max /= 2;
        for (int i = 0; i < width && max != 0; i++)
        {
            if (num / max) string[i] = '1';
            else string[i] = '0';
            num -= num / max * max;
            max /= 2;
        }
    }

    return string;
}

// DONE: 
int comp(char *compute)
{
    if (!compute) return 0;

    compute = compute;
    int retval = 0;

    int arrsize = 0;
    bool aiszero = true;

    // Get the length of the instruction and check if it uses M or A registers
    for (; compute[arrsize] != '\0' && compute[arrsize] != ';'; arrsize++) if (compute[arrsize] == 'M') aiszero = false;

    switch (compute[0])
    {
        case '0' :
            retval = 42;
            break;
        case '1' : 
            retval = 63;
            break;
        case '-' :
        case '!' :
            break;
        case 'M' :
        case 'A' :
            retval = 48;
            break;
        case 'D' :
            retval = 12;
            break;
        default :
            puts("COMP ERROR AT FIRST SWITCH");
            return (0 - compute[0]);
    }

    bool thirdchar = false;
    if (arrsize >= 2)
    {
        if (compute[1] == '\0') return (0 - compute[0]);

        switch (compute[1])
        {
            case '1' : 
                retval = 58;
                break;
            case 'D' :
                retval = 13;
                if (compute[0] == '-') retval += 2;
                break;
            case 'M' :
            case 'A' : 
                retval = 49;
                if (compute[0] == '-') retval += 2;
                break;
            case '+' :
            case '-' :
            case '&' :
            case '|' :
                thirdchar = true;
                break;
            default :
                puts("COMP ERROR AT SECOND SWITCH");
                return (0 - compute[1]);
        }
    }

    if (thirdchar && arrsize >= 3)
    {
        switch (compute[2])
        {
            case '1' :
                if (compute[1] == '+')
                {
                    switch (compute[0])
                    {
                        case 'D' :
                            retval = 31;
                            break;
                        default :
                            retval = 55;
                            break;
                    }
                }
                else if (compute[1] == '-')
                {
                    switch (compute[0])
                    {
                        case 'D':
                            retval = 14;
                            break;
                        default:
                            retval = 50;
                            break;
                    }
                }
                break;
            case 'A' :
            case 'M' :
                switch (compute[1])
                {
                    case '+' :
                        retval = 2;
                        break;
                    case '|' :
                        retval = 21;
                        break;
                    case '-' :
                        retval = 19;
                        break;
                    case '&' :
                        retval = 0;
                        break;
                }
                break;
            case 'D' :
                if (compute[1] == '-')
                {
                    retval = 7;
                    break;
                }
                else
                {
                    if (compute[0] == 'A' || compute[0] == 'M') 
                    {
                        retval = 2;
                        break;
                    }
                    puts("COMP ERROR AT THIRD SWITCH");
                    return (0 - compute[2]);
                }
                break;
        }
    }

    // printf("DEBUG: Comp %s gave %d\n", compute, retval);

    if (!aiszero) retval += 64;
    return retval;
}

// DONE:
int dest(char *destination)
{
    int retdest = 0;
    bool a, d, m;
    a = d = m = false;
    for (int i = 0; destination[i] != '=' && i < 3; i++)
    {
        switch (destination[i])
        {
            case 'A' :
                if (a) return -1;
                retdest += 4;
                a = true;
                break;
            case 'D' :
                if (d) return -1;
                retdest += 2;
                d = true;
                break;
            case 'M' : 
                if (m) return -1;
                retdest += 1;
                m = true;
                break;
                break;
            default :
                retdest = -1;
                return retdest;
        }
    }
    return retdest;
}

// DONE:
int jump(char *jumping)
{
    int retjump = 0;
    if (jumping[0] != 'J' || strlen(jumping) < 3)
    {
        retjump = 0;
        return retjump;
    }

    switch (jumping[2]) 
    {
        case 'E' :
            switch (jumping[1])
            {
                case 'G' :
                    retjump = 3;
                    break;
                case 'L' :
                    retjump = 6;
                    break;
                case 'N' : 
                    retjump = 5;
                    break;
                default :
                    retjump = 0 - jumping[1];
            }
            break;
        case 'T' :
            if (jumping[1] == 'G') retjump = 1;
            else if (jumping[1] == 'L') retjump = 4;
            break;
        case 'Q' :
            if (jumping[1] == 'E') retjump = 2;
            break;
        case 'P' : 
            if (jumping[1] == 'M') retjump = 7;
            break;
        default :
            retjump = 0 - jumping[2];
    }

    if (retjump == 0) retjump = 0 - jumping[2];

    // printf("DEBUG: Jump %s gave %d\n", jumping, retjump);

    return retjump;
}

// Translate C-Instructions
char *cinst(char *inst, int line)
{
    int d, j, c;
    d = j = c = 0;

    // Optional dest part
    char *compf = strrchr(inst, '=');
    if (compf)
    {
        d = dest(inst);
        if (d < 0 || d > 7)
        {
            *compf = '\0';
            printf("Error: Dest syntax error -> \"%s\" in line %d\n", compf, line);
            return NULL;
        }
    }
    
    if (!compf)
    {
        c = comp(inst);
    }
    else
    {
        c = comp(compf + 1);
    }


    // Required comp part
    if (c < 0)
    {
        printf("Error: Comp syntax error \"%c\" -> \"%s\" in line %d\n", (0 - c), inst, line);
        return NULL;
    }

    // Optional jump part
    char *jumpf = strrchr(inst, ';');
    if (jumpf)
    {
        j = jump(jumpf + sizeof(char));
        if (j <= 0)
        {
            if (j != 0) printf("Error: Jump syntax \"%c\" error -> \"%s\" in line %d\n", 0 - j, jumpf, line);
            else printf("Error: Jump syntax error -> \"%s\" in line %d\n", jumpf, line);
            return NULL;
        }
    }

    char *retstr = malloc(17);
    retstr[16] = '\0';
    for (int i = 0; i < 3; i++) retstr[i] = '1';

    int pcount = 3;

    char *binbuff = inttob(c, 7);
    strcpy(retstr + pcount, binbuff);
    pcount += 7;
    free(binbuff);

    binbuff = inttob(d, 3);
    strcpy(retstr + pcount, binbuff);
    pcount += 3;
    free(binbuff);

    binbuff = inttob(j, 3);
    strcpy_s(retstr + pcount, 7, binbuff);
    pcount += 3;
    free(binbuff);

    return retstr;
}