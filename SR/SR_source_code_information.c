/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "SR_defs.h"
#include "SR_vars.h"

static int SR_LoadSCI_fixup_interpret_as_code(FILE *file)
{
    char buf[8192];
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            section_code_list_Insert(SecNum, RelAdr);
        }
    }

    return 0;
}

static int SR_LoadSCI_displaced_labels(FILE *file)
{
    char buf[8192];
    int *label_value;
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items, displacement;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X,%i", &address, &displacement);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            label_value = section_label_list_Insert(SecNum, RelAdr);

            if (label_value == NULL) return 1;

            *label_value = displacement;
        }
    }

    return 0;
}

static int SR_LoadSCI_noret_procedures(FILE *file)
{
    char buf[8192];
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            section_noret_list_Insert(SecNum, RelAdr);
        }
    }

    return 0;
}

static int SR_LoadSCI_code16_areas(FILE *file)
{
    char buf[8192];
    int *code16_area_value;
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items, arealength;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X,%i", &address, &arealength);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            code16_area_value = section_code16_list_Insert(SecNum, RelAdr);

            if (code16_area_value == NULL) return 1;

            *code16_area_value = arealength;
        }
    }

    return 0;
}

static int SR_LoadSCI_fixup_do_not_interpret_as_code(FILE *file)
{
    char buf[8192];
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            section_nocode_list_Insert(SecNum, RelAdr);
        }
    }

    return 0;
}

//#if (OUTPUT_TYPE != OUT_DOS)
static int SR_LoadSCI_external_procedures(FILE *file)
{
    char buf[8192];
    char *str1, *str2;
    extrn_data *extrn;
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 == NULL) continue;
        *str1 = 0;
        str1++;

        str2 = strchr(str1, ',');
        if (str2 != NULL)
        {
            *str2 = 0;
            str2++;

            while (*str2 == ' ') str2++;
            if (*str2 == 0) str2 = NULL;
        }

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            extrn = section_extrn_list_Insert(SecNum, RelAdr, str1, str2);
            if (extrn == NULL) return 1;
        }
    }

    return 0;
}

static uint_fast32_t *force_function_list = NULL;
static unsigned int force_function_count = 0;

static int SR_LoadSCI_force_functions(FILE *file)
{
    char buf[8192];
    size_t length;
    int items;
    unsigned int address;

    while (!feof(file))
    {
        /* read enters */
        items = fscanf(file, "%8191[\n]", buf);

        /* read line */
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;

        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        if (sscanf(buf, "loc_%X", &address) != 1) continue;

        force_function_list = (uint_fast32_t *) realloc(
            force_function_list,
            sizeof(uint_fast32_t) * (force_function_count + 1)
        );

        if (force_function_list == NULL)
        {
            force_function_count = 0;
            return 1;
        }

        force_function_list[force_function_count] = address;
        force_function_count++;
    }

    return 0;
}

void SR_apply_force_functions(void)
{
    unsigned int index;
    unsigned int section_index;

    for (index = 0; index < force_function_count; index++)
    {
        for (section_index = 0; section_index < num_sections; section_index++)
        {
            if (force_function_list[index] >= section[section_index].start &&
                force_function_list[index] <= section[section_index].start + section[section_index].size)
            {
                SR_disassemble_add_address(
                    section_index,
                    force_function_list[index]
                );

                break;
            }
        }
    }

    if (force_function_list != NULL)
    {
        free(force_function_list);
        force_function_list = NULL;
    }

    force_function_count = 0;
}

static int SR_LoadSCI_global_aliases(FILE *file)
{
    char buf[8192];
    char *str1;
    alias_data *alias;
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 == NULL) continue;
        *str1 = 0;
        str1++;

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            alias = section_alias_list_Insert(SecNum, RelAdr, str1);
            if (alias == NULL) return 1;
        }
    }

    return 0;
}

static int SR_LoadSCI_instruction_replacements(FILE *file)
{
    char buf[8192];
    char *str1, *str2, *str3;
    replace_data *replace;
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items, instr_empty;
    unsigned int address, instr_len;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 == NULL) continue;
        *str1 = 0;
        str1++;

        str2 = strchr(str1, ',');

        if (str2 == NULL) continue;
        *str2 = 0;
        str2++;

        instr_empty = 0;

        str3 = strchr(str2, '|');
        if (str3 == NULL)
        {
            str3 = str2;
            while (*str3 == ' ') str3++;
            if ((*str3 == ';') || (*str3 == '@'))
            {
                instr_empty = 1;
            }
        }
        else
        {
            while (str3 != NULL)
            {
                *str3 = '\n';
                str3++;

                str3 = strchr(str3, '|');
            }
        }

        sscanf(buf, "loc_%X", &address);
        sscanf(str1, "%i", &instr_len);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            replace = section_replace_list_Insert(SecNum, RelAdr, str2, instr_len, instr_empty);
            if (replace == NULL) return 1;
        }
    }

    return 0;
}

