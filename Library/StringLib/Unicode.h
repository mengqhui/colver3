//
/// @file Library/StringLib/Unicode.h
///
/// Unicode
///

#pragma once
#ifndef __UNICODE_CHAR_HEADER__
#define __UNICODE_CHAR_HEADER__

#include <Library/StringLib.h>

// CHARACTER_RANGE
/// A range of character values
typedef struct _CHARACTER_RANGE CHARACTER_RANGE;
struct _CHARACTER_RANGE {

  // Start
  /// The starting range for valid characters
  UINT32 Start;
  // End
  /// The ending range for valid characters
  UINT32 End;

};

// mValidCharRanges
/// The valid character ranges
STATIC CHARACTER_RANGE mValidCharRanges[] = {
  // C0, Basic Latin, C1, Latin-1, Extended Latin-A, Extended Latin-B,
  // IPA Extensions, Spacing Modifier Letters, Combining Diacritical marks
  { 0x00000000, 0x00000377 },
  // Greek, Coptic, Cyrillic
  { 0x0000037A, 0x0000037F },
  { 0x00000384, 0x0000038A },
  { 0x0000038C, 0x0000038C },
  { 0x0000038E, 0x000003A1 },
  { 0x000003A3, 0x0000052F },
  // Armenian
  { 0x00000531, 0x00000556 },
  { 0x00000559, 0x0000055F },
  { 0x00000561, 0x00000587 },
  { 0x00000589, 0x0000058A },
  { 0x0000058D, 0x0000058F },
  // Hebrew
  { 0x00000591, 0x000005C7 },
  { 0x000005D0, 0x000005EA },
  { 0x000005F0, 0x000005F4 },
  // Arabic
  { 0x00000600, 0x0000061C },
  { 0x0000061E, 0x0000070D },
  // Syriac
  { 0x0000070F, 0x0000074A },
  // Arabic supplement, Thaana
  { 0x0000074D, 0x000007B1 },
  // NKo
  { 0x000007C0, 0x000007FA },
  // Samaritan
  { 0x00000800, 0x0000082D },
  { 0x00000830, 0x0000083E },
  // Mandaic
  { 0x00000840, 0x0000085B },
  { 0x0000085E, 0x0000085E },
  // Arabic Extended-A
  { 0x000008A0, 0x000008B4 },
  { 0x000008B6, 0x000008BD },
  // Devanagari
  { 0x000008D4, 0x00000983 },
  // Bengali
  { 0x00000985, 0x0000098C },
  { 0x0000098F, 0x00000990 },
  { 0x00000993, 0x000009A8 },
  { 0x000009AA, 0x000009B0 },
  { 0x000009B2, 0x000009B2 },
  { 0x000009B6, 0x000009B9 },
  { 0x000009BC, 0x000009C4 },
  { 0x000009C7, 0x000009C8 },
  { 0x000009CB, 0x000009CE },
  { 0x000009D7, 0x000009D7 },
  { 0x0000070F, 0x0000074A },
  { 0x000009DC, 0x000009DD },
  { 0x000009DF, 0x000009E3 },
  { 0x000009E6, 0x000009FB },
  // Gurmukhi
  { 0x00000A01, 0x00000A03 },
  { 0x00000A05, 0x00000A0A },
  { 0x00000A0F, 0x00000A10 },
  { 0x00000A13, 0x00000A28 },
  { 0x00000A2A, 0x00000A30 },
  { 0x00000A32, 0x00000A33 },
  { 0x00000A35, 0x00000A36 },
  { 0x00000A38, 0x00000A39 },
  { 0x00000A3C, 0x00000A3C },
  { 0x00000A3E, 0x00000A42 },
  { 0x00000A47, 0x00000A48 },
  { 0x00000A4B, 0x00000A4D },
  { 0x00000A51, 0x00000A51 },
  { 0x00000A59, 0x00000A5C },
  { 0x00000A5E, 0x00000A5E },
  { 0x00000A66, 0x00000A75 },
  // Gujarati
  { 0x00000A81, 0x00000A83 },
  { 0x00000A85, 0x00000A8D },
  { 0x00000A8F, 0x00000A91 },
  { 0x00000A93, 0x00000AA8 },
  { 0x00000AAA, 0x00000AB0 },
  { 0x00000AB2, 0x00000AB3 },
  { 0x00000AB5, 0x00000AB9 },
  { 0x00000ABC, 0x00000AC5 },
  { 0x00000AC7, 0x00000AC9 },
  { 0x00000ACB, 0x00000ACD },
  { 0x00000AD0, 0x00000AD0 },
  { 0x00000AE0, 0x00000AE3 },
  { 0x00000AE6, 0x00000AF1 },
  { 0x00000AF9, 0x00000AF9 },
  // Oriya
  { 0x00000B01, 0x00000B03 },
  { 0x00000B05, 0x00000B0C },
  { 0x00000B0F, 0x00000B10 },
  { 0x00000B13, 0x00000B28 },
  { 0x00000B2A, 0x00000B30 },
  { 0x00000B32, 0x00000B33 },
  { 0x00000B35, 0x00000B39 },
  { 0x00000B3C, 0x00000B44 },
  { 0x00000B47, 0x00000B48 },
  { 0x00000B4B, 0x00000B4D },
  { 0x00000B56, 0x00000B57 },
  { 0x00000B5C, 0x00000B5D },
  { 0x00000B5F, 0x00000B63 },
  { 0x00000B66, 0x00000B77 },
  // Tamil
  { 0x00000B82, 0x00000B83 },
  { 0x00000B85, 0x00000B8A },
  { 0x00000B8E, 0x00000B90 },
  { 0x00000B92, 0x00000B95 },
  { 0x00000B99, 0x00000B9A },
  { 0x00000B9C, 0x00000B9C },
  { 0x00000B9E, 0x00000B9F },
  { 0x00000BA3, 0x00000BA4 },
  { 0x00000BA9, 0x00000BAA },
  { 0x00000BAE, 0x00000BB9 },
  { 0x00000BBE, 0x00000BC2 },
  { 0x00000BC6, 0x00000BC8 },
  { 0x00000BCA, 0x00000BCD },
  { 0x00000BD0, 0x00000BD0 },
  { 0x00000BD7, 0x00000BD7 },
  { 0x00000BE6, 0x00000BFA },
  // Telugu
  { 0x00000C00, 0x00000C03 },
  { 0x00000C05, 0x00000C0C },
  { 0x00000C0E, 0x00000C10 },
  { 0x00000C12, 0x00000C28 },
  { 0x00000C2A, 0x00000C39 },
  { 0x00000C3D, 0x00000C44 },
  { 0x00000C46, 0x00000C48 },
  { 0x00000C4A, 0x00000C4D },
  { 0x00000C55, 0x00000C56 },
  { 0x00000C58, 0x00000C5A },
  { 0x00000C60, 0x00000C63 },
  { 0x00000C66, 0x00000C6F },
  // Kannada
  { 0x00000C78, 0x00000C7F },
  { 0x00000C80, 0x00000C83 },
  { 0x00000C85, 0x00000C8C },
  { 0x00000C8E, 0x00000C90 },
  { 0x00000C92, 0x00000CA8 },
  { 0x00000CAA, 0x00000CB3 },
  { 0x00000CB5, 0x00000CB9 },
  { 0x00000CBC, 0x00000CC4 },
  { 0x00000CC6, 0x00000CC8 },
  { 0x00000CCA, 0x00000CCD },
  { 0x00000CD5, 0x00000CD6 },
  { 0x00000CDE, 0x00000CDE },
  { 0x00000CE0, 0x00000CE3 },
  { 0x00000CE6, 0x00000CEF },
  { 0x00000CF1, 0x00000CF2 },
  // Malayalam
  { 0x00000D01, 0x00000D03 },
  { 0x00000D05, 0x00000D0C },
  { 0x00000D0E, 0x00000D10 },
  { 0x00000D12, 0x00000D3A },
  { 0x00000D3D, 0x00000D4D },
  { 0x00000D01, 0x00000D03 },
  { 0x00000D46, 0x00000D48 },
  { 0x00000D4A, 0x00000D4F },
  { 0x00000D54, 0x00000D63 },
  { 0x00000D66, 0x00000D7F },
  // Sinhala
  { 0x00000D82, 0x00000D83 },
  { 0x00000D85, 0x00000D96 },
  { 0x00000D9A, 0x00000DB1 },
  { 0x00000DB3, 0x00000DBB },
  { 0x00000DBD, 0x00000DBD },
  { 0x00000DC0, 0x00000DC6 },
  { 0x00000DCA, 0x00000DCA },
  { 0x00000DCF, 0x00000DD4 },
  { 0x00000DD6, 0x00000DD6 },
  { 0x00000DD8, 0x00000DDF },
  { 0x00000DE6, 0x00000DEF },
  { 0x00000DF2, 0x00000DF4 },
  // Thai
  { 0x00000E01, 0x00000E3A },
  { 0x00000E3F, 0x00000E5B },
  // Lao
  { 0x00000E81, 0x00000E82 },
  { 0x00000E84, 0x00000E84 },
  { 0x00000E87, 0x00000E88 },
  { 0x00000E8A, 0x00000E8A },
  { 0x00000E8D, 0x00000E8D },
  { 0x00000E94, 0x00000E97 },
  { 0x00000E99, 0x00000E9F },
  { 0x00000EA1, 0x00000EA3 },
  { 0x00000EA5, 0x00000EA5 },
  { 0x00000EA7, 0x00000EA7 },
  { 0x00000EAA, 0x00000EAB },
  { 0x00000EAD, 0x00000EB9 },
  { 0x00000EBB, 0x00000EBD },
  { 0x00000EC0, 0x00000EC4 },
  { 0x00000EC6, 0x00000EC6 },
  { 0x00000EC8, 0x00000ECD },
  { 0x00000ED0, 0x00000ED9 },
  { 0x00000EDC, 0x00000EDF },
  // Tibetan
  { 0x00000F00, 0x00000F47 },
  { 0x00000F49, 0x00000F6C },
  { 0x00000F71, 0x00000F97 },
  { 0x00000F99, 0x00000FBC },
  { 0x00000FBE, 0x00000FCC },
  { 0x00000FCE, 0x00000FDA },
  // Myanmar, Georgian
  { 0x00001000, 0x000010C5 },
  { 0x000010C7, 0x000010C7 },
  { 0x000010CD, 0x000010CD },
  // Georgian, Hangul Jamo, Ethiopic
  { 0x000010D0, 0x00001248 },
  // Ethiopic
  { 0x0000124A, 0x0000124D },
  { 0x00001250, 0x00001256 },
  { 0x00001258, 0x00001258 },
  { 0x0000125A, 0x0000125D },
  { 0x00001260, 0x00001288 },
  { 0x0000128A, 0x0000128D },
  { 0x00001290, 0x000012B0 },
  { 0x000012B2, 0x000012B5 },
  { 0x000012B8, 0x000012BE },
  { 0x000012C0, 0x000012C0 },
  { 0x000012C2, 0x000012C5 },
  { 0x000012C8, 0x000012D6 },
  { 0x000012D8, 0x00001310 },
  { 0x00001312, 0x00001315 },
  { 0x00001318, 0x0000135A },
  { 0x0000135D, 0x0000137C },
  // Ethiopic Supplement
  { 0x00001380, 0x00001399 },
  // Cherokee
  { 0x000013A0, 0x000013F5 },
  { 0x000013F8, 0x000013FD },
  // Unified Canadian Aboriginal Syllabics, Ogham
  { 0x00001400, 0x0000169C },
  // Runic
  { 0x000016A0, 0x000016F8 },
  // Tagalog
  { 0x00001700, 0x0000170C },
  { 0x0000170E, 0x00001714 },
  // Hanunoo
  { 0x00001720, 0x00001736 },
  // Buhid
  { 0x00001740, 0x00001753 },
  // Tagbanwa
  { 0x00001760, 0x0000176C },
  { 0x0000176E, 0x00001770 },
  { 0x00001772, 0x00001773 },
  // Khmer
  { 0x00001780, 0x000017DD },
  { 0x000017E0, 0x000017E9 },
  { 0x000017F0, 0x000017F9 },
  // Mongolian
  { 0x00001800, 0x0000180E },
  { 0x00001810, 0x00001819 },
  { 0x00001820, 0x00001877 },
  { 0x00001880, 0x000018AA },
  // Unified Canadian Aboriginal Syllabics Extended
  { 0x000018B0, 0x000018F5 },
  // Limbu
  { 0x00001900, 0x0000191E },
  { 0x00001920, 0x0000192B },
  { 0x00001930, 0x0000193B },
  { 0x00001940, 0x00001940 },
  { 0x00001944, 0x0000194F },
  // Tai Le
  { 0x00001950, 0x0000196D },
  { 0x00001970, 0x00001974 },
  // New Tai Lue
  { 0x00001980, 0x000019AB },
  { 0x000019B0, 0x000019C9 },
  { 0x000019D0, 0x000019DA },
  // Khmer Symbols, Buginese
  { 0x000019DE, 0x00001A1B },
  // Tai Tham
  { 0x00001A1E, 0x00001A5E },
  { 0x00001A60, 0x00001A7C },
  { 0x00001A7F, 0x00001A89 },
  { 0x00001A90, 0x00001A99 },
  { 0x00001AA0, 0x00001AAD },
  // Combining Diacritical Marks Extended
  { 0x00001AB0, 0x00001ABE },
  // Balinese
  { 0x00001B00, 0x00001B4B },
  { 0x00001B50, 0x00001B7C },
  // Sundanese, Batak
  { 0x00001B80, 0x00001BF3 },
  // Lepcha
  { 0x00001BFC, 0x00001C37 },
  { 0x00001C3B, 0x00001C49 },
  // Ol Chiki, Cyrillic Extended-C
  { 0x00001C4D, 0x00001C88 },
  // Sundanese Supplement
  { 0x00001CC0, 0x00001CC7 },
  // Vedic Extensions
  { 0x00001CD0, 0x00001CF6 },
  { 0x00001CF8, 0x00001CF9 },
  // Phonetic Extensions, Phonetic Extensions Supplement, Combining Diacritical Marks Supplement
  { 0x00001D00, 0x00001DF5 },
  // Latin Extended Additional, Greek Extended
  { 0x00001DFB, 0x00001F15 },
  // Greek Extended
  { 0x00001F18, 0x00001F1D },
  { 0x00001F20, 0x00001F45 },
  { 0x00001F48, 0x00001F4D },
  { 0x00001F50, 0x00001F57 },
  { 0x00001F59, 0x00001F59 },
  { 0x00001F5B, 0x00001F5B },
  { 0x00001F5D, 0x00001F5D },
  { 0x00001F5F, 0x00001F7D },
  { 0x00001F80, 0x00001FB4 },
  { 0x00001FB6, 0x00001FC4 },
  { 0x00001FC6, 0x00001FD3 },
  { 0x00001FD6, 0x00001FDB },
  { 0x00001FDD, 0x00001FEF },
  { 0x00001FF2, 0x00001FF4 },
  { 0x00001FF6, 0x00001FFE },
  // General Punctuation
  { 0x00002000, 0x00002064 },
  { 0x00002066, 0x00002071 },
  // Superscripts and Subscripts
  { 0x00002074, 0x0000208E },
  { 0x00002090, 0x0000209C },
  // Currency Symbols
  { 0x000020A0, 0x000020BE },
  // Combining Diacritical Marks for Symbols
  { 0x000020D0, 0x000020F1 },
  // Letterlike Symbols, Number Forms
  { 0x00002100, 0x0000218B },
  // Arrows, Mathematical Operators, Miscellaneous Technical
  { 0x00002190, 0x000023FE },
  // Control Pictures
  { 0x00002400, 0x00002426 },
  // Optical Character Recognition
  { 0x00002440, 0x0000244A },
  // Enclosed Alphanumerics, Box Drawing, Box Elements, Geometric Shapes, Miscellaneous Symbols, Dingbats,
  // Miscellaneous Mathematical Symbols-A, Supplemental Arrows-A, Braille Patterns, Supplemental Arrows-B,
  // Miscellaneous Mathematical Symbols-B, Supplemental Mathematical Operators, Miscellaneous Symbols and Arrows
  { 0x00002460, 0x00002B73 },
  // Miscellaneous Symbols and Arrows
  { 0x00002B76, 0x00002B95 },
  { 0x00002B98, 0x00002BB9 },
  { 0x00002BBD, 0x00002BC8 },
  { 0x00002BCA, 0x00002BD1 },
  { 0x00002BEC, 0x00002BEF },
  // Glagolitic
  { 0x00002C00, 0x00002C2E },
  { 0x00002C30, 0x00002C5E },
  // Latin Extended-C, Coptic
  { 0x00002C60, 0x00002CF3 },
  // Georgian Supplement
  { 0x00002CF9, 0x00002D25 },
  { 0x00002D27, 0x00002D27 },
  { 0x00002D2D, 0x00002D2D },
  // Tifinagh
  { 0x00002D30, 0x00002D67 },
  { 0x00002D6F, 0x00002D70 },
  // Ethiopic Extended
  { 0x00002D7F, 0x00002D96 },
  { 0x00002DA0, 0x00002DA6 },
  { 0x00002DA8, 0x00002DAE },
  { 0x00002DB0, 0x00002DB6 },
  { 0x00002DB8, 0x00002DBE },
  { 0x00002DC0, 0x00002DC6 },
  { 0x00002DC8, 0x00002DCE },
  { 0x00002DD0, 0x00002DE6 },
  { 0x00002DD8, 0x00002DDE },
  // Cyrillic Extended-A, Supplemental Punctuation
  { 0x00002DE0, 0x00002E44 },
  // CJK Radicals Supplement
  { 0x00002E80, 0x00002E99 },
  { 0x00002E9B, 0x00002EF3 },
  // Kangxi Radicals
  { 0x00002F00, 0x00002FD5 },
  // Ideographic Description Characters
  { 0x00002FF0, 0x00002FFB },
  // CJK Symbols and Punctuation
  { 0x00003000, 0x0000303F },
  // Hiragana, Katakana
  { 0x00003041, 0x00003096 },
  { 0x00003099, 0x000030FF },
  // Bopomofo
  { 0x00003105, 0x0000312D },
  // Hangul Compatibility Jamo
  { 0x00003131, 0x0000318E },
  // Kanbun, Bopomofo Extended
  { 0x00003190, 0x000031BA },
  // CJK Strokes
  { 0x000031F0, 0x0000321E },
  // Katakana Phonetic Extensions, Enclosed CJK Letters and Months
  { 0x000031F0, 0x0000321E },
  { 0x00003220, 0x000032FE },
  // CJK Compatibility
  { 0x00003300, 0x000033FF },
  // CJK Unified Ideographs Extension A
  { 0x00003400, 0x00004DB5 },
  // Yijing Hexagram Symbols, CJK Unified Ideographs
  { 0x00004DC0, 0x00009FD5 },
  // Yi Syllables
  { 0x0000A000, 0x0000A48C },
  // Yi Radicals
  { 0x0000A490, 0x0000A4C6 },
  // Lisu
  { 0x0000A4D0, 0x0000A4FF },
  // Vai
  { 0x0000A500, 0x0000A62B },
  // Cyrillic Extended-B, Bamum
  { 0x0000A640, 0x0000A6F7 },
  // Modifier Tone Letters, Latin Extended-D
  { 0x0000A700, 0x0000A7B7 },
  // Syloti Nagri
  { 0x0000A7F7, 0x0000A82B },
  // Common Indic Number Forms
  { 0x0000A830, 0x0000A389 },
  // Phags-pa
  { 0x0000A840, 0x0000A877 },
  // Saurashtra
  { 0x0000A880, 0x0000A8C5 },
  { 0x0000A8CE, 0x0000A8D9 },
  // Devanagari Extended
  { 0x0000A8E0, 0x0000A8FD },
  // Kayah Li, Rejang
  { 0x0000A900, 0x0000A953 },
  // Hangul Jamo Extended-A
  { 0x0000A95F, 0x0000A97C },
  // Javanese
  { 0x0000A980, 0x0000A9CD },
  { 0x0000A9CF, 0x0000A9D9 },
  // Myanmar Extended-B
  { 0x0000A9DE, 0x0000A9FE },
  // Cham
  { 0x0000AA00, 0x0000AA36 },
  { 0x0000AA40, 0x0000AA4D },
  { 0x0000AA50, 0x0000AA59 },
  // Myanmar Extended-A, Tai Viet
  { 0x0000AA5C, 0x0000AAC2 },
  // Meetei Mayek Extensions
  { 0x0000AADB, 0x0000AAF6 },
  // Ethiopic Extended-A
  { 0x0000AB01, 0x0000AB06 },
  { 0x0000AB09, 0x0000AB0E },
  { 0x0000AB11, 0x0000AB16 },
  { 0x0000AB20, 0x0000AB26 },
  { 0x0000AB28, 0x0000AB2E },
  // Latin Extended-E
  { 0x0000AB30, 0x0000AB65 },
  // Cherokee Supplement, Meetei Mayek
  { 0x0000AB70, 0x0000ABED },
  { 0x0000ABF0, 0x0000ABF9 },
  // Hangul Syllables
  { 0x0000AC00, 0x0000D7A3 },
  // Hangul Jamo Extended-B
  { 0x0000D7B0, 0x0000D7C6 },
  { 0x0000D7CB, 0x0000D7FB },
  // CJK Compatibility Ideographs
  { 0x0000F900, 0x0000FA6D },
  { 0x0000FA70, 0x0000FAD9 },
  // Alphabetic Presentation Forms
  { 0x0000FB00, 0x0000FB06 },
  { 0x0000FB13, 0x0000FB17 },
  { 0x0000FB1D, 0x0000FB36 },
  { 0x0000FB38, 0x0000FB3C },
  { 0x0000FB3E, 0x0000FB3E },
  { 0x0000FB40, 0x0000FB41 },
  { 0x0000FB43, 0x0000FB44 },
  // Arabic Presentation Forms-A
  { 0x0000FB46, 0x0000FBC1 },
  { 0x0000FBD3, 0x0000FD3F },
  { 0x0000FD50, 0x0000FD8F },
  { 0x0000FD92, 0x0000FDC7 },
  { 0x0000FDF0, 0x0000FDFD },
  // Variation Selectors, Vertical Forms
  { 0x0000FE00, 0x0000FE19 },
  // Combining Half Marks, CJK Compatibility Forms
  { 0x0000FE20, 0x0000FE52 },
  // Small Form Variants
  { 0x0000FE54, 0x0000FE66 },
  { 0x0000FE68, 0x0000FE6B },
  // Arabic Presentation Forms-B
  { 0x0000FE70, 0x0000FE74 },
  { 0x0000FE76, 0x0000FEFC },
  // Byte order mark
  { 0x0000FEFF, 0x0000FEFF },
  // Halfwidth and Fullwidth Forms
  { 0x0000FF01, 0x0000FFBE },
  { 0x0000FFC2, 0x0000FFC7 },
  { 0x0000FFCA, 0x0000FFCF },
  { 0x0000FFD2, 0x0000FFD7 },
  { 0x0000FFDA, 0x0000FFDC },
  { 0x0000FFE0, 0x0000FFE6 },
  { 0x0000FFE8, 0x0000FFEE },
  // Specials
  { 0x0000FFF9, 0x0000FFFD },
  // Linear B Syllabary
  { 0x00010000, 0x0001000B },
  { 0x0001000D, 0x00010026 },
  { 0x00010028, 0x0001003A },
  { 0x0001003C, 0x0001003D },
  { 0x0001003F, 0x0001004D },
  { 0x00010050, 0x0001005D },
  // Linear B Ideograms
  { 0x00010080, 0x000100FA },
  // Aegean Numbers
  { 0x00010100, 0x00010102 },
  { 0x00010107, 0x00010133 },
  // Ancient Greek Numbers
  { 0x00010137, 0x0001018E },
  // Ancient Symbols
  { 0x00010190, 0x0001019B },
  { 0x000101A0, 0x000101A0 },
  // Phaistos Disc
  { 0x000101D0, 0x000101FD },
  // Lycian
  { 0x00010280, 0x0001029C },
  // Carian
  { 0x000102A0, 0x000102D0 },
  // Coptic Epact Numbers
  { 0x000102E0, 0x000102FB },
  // Old Italic
  { 0x00010300, 0x00010323 },
  // Gothic
  { 0x00010330, 0x0001034A },
  // Old Permic
  { 0x00010350, 0x0001037A },
  // Ugaritic
  { 0x00010380, 0x0001039D },
  // Old Persian
  { 0x0001039F, 0x000103C3 },
  { 0x000103C8, 0x000103D5 },
  // Deseret, Shavian, Osmanya
  { 0x00010400, 0x0001049D },
  { 0x000104A0, 0x000104A9 },
  // Osage
  { 0x000102B0, 0x000104D3 },
  { 0x000104D8, 0x000104FB },
  // Elbasan
  { 0x00010500, 0x00010527 },
  // Caucasian Albanian
  { 0x00010530, 0x00010563 },
  { 0x0001056F, 0x0001056F },
  // Linear A
  { 0x00010600, 0x00010736 },
  { 0x00010740, 0x00010755 },
  { 0x00010760, 0x00010767 },
  // Cypriot Syllabary
  { 0x00010800, 0x00010805 },
  { 0x00010808, 0x00010808 },
  { 0x0001080A, 0x00010835 },
  { 0x00010837, 0x00010838 },
  { 0x0001083C, 0x0001083C },
  // Imperial Aramaic, Palmyrene, Nabataean
  { 0x0001083F, 0x00010855 },
  { 0x00010857, 0x0001089E },
  { 0x000108A7, 0x000108AF },
  // Hatran
  { 0x000108E0, 0x000108F2 },
  { 0x000108F4, 0x000108F5 },
  // Phoenician
  { 0x000108FB, 0x0001091B },
  // Lydian
  { 0x0001091F, 0x00010939 },
  { 0x0001093F, 0x0001093F },
  // Meroitic Hieroglyphs
  { 0x00010980, 0x000109B7 },
  // Meroitic Cursive
  { 0x000109BC, 0x000109CF },
  { 0x000109D2, 0x00010A03 },
  // Kharoshthi
  { 0x00010A05, 0x00010A06 },
  { 0x00010A0C, 0x00010A13 },
  { 0x00010A15, 0x00010A17 },
  { 0x00010A19, 0x00010A33 },
  { 0x00010A38, 0x00010A3A },
  { 0x00010A3F, 0x00010A47 },
  { 0x00010A50, 0x00010A58 },
  // Old South Arabian, Old North Arabian
  { 0x00010A60, 0x00010A9F },
  // Manichaean
  { 0x00010AC0, 0x00010AE6 },
  { 0x00010AEB, 0x00010AF6 },
  // Avestan
  { 0x00010B00, 0x00010B35 },
  // Inscriptional Parthian
  { 0x00010B39, 0x00010B55 },
  // Inscriptional Pahlavi
  { 0x00010B58, 0x00010B72 },
  // Psalter Pahlavi
  { 0x00010B60, 0x00010B7E },
  { 0x00010B60, 0x00010B7E },
  { 0x00010B60, 0x00010B7E },
  // Old Turkic
  { 0x00010C00, 0x00010C48 },
  // Old Hungarian
  { 0x00010C80, 0x00010CB2 },
  { 0x00010CC0, 0x00010CF2 },
  { 0x00010CFA, 0x00010CFF },
  // Rumi Numeral Symbols
  { 0x00010E60, 0x00010E7E },
  // Brahmi
  { 0x00011000, 0x0001104D },
  { 0x00011052, 0x0001106F },
  { 0x0001107F, 0x0001107F },
  // Kaithi
  { 0x00011080, 0x000110C1 },
  // Sora Sompeng
  { 0x000110D0, 0x000110E8 },
  { 0x000110F0, 0x000110F9 },
  // Chakma
  { 0x00011100, 0x00011134 },
  { 0x00011136, 0x00011143 },
  // Mahajani
  { 0x00011150, 0x00011176 },
  // Sharada
  { 0x00011180, 0x000111CD },
  { 0x000111D0, 0x000111DF },
  // Sinhala Archaic Numbers
  { 0x000111E1, 0x000111F4 },
  // Khojki
  { 0x00011200, 0x00011211 },
  { 0x00011213, 0x0001123E },
  // Multani
  { 0x00011280, 0x00011286 },
  { 0x00011288, 0x00011288 },
  { 0x0001128A, 0x0001128D },
  { 0x0001128F, 0x0001129D },
  { 0x0001129F, 0x000112A9 },
  // Khudawadi
  { 0x000112B0, 0x000112EA },
  { 0x000112F0, 0x000112F9 },
  // Grantha
  { 0x00011300, 0x00011303 },
  { 0x00011305, 0x0001130C },
  { 0x0001130F, 0x00011310 },
  { 0x00011313, 0x00011328 },
  { 0x0001132A, 0x00011330 },
  { 0x00011332, 0x00011333 },
  { 0x00011335, 0x00011339 },
  { 0x0001133C, 0x00011344 },
  { 0x00011347, 0x00011348 },
  { 0x0001134B, 0x0001134D },
  { 0x00011350, 0x00011350 },
  { 0x00011357, 0x00011357 },
  { 0x00011300, 0x00011303 },
  { 0x0001135D, 0x00011363 },
  { 0x00011366, 0x0001136C },
  { 0x00011370, 0x00011374 },
  // Newa
  { 0x00011400, 0x00011459 },
  { 0x0001145B, 0x0001145B },
  { 0x0001145D, 0x0001145D },
  // Tirhuta
  { 0x00011480, 0x000114C7 },
  { 0x000114D0, 0x000114D9 },
  // Siddham
  { 0x00011580, 0x000115B5 },
  { 0x000115B8, 0x000115DD },
  // Modi
  { 0x00011600, 0x00011644 },
  { 0x00011650, 0x00011659 },
  // Mongolian Supplement
  { 0x00011660, 0x0001166C },
  // Takri
  { 0x00011680, 0x000116B7 },
  { 0x000116C0, 0x000116C9 },
  // Ahom
  { 0x00011700, 0x00011719 },
  { 0x0001171D, 0x0001172B },
  { 0x00011730, 0x0001173F },
  // Warang Citi
  { 0x000118A0, 0x000118F2 },
  { 0x000118FF, 0x000118FF },
  // Pau Cin Hau
  { 0x00011AC0, 0x00011AF8 },
  // Bhaiksuki
  { 0x00011C00, 0x00011C08 },
  { 0x00011C0A, 0x00011C36 },
  { 0x00011C38, 0x00011C45 },
  { 0x00011C50, 0x00011C6C },
  // Marchen
  { 0x00011C70, 0x00011C8F },
  { 0x00011C92, 0x00011CA7 },
  { 0x00011CA9, 0x00011CB6 },
  // Cuneiform
  { 0x00012000, 0x00012399 },
  // Cuneiform Numbers and Punctuation
  { 0x00012400, 0x0001246E },
  { 0x00012470, 0x00012474 },
  // Early Dynastic Cuneiform
  { 0x00012500, 0x00012543 },
  // Egyptian Hieroglyphs
  { 0x00013000, 0x0001342E },
  // Anatolian Hieroglyphs
  { 0x00014400, 0x00014646 },
  // Bamum Supplement
  { 0x00016800, 0x00016A38 },
  // Mro
  { 0x00016A40, 0x00016A5E },
  { 0x00016A60, 0x00016A69 },
  { 0x00016A6E, 0x00016A6F },
  // Bassa Vah
  { 0x00016AD0, 0x00016AED },
  { 0x00016AF0, 0x00016AF5 },
  // Pahawh Hmong
  { 0x00016B00, 0x00016B45 },
  { 0x00016B50, 0x00016B59 },
  { 0x00016B5B, 0x00016B61 },
  { 0x00016B63, 0x00016B77 },
  { 0x00016B7D, 0x00016B8F },
  // Miao
  { 0x00016F00, 0x00016F44 },
  { 0x00016F50, 0x00016F7E },
  { 0x00016F8F, 0x00016F9F },
  // Ideographic Symbols and Punctuation
  { 0x00016FE0, 0x00016FE0 },
  // Tangut
  { 0x00017000, 0x000187EC },
  // Tangut Components
  { 0x00018800, 0x00018AF2 },
  // Kana Supplement
  { 0x0001B000, 0x0001B001 },
  // Duployan
  { 0x0001BC00, 0x0001BC6A },
  { 0x0001BC70, 0x0001BC7C },
  { 0x0001BC80, 0x0001BC88 },
  { 0x0001BC90, 0x0001BC99 },
  // Shorthand Format Controls
  { 0x0001BC9C, 0x0001BCA3 },
  // Byzantine Musical Symbols
  { 0x0001D000, 0x0001D0F5 },
  // Musical Symbols
  { 0x0001D100, 0x0001D126 },
  { 0x0001D129, 0x0001D1E8 },
  // Ancient Greek Musical Notation
  { 0x0001D200, 0x0001D245 },
  // Tai Xuan Jing Symbols
  { 0x0001D300, 0x0001D356 },
  // Counting Rod Numerals
  { 0x0001D360, 0x0001D371 },
  // Mathematical Alphanumeric Symbols
  { 0x0001D400, 0x0001D454 },
  { 0x0001D456, 0x0001D49C },
  { 0x0001D49E, 0x0001D49F },
  { 0x0001D4A2, 0x0001D4A2 },
  { 0x0001D4A5, 0x0001D4A6 },
  { 0x0001D4A9, 0x0001D4AC },
  { 0x0001D4AE, 0x0001D4B9 },
  { 0x0001D4BB, 0x0001D4BB },
  { 0x0001D4BD, 0x0001D4C3 },
  { 0x0001D4C5, 0x0001D505 },
  { 0x0001D507, 0x0001D50A },
  { 0x0001D50D, 0x0001D514 },
  { 0x0001D516, 0x0001D51C },
  { 0x0001D51E, 0x0001D539 },
  { 0x0001D53B, 0x0001D53E },
  { 0x0001D540, 0x0001D544 },
  { 0x0001D546, 0x0001D546 },
  { 0x0001D54A, 0x0001D550 },
  { 0x0001D552, 0x0001D6A5 },
  { 0x0001D6A8, 0x0001D7CB },
  { 0x0001D7CE, 0x0001D7FF },
  // Sutton SignWriting
  { 0x0001D800, 0x0001DA8B },
  { 0x0001DA9B, 0x0001DA9F },
  { 0x0001DAA0, 0x0001DAAF },
  // Glagolitic Supplement
  { 0x0001E000, 0x0001E006 },
  { 0x0001E008, 0x0001E018 },
  { 0x0001E01B, 0x0001E021 },
  { 0x0001E023, 0x0001E024 },
  { 0x0001E026, 0x0001E02A },
  // Mende Kikakui
  { 0x0001E800, 0x0001E8C4 },
  { 0x0001E8C7, 0x0001E8D6 },
  // Adlam
  { 0x0001E900, 0x0001E94A },
  { 0x0001E950, 0x0001E959 },
  { 0x0001E95E, 0x0001E95F },
  // Arabic Mathematical Alphabetic Symbols
  { 0x0001EE00, 0x0001EE03 },
  { 0x0001EE05, 0x0001EE1F },
  { 0x0001EE21, 0x0001EE22 },
  { 0x0001EE24, 0x0001EE24 },
  { 0x0001EE27, 0x0001EE27 },
  { 0x0001EE29, 0x0001EE32 },
  { 0x0001EE34, 0x0001EE37 },
  { 0x0001EE39, 0x0001EE39 },
  { 0x0001EE3B, 0x0001EE3B },
  { 0x0001EE42, 0x0001EE42 },
  { 0x0001EE47, 0x0001EE47 },
  { 0x0001EE49, 0x0001EE49 },
  { 0x0001EE4B, 0x0001EE4B },
  { 0x0001EE4D, 0x0001EE4F },
  { 0x0001EE51, 0x0001EE52 },
  { 0x0001EE54, 0x0001EE54 },
  { 0x0001EE57, 0x0001EE57 },
  { 0x0001EE59, 0x0001EE59 },
  { 0x0001EE5B, 0x0001EE5B },
  { 0x0001EE5D, 0x0001EE5D },
  { 0x0001EE5F, 0x0001EE5F },
  { 0x0001EE61, 0x0001EE62 },
  { 0x0001EE64, 0x0001EE64 },
  { 0x0001EE67, 0x0001EE6A },
  { 0x0001EE6C, 0x0001EE72 },
  { 0x0001EE74, 0x0001EE77 },
  { 0x0001EE79, 0x0001EE7C },
  { 0x0001EE7E, 0x0001EE7E },
  { 0x0001EE80, 0x0001EE89 },
  { 0x0001EE8B, 0x0001EE9B },
  { 0x0001EEA1, 0x0001EEA3 },
  { 0x0001EEA5, 0x0001EEA9 },
  { 0x0001EEAB, 0x0001EEBB },
  { 0x0001EEF0, 0x0001EEF1 },
  // Mahjong Tiles
  { 0x0001F000, 0x0001F02B },
  // Domino Tiles
  { 0x0001F030, 0x0001F093 },
  // Playing Cards
  { 0x0001F0A0, 0x0001F0AE },
  { 0x0001F0B1, 0x0001F0BF },
  { 0x0001F0C1, 0x0001F0CF },
  { 0x0001F0C1, 0x0001F0DF },
  { 0x0001F0E0, 0x0001F0F5 },
  // Enclosed Alphanumeric Supplement
  { 0x0001F100, 0x0001F10C },
  { 0x0001F110, 0x0001F12E },
  { 0x0001F130, 0x0001F16B },
  { 0x0001F170, 0x0001F1AC },
  { 0x0001F1E6, 0x0001F202 },
  // Enclosed Ideographic Supplement
  { 0x0001F210, 0x0001F23B },
  { 0x0001F240, 0x0001F248 },
  { 0x0001F250, 0x0001F251 },
  // Miscellaneous Symbols and Pictographs, Emoticons, Ornamental Dingbats, Transport and Map Symbols
  { 0x0001F300, 0x0001F6D2 },
  { 0x0001F6E0, 0x0001F6EC },
  { 0x0001F6F0, 0x0001F6F6 },
  // Alchemical Symbols
  { 0x0001F700, 0x0001F773 },
  // Geometric Shapes Extended
  { 0x0001F780, 0x0001F7D4 },
  // Supplemental Arrows-C
  { 0x0001F080, 0x0001F80B },
  { 0x0001F810, 0x0001F847 },
  { 0x0001F850, 0x0001F859 },
  { 0x0001F860, 0x0001F887 },
  { 0x0001F890, 0x0001F8AD },
  // Supplemental Symbols and Pictographs
  { 0x0001F910, 0x0001F91E },
  { 0x0001F920, 0x0001F927 },
  { 0x0001F930, 0x0001F930 },
  { 0x0001F933, 0x0001F93E },
  { 0x0001F940, 0x0001F94B },
  { 0x0001F950, 0x0001F95E },
  { 0x0001F980, 0x0001F991 },
  { 0x0001F9C0, 0x0001F9C0 },
  // CJK Unified Ideographs Extension B
  { 0x00020000, 0x0002A6D6 },
  // CJK Unified Ideographs Extension C
  { 0x0002A700, 0x0002B734 },
  // CJK Unified Ideographs Extension D
  { 0x0002B740, 0x0002B81D },
  // CJK Unified Ideographs Extension E
  { 0x0002B820, 0x0002CEA1 },
  // CJK Compatibility Ideographs Supplement
  { 0x0002F800, 0x0002FA1D}
};

#endif // __UNICODE_CHAR_HEADER__