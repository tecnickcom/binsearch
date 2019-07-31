"""Tests for binsearch module - column mode."""


import binsearch as bs
import os
from unittest import TestCase

nrows = 251

testDataCol8 = [
    (0, 251, 0x00, 0, 0, 0, 1, 2, 2),
    (1, 251, 0x00, 1, 1, 1, 1, 2, 2),
    (0, 251, 0x01, 2, 2, 2, 2, 3, 3),
    (0, 251, 0x0F, 16, 16, 16, 16, 17, 17),
    (0, 251, 0x10, 17, 17, 17, 17, 18, 18),
    (0, 251, 0x1F, 32, 32, 32, 32, 33, 33),
    (0, 251, 0x20, 33, 33, 33, 33, 34, 34),
    (0, 251, 0x2F, 48, 48, 48, 48, 49, 49),
    (0, 251, 0x30, 49, 49, 49, 49, 50, 50),
    (0, 251, 0x3F, 64, 64, 64, 64, 65, 65),
    (0, 251, 0x40, 65, 65, 65, 65, 66, 66),
    (0, 251, 0x4F, 80, 80, 80, 80, 81, 81),
    (0, 251, 0x50, 81, 81, 81, 81, 82, 82),
    (0, 251, 0x5F, 96, 96, 96, 96, 97, 97),
    (0, 251, 0x60, 97, 97, 97, 97, 98, 98),
    (0, 251, 0x6F, 112, 112, 112, 112, 113, 113),
    (0, 251, 0x70, 113, 113, 113, 113, 114, 114),
    (0, 251, 0x7F, 128, 128, 128, 128, 129, 129),
    (0, 251, 0x80, 129, 129, 129, 129, 130, 130),
    (0, 251, 0x8F, 144, 144, 144, 144, 145, 145),
    (0, 251, 0x90, 145, 145, 145, 145, 146, 146),
    (0, 251, 0x9F, 160, 160, 160, 160, 161, 161),
    (0, 251, 0xA0, 161, 161, 161, 161, 162, 162),
    (0, 251, 0xAF, 176, 176, 176, 176, 177, 177),
    (0, 251, 0xB0, 177, 177, 177, 177, 178, 178),
    (0, 251, 0xBF, 192, 192, 192, 192, 193, 193),
    (0, 251, 0xC0, 193, 193, 193, 193, 194, 194),
    (0, 251, 0xCF, 208, 208, 208, 208, 209, 209),
    (0, 251, 0xD0, 209, 209, 209, 209, 210, 210),
    (0, 251, 0xDF, 224, 224, 224, 224, 225, 225),
    (0, 251, 0xE0, 225, 225, 225, 225, 226, 226),
    (0, 251, 0xEF, 240, 240, 240, 240, 241, 241),
    (0, 251, 0xF0, 241, 241, 241, 241, 242, 242),
    (0, 251, 0xF8, 249, 249, 249, 249, 250, 250),
    (0, 251, 0xFF, 250, 250, 250, 250, 251, 251),
    (0, 251, 0xF9, 251, 249, 250, 251, 249, 250),
    (0, 51, 0x70, 51, 50, 51, 51, 50, 51),
    (150, 251, 0x70, 251, 149, 150, 251, 149, 150),
]

