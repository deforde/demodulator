#include "rds.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static const size_t RDS_BLOCK_LEN_BITS = 26;

typedef enum {
    A,
    B,
    C_1,
    C_2,
    D
} offset_word_label;

typedef struct {
    offset_word_label label;
    uint16_t word;
} offset_word_t;

typedef struct {
    uint32_t pty_code;
    const char* pty_string;
} pty_code_t;

offset_word_t offset_words[] = {
    { .label = A, .word = 0xFC },
    { .label = B, .word = 0x198 },
    { .label = C_1, .word = 0x168 },
    { .label = C_2, .word = 0x350 },
    { .label = D, .word = 0x1B4 }
};

pty_code_t pty_codes[] = {
    { .pty_code = 0, .pty_string = "None" },
    { .pty_code = 1, .pty_string = "News" },
    { .pty_code = 2, .pty_string = "Current Affairs" },
    { .pty_code = 3, .pty_string = "Information" },
    { .pty_code = 4, .pty_string = "Sport" },
    { .pty_code = 5, .pty_string = "Education" },
    { .pty_code = 6, .pty_string = "Drama" },
    { .pty_code = 7, .pty_string = "Culture" },
    { .pty_code = 8, .pty_string = "Science" },
    { .pty_code = 9, .pty_string = "Varied" },
    { .pty_code = 10, .pty_string = "Pop Music" },
    { .pty_code = 11, .pty_string = "Rock Music" },
    { .pty_code = 12, .pty_string = "Easy Music" },
    { .pty_code = 13, .pty_string = "Light Classical" },
    { .pty_code = 14, .pty_string = "Seriously Classical" },
    { .pty_code = 15, .pty_string = "Other Music" },
    { .pty_code = 16, .pty_string = "Weather" },
    { .pty_code = 17, .pty_string = "Finance" },
    { .pty_code = 18, .pty_string = "Children" },
    { .pty_code = 19, .pty_string = "Social Affairs" },
    { .pty_code = 20, .pty_string = "Religion" },
    { .pty_code = 21, .pty_string = "Phone In" },
    { .pty_code = 22, .pty_string = "Travel" },
    { .pty_code = 23, .pty_string = "Leisure" },
    { .pty_code = 24, .pty_string = "Jazz Music" },
    { .pty_code = 25, .pty_string = "Country Music" },
    { .pty_code = 26, .pty_string = "National Music" },
    { .pty_code = 27, .pty_string = "Oldies Music" },
    { .pty_code = 28, .pty_string = "Folk Music" },
    { .pty_code = 29, .pty_string = "Documentary" },
    { .pty_code = 30, .pty_string = "Alarm Test" },
    { .pty_code = 31, .pty_string = "Alarm" }
};

bool crc_check(uint32_t block, offset_word_label* label)
{
    static const uint32_t polynomial_len = 10;
    static const uint32_t polynomial = 0x2DD;

    uint32_t input = block;
    size_t current_shift = 0;
    while((input & 0xFFFFFC00) != 0) {
        while(((input << current_shift) & 0x80000000) != 0x80000000) {
            ++current_shift;
        }
        input ^= (polynomial << (sizeof(input) * 8 - current_shift - polynomial_len));
    }

    const uint32_t remainder = input & 0x3FF;

    for(size_t i = 0; i < sizeof(offset_words) / sizeof(*offset_words); ++i) {
        if(remainder == offset_words[i].word) {
            *label = offset_words[i].label;
            return true;
        }
    }

    return false;
}

void interpret_payload(uint16_t payload, offset_word_label label)
{
    switch(label) {
    case A: {
        uint32_t country_code = ((payload & 0xF000) >> 12);
        uint32_t program_type = ((payload & 0x0F00) >> 8);
        uint32_t program_reference_num = (payload & 0x00FF);

        printf(
            "RDS:\n"
            "\tOffset Word: A\n"
            "\t\tCountry Code: %u\n"
            "\t\tProgram Type: %u\n"
            "\t\tProgram Reference Number: %u\n",
            country_code,
            program_type,
            program_reference_num);

        break;
    }
    case B: {
        uint32_t group_type = ((payload & 0xF000) >> 12);
        char group_version = (((payload & 0x0800) >> 11) == 1) ? 'B' : 'A';
        uint32_t traffic_program = ((payload & 0x0400) >> 10);

        uint32_t pty = ((payload & 0x03E0) >> 5);
        pty_code_t* pty_code = NULL;

        for(size_t i = 0; i < sizeof(pty_codes) / sizeof(*pty_codes); ++i) {
            if(pty_codes[i].pty_code == pty) {
                pty_code = &pty_codes[i];
                break;
            }
        }

        printf(
            "RDS:\n"
            "\tOffset Word: B\n"
            "\t\tPTY: %u (%s)\n"
            "\t\tGroup Type/Version: %u%c\n"
            "\t\tTraffic Program: %u\n",
            pty,
            (pty_code != NULL ? pty_code->pty_string : "Unknown"),
            group_type,
            group_version,
            traffic_program);

        break;
    }
    case C_1: {
        printf(
            "RDS:\n"
            "\tOffset Word: C\n");
        break;
    }
    case C_2: {
        printf(
            "RDS:\n"
            "\tOffset Word: C'\n");
        break;
    }
    case D: {
        printf(
            "RDS:\n"
            "\tOffset Word: D\n");
        break;
    }
    }
}

void init_rds_interpreter(rds_interpreter_t* interpreter)
{
    interpreter->block = 0;
    interpreter->syncd = false;
    interpreter->num_bits_since_sync = 0;
}

void interpret_rds_data(rds_interpreter_t* interpreter, uint32_t bit)
{
    interpreter->block <<= 1;
    interpreter->block &= 0x03FFFFFE;
    interpreter->block |= (0x1 & bit);

    ++interpreter->num_bits_since_sync;

    if(interpreter->syncd) {
        if(interpreter->num_bits_since_sync < RDS_BLOCK_LEN_BITS) {
            return;
        }
    }

    offset_word_label label;
    interpreter->syncd = crc_check(interpreter->block, &label);
    if(interpreter->syncd) {
        interpreter->num_bits_since_sync = 0;
        const uint16_t payload = ((interpreter->block >> 10) & 0xFFFF);
        interpret_payload(payload, label);
    }
}

void destroy_rds_interpreter(rds_interpreter_t* interpreter)
{
    interpreter->block = 0;
    interpreter->syncd = false;
    interpreter->num_bits_since_sync = 0;
}
