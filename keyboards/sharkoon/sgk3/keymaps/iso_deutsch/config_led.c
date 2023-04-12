/* Copyright 2023 Willerson Stockler <https://github.com/willersonsp>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifdef RGB_MATRIX_ENABLE

#include "rgb_matrix.h"
#include "config_led.h"

led_config_t g_led_config = { {
    {  0,      1,      2,      3,      4,      5,      6,      7,      8,      9,     10,     11,     12, NO_LED,     13,     14,     15, NO_LED, NO_LED, NO_LED, NO_LED },
    { 16,     17,     18,     19,     20,     21,     22,     23,     24,     25,     26,     27,     28,     29,     30,     31,     32,     33,     34,     35,     36 },
    { 37,     38,     39,     40,     41,     42,     43,     44,     45,     46,     47,     48,     49,     50,     51,     52,     53,     54,     55,     56,     57 },
    { 58,     59,     60,     61,     62,     63,     64,     65,     66,     67,     68,     69, NO_LED,     70, NO_LED, NO_LED, NO_LED,     71,     72,     73, NO_LED },
    { 74,     75,     76,     77,     78,     79,     80,     81,     82,     83,     84,     85, NO_LED,     86, NO_LED,     87, NO_LED,     88,     89,     90,     91 },
    { 92,     93,     94,     95,     96,     97,     98, NO_LED,     99, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED,    100,    101,    102, NO_LED,    103,    104, NO_LED }
}, {
    {0  ,  0}, {21 ,  0}, {31 ,  0}, {42 ,  0}, {52 ,  0}, {68 ,  0}, {78 ,  0}, {89 ,  0}, {99 ,  0}, {115,  0}, {125,  0}, {135,  0}, {146,  0},            {159,  0}, {169,  0}, {180,  0},
    {0  , 17}, {10 , 17}, {21 , 17}, {31 , 17}, {42 , 17}, {52 , 17}, {63 , 17}, {73 , 17}, {83 , 17}, {94 , 17}, {104, 17}, {115, 17}, {125, 17}, {141, 17}, {159, 17}, {169, 17}, {180, 17}, {193, 17}, {203, 17}, {214, 17}, {224, 17},
    {3  , 29}, {16 , 29}, {26 , 29}, {36 , 29}, {47 , 29}, {57 , 29}, {68 , 29}, {78 , 29}, {89 , 29}, {99 , 29}, {109, 29}, {120, 29}, {130, 29}, {133, 41}, {159, 29}, {169, 29}, {180, 29}, {193, 29}, {203, 29}, {214, 29}, {224, 35},
    {4  , 41}, {18 , 41}, {29 , 41}, {39 , 41}, {49 , 41}, {60 , 41}, {70 , 41}, {81 , 41}, {91 , 41}, {102, 41}, {112, 41}, {122, 41},            {145, 35},                                  {193, 41}, {203, 41}, {214, 41},
    {1  , 52}, {13 , 52}, {23 , 52}, {34 , 52}, {44 , 52}, {55 , 52}, {65 , 52}, {76 , 52}, {86 , 52}, {96 , 52}, {107, 52}, {117, 52},            {137, 52},            {169, 52},            {193, 52}, {203, 52}, {214, 52}, {224, 58},
    {1  , 64}, {14 , 64}, {27 , 64}, {66 , 64}, {105, 64}, {119, 64}, {132, 64},            {145, 64},                                                        {159, 64}, {169, 64}, {180, 64},            {198, 64}, {214, 64},
}, {
   1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    4, 4, 4,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 4, 4, 4, 4, 4, 4, 4,
   1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
   8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    1,          4, 4, 4,
   1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    1,    4,    4, 4, 4, 4,
   1, 1, 1, 4, 1, 1, 1,    1,                4, 4, 4,    4, 4,
} };

#endif