testDataColSub8 = [
    (0, 251, 0x00, 0, 0, 0, 1, 2, 2),
    (1, 251, 0x00, 1, 1, 1, 1, 2, 2),
    (0, 251, 0x01, 2, 2, 2, 2, 3, 3),
    (0, 251, 0x0F, 16, 16, 16, 16, 17, 17),
    (0, 251, 0x10, 17, 17, 17, 17, 18, 18),
    (0, 251, 0x1F, 32, 32, 32, 32, 33, 33),
    (0, 251, 0x20, 33, 33, 33, 33, 34, 34),
    (0, 251, 0x2F, 48, 48, 48, 48, 49, 49),
    (0, 251, 0x30, 49, 49, 49, 49, 50, 50),
    (0, 251, 0x3F, 64, 64, 64, 64, 65, 65),
    (0, 251, 0x40, 65, 65, 65, 65, 66, 66),
    (0, 251, 0x4F, 80, 80, 80, 80, 81, 81),
    (0, 251, 0x50, 81, 81, 81, 81, 82, 82),
    (0, 251, 0x5F, 96, 96, 96, 96, 97, 97),
    (0, 251, 0x60, 97, 97, 97, 97, 98, 98),
    (0, 251, 0x6F, 112, 112, 112, 112, 113, 113),
    (0, 251, 0x70, 113, 113, 113, 113, 114, 114),
    (0, 251, 0x7F, 128, 128, 128, 128, 129, 129),
    (0, 251, 0x80, 129, 129, 129, 129, 130, 130),
    (0, 251, 0x8F, 144, 144, 144, 144, 145, 145),
    (0, 251, 0x90, 145, 145, 145, 145, 146, 146),
    (0, 251, 0x9F, 160, 160, 160, 160, 161, 161),
    (0, 251, 0xA0, 161, 161, 161, 161, 162, 162),
    (0, 251, 0xAF, 176, 176, 176, 176, 177, 177),
    (0, 251, 0xB0, 177, 177, 177, 177, 178, 178),
    (0, 251, 0xBF, 192, 192, 192, 192, 193, 193),
    (0, 251, 0xC0, 193, 193, 193, 193, 194, 194),
    (0, 251, 0xCF, 208, 208, 208, 208, 209, 209),
    (0, 251, 0xD0, 209, 209, 209, 209, 210, 210),
    (0, 251, 0xDF, 224, 224, 224, 224, 225, 225),
    (0, 251, 0xE0, 225, 225, 225, 225, 226, 226),
    (0, 251, 0xEF, 240, 240, 240, 240, 241, 241),
    (0, 251, 0xF0, 241, 241, 241, 241, 242, 242),
    (0, 251, 0xF8, 249, 249, 249, 249, 250, 250),
    (0, 251, 0xFF, 250, 250, 250, 250, 251, 251),
    (0, 251, 0xF9, 251, 249, 250, 251, 249, 250),
    (0, 51, 0x70, 51, 50, 51, 51, 50, 51),
    (150, 251, 0x70, 251, 149, 150, 251, 149, 150),
]

testDataCol16 = [
    (0, 251, 0x0000, 0, 0, 0, 0, 1, 1),
    (1, 251, 0x0001, 1, 1, 1, 1, 2, 2),
    (0, 251, 0x0102, 2, 2, 2, 2, 3, 3),
    (0, 251, 0x0F10, 16, 16, 16, 16, 17, 17),
    (0, 251, 0x1011, 17, 17, 17, 17, 18, 18),
    (0, 251, 0x1F20, 32, 32, 32, 32, 33, 33),
    (0, 251, 0x2021, 33, 33, 33, 33, 34, 34),
    (0, 251, 0x2F30, 48, 48, 48, 48, 49, 49),
    (0, 251, 0x3031, 49, 49, 49, 49, 50, 50),
    (0, 251, 0x3F40, 64, 64, 64, 64, 65, 65),
    (0, 251, 0x4041, 65, 65, 65, 65, 66, 66),
    (0, 251, 0x4F50, 80, 80, 80, 80, 81, 81),
    (0, 251, 0x5051, 81, 81, 81, 81, 82, 82),
    (0, 251, 0x5F60, 96, 96, 96, 96, 97, 97),
    (0, 251, 0x6061, 97, 97, 97, 97, 98, 98),
    (0, 251, 0x6F70, 112, 112, 112, 112, 113, 113),
    (0, 251, 0x7071, 113, 113, 113, 113, 114, 114),
    (0, 251, 0x7F80, 128, 128, 128, 128, 129, 129),
    (0, 251, 0x8081, 129, 129, 129, 129, 130, 130),
    (0, 251, 0x8F90, 144, 144, 144, 144, 145, 145),
    (0, 251, 0x9091, 145, 145, 145, 145, 146, 146),
    (0, 251, 0x9FA0, 160, 160, 160, 160, 161, 161),
    (0, 251, 0xA0A1, 161, 161, 161, 161, 162, 162),
    (0, 251, 0xAFB0, 176, 176, 176, 176, 177, 177),
    (0, 251, 0xB0B1, 177, 177, 177, 177, 178, 178),
    (0, 251, 0xBFC0, 192, 192, 192, 192, 193, 193),
    (0, 251, 0xC0C1, 193, 193, 193, 193, 194, 194),
    (0, 251, 0xCFD0, 208, 208, 208, 208, 209, 209),
    (0, 251, 0xD0D1, 209, 209, 209, 209, 210, 210),
    (0, 251, 0xDFE0, 224, 224, 224, 224, 225, 225),
    (0, 251, 0xE0E1, 225, 225, 225, 225, 226, 226),
    (0, 251, 0xEFF0, 240, 240, 240, 240, 241, 241),
    (0, 251, 0xF0F1, 241, 241, 241, 241, 242, 242),
    (0, 251, 0xF8F9, 249, 249, 249, 249, 250, 250),
    (0, 251, 0xFFFF, 250, 250, 250, 250, 251, 251),
    (0, 251, 0xF9F9, 251, 249, 250, 251, 249, 250),
    (0, 51, 0x7071, 51, 50, 51, 51, 50, 51),
    (150, 251, 0x7071, 251, 149, 150, 251, 149, 150),
]

