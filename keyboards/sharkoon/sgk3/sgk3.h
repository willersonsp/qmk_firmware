#pragma once

#include "quantum.h"

#define XXX KC_NO

#define LAYOUT_all( \
    k00,      k01, k02, k03, k04,   k05, k06, k07, k08,    k09, k10, k11, k12,   k13,  k14,  k15,                      \
    k16, k17, k18, k19, k20, k21, k22, k23, k24, k25, k26, k27, k28,    k29,     k30,  k31,  k32,  k33, k34, k35, k36, \
     k37,  k38, k39, k40, k41, k42, k43, k44, k45, k46, k47, k48, k49,   k50,    k51,  k52,  k53,  k54, k55, k56, k57, \
      k58,   k59, k60, k61, k62, k63, k64, k65, k66, k67, k68, k69,      k70,                      k71, k72, k73,      \
    k74,  k75, k76, k77, k78, k79, k80, k81, k82, k83, k84, k85,       k86,            k87,        k88, k89, k90, k91, \
    k92,  k93,  k94,                k95,                k96,  k97,  k98,  k99,  k100, k101, k102,    k103,  k104       \
) { \
    { k00, k01, k02, k03, k04, k05, k06, k07,  k08, k09, k10, k11, k12, XXX,  k13,  k14,  k15,  XXX,   XXX,  XXX, XXX }, \
    { k16, k17, k18, k19, k20, k21, k22, k23,  k24, k25, k26, k27, k28, k29,  k30,  k31,  k32,  k33,   k34,  k35, k36 }, \
    { k37, k38, k39, k40, k41, k42, k43, k44,  k45, k46, k47, k48, k49, k50,  k51,  k52,  k53,  k54,   k55,  k56, k57 }, \
    { k58, k59, k60, k61, k62, k63, k64, k65,  k66, k67, k68, k69, k70, k71,  XXX,  XXX,  XXX,  k72,   k73,  k74, XXX }, \
    { k75, k76, k77, k78, k79, k80, k81, k82,  k83, k84, k85, k86, XXX, k87,  XXX,  k88,  XXX,  k89,   k90,  k91, k92 }, \
    { k93, k94, k95, k96, k97, k98, k99, XXX, k100, XXX, XXX, XXX, XXX, XXX, k101, k102, k103,  XXX,  k104, k105, XXX }  \
}

#define LAYOUT_ansi( \
    k00,      k01, k02, k03, k04,   k05, k06, k07, k08,    k09, k10, k11, k12,  k13,  k14,  k15,                      \
    k16, k17, k18, k19, k20, k21, k22, k23, k24, k25, k26, k27, k28,    k29,    k30,  k31,  k32,  k33, k34, k35, k36, \
     k37,  k38, k39, k40, k41, k42, k43, k44, k45, k46, k47, k48, k49,   k50,   k51,  k52,  k53,  k54, k55, k56, k57, \
      k58,   k59, k60, k61, k62, k63, k64, k65, k66, k67, k68, k69,    k70,                       k71, k72, k73,      \
       k74,    k75, k76, k77, k78, k79, k80, k81, k82, k83, k84,      k85,            k86,        k87, k88, k89, k90, \
    k91,  k92,  k93,                k94,                k95,  k96,  k97,  k98,  k99, k100, k101,    k102,  k103       \
) { \
    { k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k10, k11, k12, XXX, k13,  k14,  k15,  XXX,   XXX,  XXX, XXX }, \
    { k16, k17, k18, k19, k20, k21, k22, k23, k24, k25, k26, k27, k28, k29, k30,  k31,  k32,  k33,   k34,  k35, k36 }, \
    { k37, k38, k39, k40, k41, k42, k43, k44, k45, k46, k47, k48, k49, k50, k51,  k52,  k53,  k54,   k55,  k56, k57 }, \
    { k58, k59, k60, k61, k62, k63, k64, k65, k66, k67, k68, k69, XXX, k70, XXX,  XXX,  XXX,  k71,   k72,  k73, XXX }, \
    { k74, XXX, k75, k76, k77, k78, k79, k80, k81, k82, k83, k84, XXX, k85, XXX,  k86,  XXX,  k87,   k88,  k89, k90 }, \
    { k91, k92, k93, k94, k95, k96, k97, XXX, k98, XXX, XXX, XXX, XXX, XXX, k99, k100, k101,  XXX,  k102, k103, XXX }  \
}

