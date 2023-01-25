#include <tty/font.h>

const uint16_t DEFAULT_FONT_DATA[] = {
    0,     0,     0,     0,     0,     0,     0,     0,     0,     // ' '  32
    0,     0,     62,    13311, 13311, 62,    0,     0,     0,     // '!'  33
    0,     7,     7,     0,     0,     7,     7,     0,     0,     // '"'  34
    288,   8190,  8190,  288,   288,   8190,  8190,  288,   0,     // '#'  35
    1592,  3644,  2148,  16383, 16383, 2436,  3868,  1816,  0,     // '$'  36
    12318, 7219,  1822,  448,   112,   7708,  13063, 7681,  0,     // '%'  37
    7966,  12723, 8417,  8417,  12723, 7966,  6912,  12672, 0,     // '&'  38
    0,     0,     0,     11,    7,     0,     0,     0,     0,     // '''  39
    0,     0,     2032,  16382, 30735, 16385, 0,     0,     0,     // '('  40
    0,     0,     16385, 30735, 16382, 2032,  0,     0,     0,     // ')'  41
    128,   672,   992,   448,   448,   992,   672,   128,   0,     // '*'  42
    128,   128,   128,   992,   992,   128,   128,   128,   0,     // '+'  43
    0,     0,     0,     45056, 28672, 0,     0,     0,     0,     // ','  44
    128,   128,   128,   128,   128,   128,   128,   128,   0,     // '-'  45
    0,     0,     0,     12288, 12288, 0,     0,     0,     0,     // '.'  46
    12288, 15360, 3840,  960,   240,   60,    15,    3,     0,     // '/'  47
    4092,  8190,  13827, 8577,  8289,  12315, 8190,  4092,  0,     // '0'  48
    8196,  8196,  8198,  16383, 16383, 8192,  8192,  8192,  0,     // '1'  49
    14348, 15374, 9731,  8961,  8577,  8387,  8318,  8252,  0,     // '2'  50
    3084,  7182,  12355, 8257,  8257,  12355, 8190,  4028,  0,     // '3'  51
    3840,  4064,  2300,  2079,  16259, 16256, 2048,  2048,  0,     // '4'  52
    3135,  7231,  12321, 8225,  8225,  12385, 8129,  3969,  0,     // '5'  53
    4064,  8184,  12380, 8262,  8259,  12481, 8065,  3841,  0,     // '6'  54
    1,     1,     1,     15361, 16257, 1009,  127,   15,    0,     // '7'  55
    3868,  8126,  12515, 8257,  8257,  12515, 8126,  3868,  0,     // '8'  56
    8252,  8318,  8387,  12417, 6273,  3715,  2046,  508,   0,     // '9'  57
    0,     0,     0,     12384, 12384, 0,     0,     0,     0,     // ':'  58
    0,     0,     0,     45152, 28768, 0,     0,     0,     0,     // ';'  59
    128,   448,   864,   1584,  3096,  6156,  4100,  0,     0,     // '<'  60
    288,   288,   288,   288,   288,   288,   288,   288,   0,     // '='  61
    4100,  6156,  3096,  1584,  864,   448,   128,   0,     0,     // '>'  62
    6,     7,     3,     13057, 13249, 243,   63,    30,    0,     // '?'  63
    4092,  8190,  12291, 8673,  9201,  8721,  8467,  13310, 5116,  // '@'  64
    16380, 16382, 259,   257,   257,   259,   16382, 16380, 0,     // 'A'  65
    16383, 16383, 8257,  8257,  8257,  12515, 8126,  3868,  0,     // 'B'  66
    4092,  8190,  12291, 8193,  8193,  12291, 7182,  3084,  0,     // 'C'  67
    16383, 16383, 8193,  8193,  8193,  14343, 8190,  2040,  0,     // 'D'  68
    16383, 16383, 8257,  8257,  8257,  8257,  8257,  8193,  0,     // 'E'  69
    16383, 16383, 129,   129,   129,   129,   129,   1,     0,     // 'F'  70
    4092,  8190,  12291, 8193,  8321,  12419, 8078,  3980,  0,     // 'G'  71
    16383, 16383, 64,    64,    64,    64,    16383, 16383, 0,     // 'H'  72
    8193,  8193,  8193,  16383, 16383, 8193,  8193,  8193,  0,     // 'I'  73
    3072,  7168,  12288, 8192,  8192,  12288, 8191,  4095,  0,     // 'J'  74
    16383, 16383, 192,   480,   1848,  3612,  15375, 12291, 0,     // 'K'  75
    16383, 16383, 8192,  8192,  8192,  8192,  8192,  8192,  0,     // 'L'  76
    16383, 16383, 120,   960,   960,   120,   16383, 16383, 0,     // 'M'  77
    16383, 16383, 28,    112,   448,   1792,  16383, 16383, 0,     // 'N'  78
    4092,  8190,  12291, 8193,  8193,  12291, 8190,  4092,  0,     // 'O'  79
    16383, 16383, 129,   129,   129,   195,   126,   60,    0,     // 'P'  80
    4092,  8190,  12291, 8193,  24577, 61443, 40958, 4092,  0,     // 'Q'  81
    16383, 16383, 129,   129,   385,   8131,  15998, 8252,  0,     // 'R'  82
    3132,  7294,  12387, 8417,  8641,  12675, 8078,  3852,  0,     // 'S'  83
    1,     1,     1,     16383, 16383, 1,     1,     1,     0,     // 'T'  84
    4095,  8191,  12288, 8192,  8192,  12288, 8191,  4095,  0,     // 'U'  85
    63,    511,   4032,  15360, 15360, 4032,  511,   63,    0,     // 'V'  86
    511,   16383, 15872, 448,   448,   15872, 16383, 511,   0,     // 'W'  87
    15375, 16191, 1008,  192,   192,   1008,  16191, 15375, 0,     // 'X'  88
    63,    255,   448,   16256, 16256, 448,   255,   63,    0,     // 'Y'  89
    14337, 15873, 10113, 8641,  8289,  8249,  8223,  8199,  0,     // 'Z'  90
    0,     0,     16383, 16383, 8193,  8193,  0,     0,     0,     // '['  91
    3,     15,    60,    240,   960,   3840,  15360, 12288, 0,     // '\'  92
    0,     0,     8193,  8193,  16383, 16383, 0,     0,     0,     // ']'  93
    4,     4,     6,     3,     3,     6,     6,     4,     0,     // '^'  94
    32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, // '_'  95
    0,     0,     0,     7,     11,    0,     0,     0,     0,     // '`'  96
    7680,  16192, 8480,  8480,  8480,  4384,  16352, 16320, 0,     // 'a'  97
    16383, 16383, 4160,  8224,  8224,  12384, 8128,  3968,  0,     // 'b'  98
    3968,  8128,  12384, 8224,  8224,  8224,  12384, 4160,  0,     // 'c'  99
    3968,  8128,  12384, 8224,  8224,  4160,  16383, 16383, 0,     // 'd' 100
    3968,  8128,  12896, 8736,  8736,  8800,  13248, 4992,  0,     // 'e' 101
    32,    32,    16382, 16383, 33,    33,    35,    2,     0,     // 'f' 102
    18304, 20416, 55392, 36896, 36896, 51264, 32736, 16352, 0,     // 'g' 103
    16383, 16383, 64,    32,    32,    96,    16320, 16256, 0,     // 'h' 104
    0,     8224,  8224,  16355, 16355, 8192,  8192,  0,     0,     // 'i' 105
    0,     16384, 49184, 32800, 65507, 32739, 0,     0,     0,     // 'j' 106
    16383, 16383, 1536,  1792,  3456,  6336,  12384, 8224,  0,     // 'k' 107
    0,     8193,  8193,  16383, 16383, 8192,  8192,  0,     0,     // 'l' 108
    16352, 16352, 96,    8128,  8128,  96,    16352, 16320, 0,     // 'm' 109
    16352, 16352, 64,    32,    32,    96,    16320, 16256, 0,     // 'n' 110
    3968,  8128,  12384, 8224,  8224,  12384, 8128,  3968,  0,     // 'o' 111
    65504, 65504, 4160,  8224,  8224,  12384, 8128,  3968,  0,     // 'p' 112
    3968,  8128,  8224,  8224,  8224,  4160,  65504, 65504, 0,     // 'q' 113
    16352, 16352, 192,   64,    96,    96,    96,    64,    0,     // 'r' 114
    4288,  12768, 8480,  8992,  9760,  9248,  15456, 6208,  0,     // 's' 115
    0,     32,    32,    8190,  16382, 8224,  8224,  0,     0,     // 't' 116
    8160,  16352, 8192,  8192,  8192,  4096,  16352, 16352, 0,     // 'u' 117
    992,   2016,  7168,  12288, 12288, 7168,  2016,  992,   0,     // 'v' 118
    2016,  16352, 14336, 1984,  1984,  14336, 16352, 2016,  0,     // 'w' 119
    12384, 15840, 3456,  1792,  1792,  3456,  15840, 12384, 0,     // 'x' 120
    33760, 36832, 56320, 28672, 12288, 7168,  4064,  992,   0,     // 'y' 121
    12320, 14368, 11296, 9760,  8992,  8608,  8416,  8288,  0,     // 'z' 122
    0,     0,     128,   16382, 32639, 16385, 16385, 0,     0,     // '{' 123
    0,     0,     0,     65407, 65407, 0,     0,     0,     0,     // '|' 124
    0,     16385, 16385, 32639, 16382, 128,   0,     0,     0,     // '}' 125
    2,     2,     1,     1,     2,     2,     1,     1,     0      // '~' 126
};