testDataColSub16 = [
    (0, 251, 0x0000, 0, 0, 0, 0, 1, 1),
    (1, 251, 0x0001, 1, 1, 1, 1, 2, 2),
    (0, 251, 0x0102, 2, 2, 2, 2, 3, 3),
    (0, 251, 0x0F10, 16, 16, 16, 16, 17, 17),
    (0, 251, 0x1011, 17, 17, 17, 17, 18, 18),
    (0, 251, 0x1F20, 32, 32, 32, 32, 33, 33),
    (0, 251, 0x2021, 33, 33, 33, 33, 34, 34),
    (0, 251, 0x2F30, 48, 48, 48, 48, 49, 49),
    (0, 251, 0x3031, 49, 49, 49, 49, 50, 50),
    (0, 251, 0x3F40, 64, 64, 64, 64, 65, 65),
    (0, 251, 0x4041, 65, 65, 65, 65, 66, 66),
    (0, 251, 0x4F50, 80, 80, 80, 80, 81, 81),
    (0, 251, 0x5051, 81, 81, 81, 81, 82, 82),
    (0, 251, 0x5F60, 96, 96, 96, 96, 97, 97),
    (0, 251, 0x6061, 97, 97, 97, 97, 98, 98),
    (0, 251, 0x6F70, 112, 112, 112, 112, 113, 113),
    (0, 251, 0x7071, 113, 113, 113, 113, 114, 114),
    (0, 251, 0x7F80, 128, 128, 128, 128, 129, 129),
    (0, 251, 0x8081, 129, 129, 129, 129, 130, 130),
    (0, 251, 0x8F90, 144, 144, 144, 144, 145, 145),
    (0, 251, 0x9091, 145, 145, 145, 145, 146, 146),
    (0, 251, 0x9FA0, 160, 160, 160, 160, 161, 161),
    (0, 251, 0xA0A1, 161, 161, 161, 161, 162, 162),
    (0, 251, 0xAFB0, 176, 176, 176, 176, 177, 177),
    (0, 251, 0xB0B1, 177, 177, 177, 177, 178, 178),
    (0, 251, 0xBFC0, 192, 192, 192, 192, 193, 193),
    (0, 251, 0xC0C1, 193, 193, 193, 193, 194, 194),
    (0, 251, 0xCFD0, 208, 208, 208, 208, 209, 209),
    (0, 251, 0xD0D1, 209, 209, 209, 209, 210, 210),
    (0, 251, 0xDFE0, 224, 224, 224, 224, 225, 225),
    (0, 251, 0xE0E1, 225, 225, 225, 225, 226, 226),
    (0, 251, 0xEFF0, 240, 240, 240, 240, 241, 241),
    (0, 251, 0xF0F1, 241, 241, 241, 241, 242, 242),
    (0, 251, 0xF8F9, 249, 249, 249, 249, 250, 250),
    (0, 251, 0xFFFF, 250, 250, 250, 250, 251, 251),
    (0, 251, 0xF9F9, 251, 249, 250, 251, 249, 250),
    (0, 51, 0x7071, 51, 50, 51, 51, 50, 51),
    (150, 251, 0x7071, 251, 149, 150, 251, 149, 150),
]