#define LAYOUT_iso( \
    k00,      k01, k02, k03, k04,   k05, k06, k07, k08,    k09, k10, k11, k12,   k13,  k14,  k15,                      \
    k16, k17, k18, k19, k20, k21, k22, k23, k24, k25, k26, k27, k28,    k29,     k30,  k31,  k32,  k33, k34, k35, k36, \
     k37,  k38, k39, k40, k41, k42, k43, k44, k45, k46, k47, k48, k49,           k50,  k51,  k52,  k53, k54, k55, k56, \
      k57,   k58, k59, k60, k61, k62, k63, k64, k65, k66, k67, k68, k69, k70,                      k71, k72, k73,      \
    k74,  k75, k76, k77, k78, k79, k80, k81, k82, k83, k84, k85,       k86,            k87,        k88, k89, k90, k91, \
    k92,  k93,  k94,                k95,                k96,  k97,  k98,  k99,  k100, k101, k102,    k103,  k104       \
) { \
    { k00, k01, k02, k03, k04, k05, k06, k07, k08, k09, k10, k11, k12, XXX,  k13,  k14,  k15,  XXX,   XXX,  XXX, XXX }, \
    { k16, k17, k18, k19, k20, k21, k22, k23, k24, k25, k26, k27, k28, k29,  k30,  k31,  k32,  k33,   k34,  k35, k36 }, \
    { k37, k38, k39, k40, k41, k42, k43, k44, k45, k46, k47, k48, k49, k50,  k51,  k52,  k53,  k54,   k55,  k56, k57 }, \
    { k58, k59, k60, k61, k62, k63, k64, k65, k66, k67, k68, k69, XXX, k70,  XXX,  XXX,  XXX,  k71,   k72,  k73, XXX }, \
    { k74, k75, k76, k77, k78, k79, k80, k81, k82, k83, k84, k85, XXX, k86,  XXX,  k87,  XXX,  k88,   k89,  k90, k91 }, \
    { k92, k93, k94, k95, k96, k97, k98, XXX, k99, XXX, XXX, XXX, XXX, XXX, k100, k101, k102,  XXX,  k103, k104, XXX }  \
}

#define LAYOUT_abnt2( \
    K00,      K01, K02, K03, K04,   K05, K06, K07, K08,    K09, K0A, K0B, K0C,  K0E, K0F, K0G,                      \
    K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C,    K1D,    K1E, K1F, K1G,  K1H, K1I, K1J, K1K, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C,            K2E, K2F, K2G,  K2H, K2I, K2J, K2K, \
    K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K2D,  K3D,                      K3H, K3I, K3J, K3K, \
    K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, K4C,  K4D,           K4F,       K4H, K4I, K4J, K4K, \
    K50, K51, K52,                K53,                K54, K55, K56,  K58,      K5E, K5F, K5G,  K5I,   K5J       \
) { \
    { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, XXX, K0E, K0F, K0G, XXX, XXX, XXX, XXX }, \
    { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, K1E, K1F, K1G, K1H, K1I, K1J, K1K }, \
    { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2C, K2D, K2E, K2F, K2G, K2H, K2I, K2J, K2K }, \
    { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, XXX, K3D, XXX, XXX, XXX, K3H, K3I, K3J, K3K }, \
    { K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K4A, K4B, K4C, K4D, XXX, K4F, XXX, K4H, K4I, K4J, K4K }, \
    { K50, K51, K52, K53, K54, K55, K56, XXX, K58, XXX, XXX, XXX, XXX, XXX, K5E, K5F, K5G, XXX, K5I, K5J, XXX }  \
}
