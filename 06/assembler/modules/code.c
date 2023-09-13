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

    char *string = malloc(sizeof(char) * width + 1);
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
    for (; compute[arrsize] != '\0' && compute[arrsize] != ';'; arrsize++)
    {
        if (compute[arrsize] == 'M') aiszero = false;
    }

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
    bool pos = false;
    bool and = false;
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
                pos = true;
            case '-' :
            case '&' :
                and = true;
            case '|' :
                thirdchar = true;
                break;
            default :
                puts("COMP ERROR AT SECOND SWITCH");
                return (0 - compute[1]);
        }
    }

    if (thirdchar && arrsize == 3)
    {
        switch (compute[2])
        {
            case '1' :
                if (pos)
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
                else 
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
            case 'A' :
            case 'M' :
                if (pos && !and) 
                {
                    retval = 2;
                }
                else if (!pos && !and)
                {
                    retval = 19;
                }
                else if (!and) 
                {
                    retval = 21;
                }
                break;
            case 'D' :
                if (!pos)
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
        }
    }

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
    if (!(jumping[0] == 'J'))
    {
        retjump = -1;
        printf("Jump error: \"%s\" is not a jump statement\n", jumping);
        return retjump;
    }

    jumping = jumping + sizeof(char);

    switch (jumping[0])
    {
        case 'L' :
        case 'N' : 
        case 'M' : 
            retjump += 4;
            break;
        case 'G' :
        case 'E' :
            break;
        default :
            retjump -= 1000;
    }

    switch (jumping[0]) 
    {
        case 'E' :
            switch (jumping[1])
            {
                case 'G' :
                    retjump += 3;
                    break;
                case 'L' :
                    retjump += 2;
                    break;
                case 'N' : 
                    retjump += 1;
                    break;
                default :
                    retjump -= 1000;
            }
        case 'T' :
            switch (jumping[1])
            {
                case 'G' :
                    retjump += 1;
                    break;
                default : 
                    retjump -= 1000;
            }
        case 'Q' :
            retjump += 2;
            break;
        case 'M' : 
            retjump += 3;
            break;
        default :
            retjump -= 1000;
    }
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
        if (j < 0)
        {
            printf("Error: Jump syntax error -> \"%s\" in line %d\n", jumpf, line);
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