testDataCol32 = [
    (0, 251, 0x00000000, 0, 0, 0, 0, 1, 1),
    (1, 251, 0x00010203, 1, 1, 1, 1, 2, 2),
    (0, 251, 0x01020304, 2, 2, 2, 2, 3, 3),
    (0, 251, 0x0F101112, 16, 16, 16, 16, 17, 17),
    (0, 251, 0x10111213, 17, 17, 17, 17, 18, 18),
    (0, 251, 0x1F202122, 32, 32, 32, 32, 33, 33),
    (0, 251, 0x20212223, 33, 33, 33, 33, 34, 34),
    (0, 251, 0x2F303132, 48, 48, 48, 48, 49, 49),
    (0, 251, 0x30313233, 49, 49, 49, 49, 50, 50),
    (0, 251, 0x3F404142, 64, 64, 64, 64, 65, 65),
    (0, 251, 0x40414243, 65, 65, 65, 65, 66, 66),
    (0, 251, 0x4F505152, 80, 80, 80, 80, 81, 81),
    (0, 251, 0x50515253, 81, 81, 81, 81, 82, 82),
    (0, 251, 0x5F606162, 96, 96, 96, 96, 97, 97),
    (0, 251, 0x60616263, 97, 97, 97, 97, 98, 98),
    (0, 251, 0x6F707172, 112, 112, 112, 112, 113, 113),
    (0, 251, 0x70717273, 113, 113, 113, 113, 114, 114),
    (0, 251, 0x7F808182, 128, 128, 128, 128, 129, 129),
    (0, 251, 0x80818283, 129, 129, 129, 129, 130, 130),
    (0, 251, 0x8F909192, 144, 144, 144, 144, 145, 145),
    (0, 251, 0x90919293, 145, 145, 145, 145, 146, 146),
    (0, 251, 0x9FA0A1A2, 160, 160, 160, 160, 161, 161),
    (0, 251, 0xA0A1A2A3, 161, 161, 161, 161, 162, 162),
    (0, 251, 0xAFB0B1B2, 176, 176, 176, 176, 177, 177),
    (0, 251, 0xB0B1B2B3, 177, 177, 177, 177, 178, 178),
    (0, 251, 0xBFC0C1C2, 192, 192, 192, 192, 193, 193),
    (0, 251, 0xC0C1C2C3, 193, 193, 193, 193, 194, 194),
    (0, 251, 0xCFD0D1D2, 208, 208, 208, 208, 209, 209),
    (0, 251, 0xD0D1D2D3, 209, 209, 209, 209, 210, 210),
    (0, 251, 0xDFE0E1E2, 224, 224, 224, 224, 225, 225),
    (0, 251, 0xE0E1E2E3, 225, 225, 225, 225, 226, 226),
    (0, 251, 0xEFF0F1F2, 240, 240, 240, 240, 241, 241),
    (0, 251, 0xF0F1F2F3, 241, 241, 241, 241, 242, 242),
    (0, 251, 0xF8F9FAFB, 249, 249, 249, 249, 250, 250),
    (0, 251, 0xFFFFFFFF, 250, 250, 250, 250, 251, 251),
    (0, 251, 0xF9F9FAFB, 251, 249, 250, 251, 249, 250),
    (0, 51, 0x70717273, 51, 50, 51, 51, 50, 51),
    (150, 251, 0x70717273, 251, 149, 150, 251, 149, 150),
]