#if ((OUTPUT_TYPE == OUT_ARM_LINUX) || (OUTPUT_TYPE == OUT_LLASM))
static int SR_LoadSCI_unaligned_ebp_areas(FILE *file)
{
    char buf[8192];
    int *ua_ebp_area_value;
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items, arealength;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X,%i", &address, &arealength);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            ua_ebp_area_value = section_ua_ebp_list_Insert(SecNum, RelAdr);

            if (ua_ebp_area_value == NULL) return 1;

            *ua_ebp_area_value = arealength;
        }
    }

    return 0;
}

static int SR_LoadSCI_unaligned_esp_areas(FILE *file)
{
    char buf[8192];
    int *ua_esp_area_value;
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items, arealength;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X,%i", &address, &arealength);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            ua_esp_area_value = section_ua_esp_list_Insert(SecNum, RelAdr);

            if (ua_esp_area_value == NULL) return 1;

            *ua_esp_area_value = arealength;
        }
    }

    return 0;
}
#endif

static int SR_LoadSCI_instruction_flags(FILE *file)
{
    char buf[8192];
    char *str1, *str2, *str3;
    bound_data *iflags;
    uint_fast32_t SecNum, RelAdr;
    size_t length;
    int items;
    unsigned int address, to_set, to_clear;

    while (!feof(file))
    {
        // read enters
        items = fscanf(file, "%8191[\n]", buf);
        // read line
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 == NULL) continue;
        *str1 = 0;
        str1++;

        str2 = strchr(str1, ',');

        if (str2 == NULL) continue;
        *str2 = 0;
        str2++;

        str3 = strchr(str2, ';');

        if (str3 != NULL)
        {
            *str3 = 0;
        }

        sscanf(buf, "loc_%X", &address);
        sscanf(str1, "%i", &to_set);
        sscanf(str2, "%i", &to_clear);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            iflags = section_iflags_list_Insert(SecNum, RelAdr, to_set, to_clear);
            if (iflags == NULL) return 1;
        }
    }

    return 0;
}
//#endif

static int SR_LoadSCI_symbol_renames(FILE *file)
{
    char buf[8192];
    char *str1;
    char *str2;
    size_t length;
    int items;

    while (!feof(file))
    {
        /* read enters */
        items = fscanf(file, "%8191[\n]", buf);

        /* read line */
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;

        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');
        if (str1 == NULL) continue;

        *str1 = 0;
        str1++;

        while (*str1 == ' ') str1++;

        str2 = str1 + strlen(str1);
        while (str2 > str1 && str2[-1] == ' ')
        {
            str2--;
        }
        *str2 = 0;

        if (*buf == 0 || *str1 == 0) continue;

        /* Normalize hexadecimal address to uppercase. */
        for (char *p = buf+3; *p != 0; p++)
        {
            if (*p >= 'a' && *p <= 'f')
                *p = toupper(*p);
        }

        SR_add_symbol_rename(buf, str1);
    }

    return 0;
}

static int SR_LoadSCI_input_patches(FILE *file)
{
    char buf[8192];
    char *str1;
    char *str2;
    char *str3;
    size_t length;
    int items;
    unsigned int address;
    unsigned int patch_length;
    unsigned int byte_value;
    uint_fast32_t SecNum, RelAdr;
    unsigned int index;

    while (!feof(file))
    {
        /* read enters */
        items = fscanf(file, "%8191[\n]", buf);

        /* read line */
        buf[0] = 0;
        items = fscanf(file, "%8191[^\n]", buf);
        if (items <= 0) continue;

        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');
        if (str1 == NULL) continue;

        *str1 = 0;
        str1++;

        str2 = strchr(str1, ',');
        if (str2 == NULL) continue;

        *str2 = 0;
        str2++;

        sscanf(buf, "loc_%X", &address);
        sscanf(str1, "%i", &patch_length);

        if (patch_length == 0) continue;

        if (!SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            fprintf(stderr,
                    "Warning: input patch address loc_%X is outside all sections\n",
                    address);
            continue;
        }

        if (RelAdr + patch_length > section[SecNum].size)
        {
            fprintf(stderr,
                    "Warning: input patch loc_%X,%u extends past section boundary\n",
                    address, patch_length);
            continue;
        }

        str3 = str2;

        for (index = 0; index < patch_length; index++)
        {
            while (*str3 == ' ' || *str3 == '\t')
            {
                str3++;
            }

            if (*str3 == 0)
            {
                fprintf(stderr,
                        "Warning: input patch loc_%X is missing byte %u\n",
                        address, index);
                break;
            }

            if (sscanf(str3, "%2X", &byte_value) != 1)
            {
                fprintf(stderr,
                        "Warning: invalid byte in input patch loc_%X\n",
                        address);
                break;
            }

            printf("replacing byte at loc_%X with %u\n", address, byte_value);

            section[SecNum].adr[RelAdr + index] = (uint8_t)byte_value;

            while (*str3 != 0 &&
                   *str3 != ' ' &&
                   *str3 != '\t')
            {
                str3++;
            }
        }
    }

    return 0;
}