testDataColSub32 = [
    (0, 251, 0x00000000, 0, 0, 0, 0, 1, 1),
    (1, 251, 0x00000102, 1, 1, 1, 1, 2, 2),
    (0, 251, 0x00000203, 2, 2, 2, 2, 3, 3),
    (0, 251, 0x00001011, 16, 16, 16, 16, 17, 17),
    (0, 251, 0x00001112, 17, 17, 17, 17, 18, 18),
    (0, 251, 0x00002021, 32, 32, 32, 32, 33, 33),
    (0, 251, 0x00002122, 33, 33, 33, 33, 34, 34),
    (0, 251, 0x00003031, 48, 48, 48, 48, 49, 49),
    (0, 251, 0x00003132, 49, 49, 49, 49, 50, 50),
    (0, 251, 0x00004041, 64, 64, 64, 64, 65, 65),
    (0, 251, 0x00004142, 65, 65, 65, 65, 66, 66),
    (0, 251, 0x00005051, 80, 80, 80, 80, 81, 81),
    (0, 251, 0x00005152, 81, 81, 81, 81, 82, 82),
    (0, 251, 0x00006061, 96, 96, 96, 96, 97, 97),
    (0, 251, 0x00006162, 97, 97, 97, 97, 98, 98),
    (0, 251, 0x00007071, 112, 112, 112, 112, 113, 113),
    (0, 251, 0x00007172, 113, 113, 113, 113, 114, 114),
    (0, 251, 0x00008081, 128, 128, 128, 128, 129, 129),
    (0, 251, 0x00008182, 129, 129, 129, 129, 130, 130),
    (0, 251, 0x00009091, 144, 144, 144, 144, 145, 145),
    (0, 251, 0x00009192, 145, 145, 145, 145, 146, 146),
    (0, 251, 0x0000A0A1, 160, 160, 160, 160, 161, 161),
    (0, 251, 0x0000A1A2, 161, 161, 161, 161, 162, 162),
    (0, 251, 0x0000B0B1, 176, 176, 176, 176, 177, 177),
    (0, 251, 0x0000B1B2, 177, 177, 177, 177, 178, 178),
    (0, 251, 0x0000C0C1, 192, 192, 192, 192, 193, 193),
    (0, 251, 0x0000C1C2, 193, 193, 193, 193, 194, 194),
    (0, 251, 0x0000D0D1, 208, 208, 208, 208, 209, 209),
    (0, 251, 0x0000D1D2, 209, 209, 209, 209, 210, 210),
    (0, 251, 0x0000E0E1, 224, 224, 224, 224, 225, 225),
    (0, 251, 0x0000E1E2, 225, 225, 225, 225, 226, 226),
    (0, 251, 0x0000F0F1, 240, 240, 240, 240, 241, 241),
    (0, 251, 0x0000F1F2, 241, 241, 241, 241, 242, 242),
    (0, 251, 0x0000F9FA, 249, 249, 249, 249, 250, 250),
    (0, 251, 0x0000FFFF, 250, 250, 250, 250, 251, 251),
    (0, 251, 0x0000F9FA, 249, 249, 249, 249, 250, 250),
    (0, 51, 0x00007172, 51, 50, 51, 51, 50, 51),
    (150, 251, 0x00007172, 251, 149, 150, 251, 149, 150),
]

testDataCol64 = [
    (0, 251, 0x0000000000000000, 0, 0, 0, 0, 1, 1),
    (1, 251, 0x0001020304050607, 1, 1, 1, 1, 2, 2),
    (0, 251, 0x0102030405060708, 2, 2, 2, 2, 3, 3),
    (0, 251, 0x0F10111213141516, 16, 16, 16, 16, 17, 17),
    (0, 251, 0x1011121314151617, 17, 17, 17, 17, 18, 18),
    (0, 251, 0x1F20212223242526, 32, 32, 32, 32, 33, 33),
    (0, 251, 0x2021222324252627, 33, 33, 33, 33, 34, 34),
    (0, 251, 0x2F30313233343536, 48, 48, 48, 48, 49, 49),
    (0, 251, 0x3031323334353637, 49, 49, 49, 49, 50, 50),
    (0, 251, 0x3F40414243444546, 64, 64, 64, 64, 65, 65),
    (0, 251, 0x4041424344454647, 65, 65, 65, 65, 66, 66),
    (0, 251, 0x4F50515253545556, 80, 80, 80, 80, 81, 81),
    (0, 251, 0x5051525354555657, 81, 81, 81, 81, 82, 82),
    (0, 251, 0x5F60616263646566, 96, 96, 96, 96, 97, 97),
    (0, 251, 0x6061626364656667, 97, 97, 97, 97, 98, 98),
    (0, 251, 0x6F70717273747576, 112, 112, 112, 112, 113, 113),
    (0, 251, 0x7071727374757677, 113, 113, 113, 113, 114, 114),
    (0, 251, 0x7F80818283848586, 128, 128, 128, 128, 129, 129),
    (0, 251, 0x8081828384858687, 129, 129, 129, 129, 130, 130),
    (0, 251, 0x8F90919293949596, 144, 144, 144, 144, 145, 145),
    (0, 251, 0x9091929394959697, 145, 145, 145, 145, 146, 146),
    (0, 251, 0x9FA0A1A2A3A4A5A6, 160, 160, 160, 160, 161, 161),
    (0, 251, 0xA0A1A2A3A4A5A6A7, 161, 161, 161, 161, 162, 162),
    (0, 251, 0xAFB0B1B2B3B4B5B6, 176, 176, 176, 176, 177, 177),
    (0, 251, 0xB0B1B2B3B4B5B6B7, 177, 177, 177, 177, 178, 178),
    (0, 251, 0xBFC0C1C2C3C4C5C6, 192, 192, 192, 192, 193, 193),
    (0, 251, 0xC0C1C2C3C4C5C6C7, 193, 193, 193, 193, 194, 194),
    (0, 251, 0xCFD0D1D2D3D4D5D6, 208, 208, 208, 208, 209, 209),
    (0, 251, 0xD0D1D2D3D4D5D6D7, 209, 209, 209, 209, 210, 210),
    (0, 251, 0xDFE0E1E2E3E4E5E6, 224, 224, 224, 224, 225, 225),
    (0, 251, 0xE0E1E2E3E4E5E6E7, 225, 225, 225, 225, 226, 226),
    (0, 251, 0xEFF0F1F2F3F4F5F6, 240, 240, 240, 240, 241, 241),
    (0, 251, 0xF0F1F2F3F4F5F6F7, 241, 241, 241, 241, 242, 242),
    (0, 251, 0xF8F9FAFBFCFDFEFF, 249, 249, 249, 249, 250, 250),
    (0, 251, 0xFFFFFFFFFFFFFFFF, 250, 250, 250, 250, 251, 251),
    (0, 251, 0xF9F9FAFBFCFDFEFF, 251, 249, 250, 251, 249, 250),
    (0, 51, 0x7071727374757677, 51, 50, 51, 51, 50, 51),
    (150, 251, 0x7071727374757677, 251, 149, 150, 251, 149, 150),
]