int SR_LoadSCI(void)
{
    const static char fixup_interpret_as_code[] = "fixup_interpret_as_code.sci";
    const static char displaced_labels[] = "displaced_labels.sci";
    const static char noret_procedures[] = "noret_procedures.sci";
    const static char code16_areas[] = "code16_areas.sci";
    const static char fixup_do_not_interpret_as_code[] = "fixup_do_not_interpret_as_code.sci";
    const static char symbol_renames[] = "symbol_renames.sci";
    const static char input_patches[] = "input_patches.sci";
//#if (OUTPUT_TYPE != OUT_DOS)
    const static char external_procedures[] = "external_procedures.sci";
    const static char force_functions[] = "force_functions.sci";
    const static char global_aliases[] = "global_aliases.sci";
    const static char instruction_replacements[] = "instruction_replacements.sci";
    const static char instruction_replacements_FPU[] = "instruction_replacements_FPU.sci";
#if ((OUTPUT_TYPE == OUT_ARM_LINUX) || (OUTPUT_TYPE == OUT_LLASM))
    const static char unaligned_ebp_areas[] = "unaligned_ebp_areas.sci";
    const static char unaligned_esp_areas[] = "unaligned_esp_areas.sci";
#endif
    const static char instruction_flags[] = "instruction_flags.sci";
//#endif
    FILE *f;
    int ret;


    f = fopen(fixup_interpret_as_code, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", fixup_interpret_as_code);

        ret = SR_LoadSCI_fixup_interpret_as_code(f);

        fclose(f);

        if (ret) return ret;
    }


    f = fopen(displaced_labels, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", displaced_labels);

        ret = SR_LoadSCI_displaced_labels(f);

        fclose(f);

        if (ret) return ret;
    }


    f = fopen(noret_procedures, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", noret_procedures);

        ret = SR_LoadSCI_noret_procedures(f);

        fclose(f);

        if (ret) return ret;
    }


    f = fopen(code16_areas, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", code16_areas);

        ret = SR_LoadSCI_code16_areas(f);

        fclose(f);

        if (ret) return ret;
    }


    f = fopen(fixup_do_not_interpret_as_code, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", fixup_do_not_interpret_as_code);

        ret = SR_LoadSCI_fixup_do_not_interpret_as_code(f);

        fclose(f);

        if (ret) return ret;
    }

    
    f = fopen(symbol_renames, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", symbol_renames);

        ret = SR_LoadSCI_symbol_renames(f);

        fclose(f);

        if (ret) return ret;
    }
    

    f = fopen(input_patches, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", input_patches);

        ret = SR_LoadSCI_input_patches(f);

        fclose(f);

        if (ret) return ret;
    }


//#if (OUTPUT_TYPE != OUT_DOS)
    f = fopen(external_procedures, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", external_procedures);

        ret = SR_LoadSCI_external_procedures(f);

        fclose(f);

        if (ret) return ret;
    }


    f = fopen(force_functions, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", force_functions);

        ret = SR_LoadSCI_force_functions(f);

        fclose(f);

        if (ret) return ret;
    }


    f = fopen(global_aliases, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", global_aliases);

        ret = SR_LoadSCI_global_aliases(f);

        fclose(f);

        if (ret) return ret;
    }


    f = fopen(instruction_replacements, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", instruction_replacements);

        ret = SR_LoadSCI_instruction_replacements(f);

        fclose(f);

        if (ret) return ret;
    }


    f = fopen(instruction_replacements_FPU, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", instruction_replacements_FPU);

        ret = SR_LoadSCI_instruction_replacements(f);

        fclose(f);

        if (ret) return ret;
    }


#if ((OUTPUT_TYPE == OUT_ARM_LINUX) || (OUTPUT_TYPE == OUT_LLASM))

    f = fopen(unaligned_ebp_areas, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", unaligned_ebp_areas);

        ret = SR_LoadSCI_unaligned_ebp_areas(f);

        fclose(f);

        if (ret) return ret;
    }


    f = fopen(unaligned_esp_areas, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", unaligned_esp_areas);

        ret = SR_LoadSCI_unaligned_esp_areas(f);

        fclose(f);

        if (ret) return ret;
    }


#endif

    f = fopen(instruction_flags, "rt");
    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", instruction_flags);

        ret = SR_LoadSCI_instruction_flags(f);

        fclose(f);

        if (ret) return ret;
    }
//#endif

    return 0;
}