testDataColSub64 = [
    (0, 251, 0x0000000000000000, 0, 0, 0, 0, 1, 1),
    (1, 251, 0x0000000002030405, 1, 1, 1, 1, 2, 2),
    (0, 251, 0x0000000003040506, 2, 2, 2, 2, 3, 3),
    (0, 251, 0x0000000011121314, 16, 16, 16, 16, 17, 17),
    (0, 251, 0x0000000012131415, 17, 17, 17, 17, 18, 18),
    (0, 251, 0x0000000021222324, 32, 32, 32, 32, 33, 33),
    (0, 251, 0x0000000022232425, 33, 33, 33, 33, 34, 34),
    (0, 251, 0x0000000031323334, 48, 48, 48, 48, 49, 49),
    (0, 251, 0x0000000032333435, 49, 49, 49, 49, 50, 50),
    (0, 251, 0x0000000041424344, 64, 64, 64, 64, 65, 65),
    (0, 251, 0x0000000042434445, 65, 65, 65, 65, 66, 66),
    (0, 251, 0x0000000051525354, 80, 80, 80, 80, 81, 81),
    (0, 251, 0x0000000052535455, 81, 81, 81, 81, 82, 82),
    (0, 251, 0x0000000061626364, 96, 96, 96, 96, 97, 97),
    (0, 251, 0x0000000062636465, 97, 97, 97, 97, 98, 98),
    (0, 251, 0x0000000071727374, 112, 112, 112, 112, 113, 113),
    (0, 251, 0x0000000072737475, 113, 113, 113, 113, 114, 114),
    (0, 251, 0x0000000081828384, 128, 128, 128, 128, 129, 129),
    (0, 251, 0x0000000082838485, 129, 129, 129, 129, 130, 130),
    (0, 251, 0x0000000091929394, 144, 144, 144, 144, 145, 145),
    (0, 251, 0x0000000092939495, 145, 145, 145, 145, 146, 146),
    (0, 251, 0x00000000A1A2A3A4, 160, 160, 160, 160, 161, 161),
    (0, 251, 0x00000000A2A3A4A5, 161, 161, 161, 161, 162, 162),
    (0, 251, 0x00000000B1B2B3B4, 176, 176, 176, 176, 177, 177),
    (0, 251, 0x00000000B2B3B4B5, 177, 177, 177, 177, 178, 178),
    (0, 251, 0x00000000C1C2C3C4, 192, 192, 192, 192, 193, 193),
    (0, 251, 0x00000000C2C3C4C5, 193, 193, 193, 193, 194, 194),
    (0, 251, 0x00000000D1D2D3D4, 208, 208, 208, 208, 209, 209),
    (0, 251, 0x00000000D2D3D4D5, 209, 209, 209, 209, 210, 210),
    (0, 251, 0x00000000E1E2E3E4, 224, 224, 224, 224, 225, 225),
    (0, 251, 0x00000000E2E3E4E5, 225, 225, 225, 225, 226, 226),
    (0, 251, 0x00000000F1F2F3F4, 240, 240, 240, 240, 241, 241),
    (0, 251, 0x00000000F2F3F4F5, 241, 241, 241, 241, 242, 242),
    (0, 251, 0x00000000FAFBFCFD, 249, 249, 249, 249, 250, 250),
    (0, 251, 0x00000000FFFFFFFF, 250, 250, 250, 250, 251, 251),
    (0, 251, 0x00000000FAFBFCFD, 249, 249, 249, 249, 250, 250),
    (0, 51, 0x0000000072737475, 51, 50, 51, 51, 50, 51),
    (150, 251, 0x0000000072737475, 251, 149, 150, 251, 149, 150),
]


class TestFunctions(TestCase):
    @classmethod
    def setUpClass(cls):
        global src, fd, size, doffset, dlength, nrows, ncols, index
        inputfile = os.path.realpath(
            os.path.dirname(os.path.realpath(__file__))
            + "/../../c/test/data/test_data_col.bin"
        )
        src, fd, size, doffset, dlength, nrows, ncols, index, idx = bs.mmap_binfile(
            inputfile, [1, 2, 4, 8]
        )
        if fd < 0 or size != 3776:
            assert False, "Unable to open the file"

    @classmethod
    def tearDownClass(cls):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"

    def test_col_find_first_uint8(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol8:
            rp, rf, rl = bs.col_find_first_uint8(src, index[0], first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_uint8(src, index[0], pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_uint16(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol16:
            rp, rf, rl = bs.col_find_first_uint16(src, index[1], first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_uint16(src, index[1], pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_uint32(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol32:
            rp, rf, rl = bs.col_find_first_uint32(src, index[2], first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_uint32(src, index[2], pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_uint64(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol64:
            rp, rf, rl = bs.col_find_first_uint64(src, index[3], first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_uint64(src, index[3], pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_uint8(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol8:
            rp, rf, rl = bs.col_find_last_uint8(src, index[0], first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_uint8(src, index[0], first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_uint16(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol16:
            rp, rf, rl = bs.col_find_last_uint16(src, index[1], first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_uint16(src, index[1], first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_uint32(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol32:
            rp, rf, rl = bs.col_find_last_uint32(src, index[2], first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_uint32(src, index[2], first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_uint64(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol64:
            rp, rf, rl = bs.col_find_last_uint64(src, index[3], first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_uint64(src, index[3], first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_sub_uint8(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub8:
            rp, rf, rl = bs.col_find_first_sub_uint8(
                src, index[0], 0, 7, first, last, search
            )
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_sub_uint8(
                        src, index[0], 0, 7, pos, last, search
                    )
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_sub_uint16(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub16:
            rp, rf, rl = bs.col_find_first_sub_uint16(
                src, index[1], 0, 15, first, last, search
            )
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_sub_uint16(
                        src, index[1], 0, 15, pos, last, search
                    )
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_sub_uint32(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub32:
            rp, rf, rl = bs.col_find_first_sub_uint32(
                src, index[2], 8, 23, first, last, search
            )
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_sub_uint32(
                        src, index[2], 8, 23, pos, last, search
                    )
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_sub_uint64(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub64:
            rp, rf, rl = bs.col_find_first_sub_uint64(
                src, index[3], 16, 47, first, last, search
            )
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_sub_uint64(
                        src, index[3], 16, 47, pos, last, search
                    )
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_sub_uint8(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub8:
            rp, rf, rl = bs.col_find_last_sub_uint8(
                src, index[0], 0, 7, first, last, search
            )
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_sub_uint8(
                        src, index[0], 0, 7, first, pos, search
                    )
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_sub_uint16(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub16:
            rp, rf, rl = bs.col_find_last_sub_uint16(
                src, index[1], 0, 15, first, last, search
            )
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_sub_uint16(
                        src, index[1], 0, 15, first, pos, search
                    )
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_sub_uint32(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub32:
            rp, rf, rl = bs.col_find_last_sub_uint32(
                src, index[2], 8, 23, first, last, search
            )
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_sub_uint32(
                        src, index[2], 8, 23, first, pos, search
                    )
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_sub_uint64(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub64:
            rp, rf, rl = bs.col_find_last_sub_uint64(
                src, index[3], 16, 47, first, last, search
            )
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp < last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_sub_uint64(
                        src, index[3], 16, 47, first, pos, search
                    )
                    counter = counter + 1
                self.assertEqual(counter, numitems)


class TestBenchmark(object):

    global setup

    def setup():
        global src, fd, size, doffset, dlength, nrows, ncols, index
        if fd >= 0:
            pass
        bs.munmap_binfile(src, fd, size)
        inputfile = os.path.realpath(
            os.path.dirname(os.path.realpath(__file__))
            + "/../../c/test/data/test_data_col.bin"
        )
        src, fd, size, doffset, dlength, nrows, ncols, index, idx = bs.mmap_binfile(
            inputfile, [1, 2, 4, 8]
        )
        if fd < 0 or size != 3776:
            assert False, "Unable to open the file"

    def test_col_find_first_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_uint8,
            args=[src, index[0], 0, 251, 0x2F],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_first_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_uint16,
            args=[src, index[1], 0, 251, 0x2F30],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_first_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_uint32,
            args=[src, index[2], 0, 251, 0x2F303132],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_first_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_uint64,
            args=[src, index[3], 0, 251, 0x2F30313233343536],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_last_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_uint8,
            args=[src, index[0], 0, 250, 0x2F],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_last_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_uint16,
            args=[src, index[1], 0, 251, 0x2F30],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_last_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_uint32,
            args=[src, index[2], 0, 251, 0x2F303132],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_last_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_uint64,
            args=[src, index[3], 0, 251, 0x2F30313233343536],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_first_sub_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_sub_uint8,
            args=[src, index[0], 0, 7, 0, 251, 0x2F],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_first_sub_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_sub_uint16,
            args=[src, index[1], 0, 15, 0, 251, 0x2F30],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_first_sub_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_sub_uint32,
            args=[src, index[2], 8, 23, 0, 251, 0x00003031],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_first_sub_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_sub_uint64,
            args=[src, index[3], 16, 47, 0, 251, 0x0000000031323334],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_last_sub_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_sub_uint8,
            args=[src, index[0], 0, 7, 0, 251, 0x2F],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_last_sub_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_sub_uint16,
            args=[src, index[1], 0, 15, 0, 251, 0x2F30],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_last_sub_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_sub_uint32,
            args=[src, index[2], 8, 23, 0, 251, 0x00003031],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_find_last_sub_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_sub_uint64,
            args=[src, index[3], 16, 47, 0, 251, 0x0000000031323334],
            setup=setup,
            iterations=1,
            rounds=10000,
        )

    def test_col_tearDown(self):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        fd = -1
        size = 0
        if h != 0:
            assert False, "Error while closing the memory-mapped file"
