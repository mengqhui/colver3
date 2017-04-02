//
/// @file Library/PlatformLib/CPU/Intel.h
///
/// Platform information library Intel CPU models
///

#pragma once
#ifndef __CPU_MODELS_INTEL_HEADER__
#define __CPU_MODELS_INTEL_HEADER__

#include "../Platform.h"

#if defined(MDE_CPU_IA32)

//
// 486 compatible CPU models
//

// CPU_MODEL_486
/// 486 compatible CPU model
#define CPU_MODEL_486      0x400
// CPU_MODEL_486_DX
/// 486DX compatible CPU model
#define CPU_MODEL_486_DX   0x401
// CPU_MODEL_486_SX
/// 486SX compatible CPU model
#define CPU_MODEL_486_SX   0x402
// CPU_MODEL_486_DX2
/// 486DX2 compatible CPU model
#define CPU_MODEL_486_DX2  0x403
// CPU_MODEL_486_SL
/// 486SL compatible CPU model
#define CPU_MODEL_486_SL   0x404
// CPU_MODEL_486_SX2
/// 486SX2 compatible CPU model
#define CPU_MODEL_486_SX2  0x405
// CPU_MODEL_486_EDX2
/// 486EDX2 compatible CPU model
#define CPU_MODEL_486_EDX2 0x407
// CPU_MODEL_486_DX4
/// 486DX4 compatible CPU model
#define CPU_MODEL_486_DX4  0x408
// CPU_MODEL_486_EDX4
/// 486EDX4 compatible CPU model
#define CPU_MODEL_486_EDX4 0x409

//
// Intel Pentium compatible CPU models
//

// CPU_MODEL_PENTIUM
/// Pentium compatible CPU model
#define CPU_MODEL_PENTIUM     0x513
// CPU_MODEL_PENTIUM_B0
/// Pentium C compatible CPU model
#define CPU_MODEL_PENTIUM_B0  0x520
// CPU_MODEL_PENTIUM_B1
/// Pentium C compatible CPU model
#define CPU_MODEL_PENTIUM_B1  0x521
// CPU_MODEL_PENTIUM_B3
/// Pentium C compatible CPU model
#define CPU_MODEL_PENTIUM_B3  0x522
// CPU_MODEL_PENTIUM_B5
/// Pentium C compatible CPU model
#define CPU_MODEL_PENTIUM_B5  0x524
// CPU_MODEL_PENTIUM_C1
/// Pentium compatible CPU model
#define CPU_MODEL_PENTIUM_C1  0x515
// CPU_MODEL_PENTIUM_C2
/// Pentium compatible CPU model
#define CPU_MODEL_PENTIUM_C2  0x525
// CPU_MODEL_PENTIUM_D1
/// Pentium compatible CPU model
#define CPU_MODEL_PENTIUM_D1  0x517
// CPU_MODEL_PENTIUM_E0
/// Pentium compatible CPU model
#define CPU_MODEL_PENTIUM_E0  0x526
// CPU_MODEL_PENTIUM_cB1
/// Pentium secondary compatible CPU model
#define CPU_MODEL_PENTIUM_cB1 0x52B
// CPU_MODEL_PENTIUM_cC0
/// Pentium secondary compatible CPU model
#define CPU_MODEL_PENTIUM_cC0 0x52C

//
// Intel Mobile Pentium compatible CPU models
//

// CPU_MODEL_MOBILE_PENTIUM
/// Mobile Pentium compatible CPU model
#define CPU_MODEL_MOBILE_PENTIUM 0x570

//
// Intel Pentium MMX compatible CPU models
//

// CPU_MODEL_PENTIUM_MMX
/// Pentium MMX compatible CPU model
#define CPU_MODEL_PENTIUM_MMX    0x543
// CPU_MODEL_PENTIUM_MMX_B1
/// Pentium MMX compatible CPU model
#define CPU_MODEL_PENTIUM_MMX_B1 0x544

//
// Intel Mobile Pentium MMX compatible CPU models
//

// CPU_MODEL_MOBILE_PENTIUM_MMX
/// Mobile Pentium compatible CPU model
#define CPU_MODEL_MOBILE_PENTIUM_MMX    0x581
// CPU_MODEL_MOBILE_PENTIUM_MMX_B2
/// Mobile Pentium compatible CPU model
#define CPU_MODEL_MOBILE_PENTIUM_MMX_B2 0x582

//
// Intel Pentium secondary compatible CPU models
//

// CPU_MODEL_PENTIUM_SECONDARY
/// Pentium secondary compatible CPU model
#define CPU_MODEL_PENTIUM_SECONDARY    0x2521
// CPU_MODEL_PENTIUM_SECONDARY_B1
/// Pentium secondary compatible CPU model
#define CPU_MODEL_PENTIUM_SECONDARY_B1 0x252B
// CPU_MODEL_PENTIUM_SECONDARY_B3
/// Pentium secondary compatible CPU model
#define CPU_MODEL_PENTIUM_SECONDARY_B3 0x2522
// CPU_MODEL_PENTIUM_SECONDARY_B5
/// Pentium secondary compatible CPU model
#define CPU_MODEL_PENTIUM_SECONDARY_B5 0x2524
// CPU_MODEL_PENTIUM_SECONDARY_C0
/// Pentium secondary compatible CPU model
#define CPU_MODEL_PENTIUM_SECONDARY_C0 0x252C
// CPU_MODEL_PENTIUM_SECONDARY_C2
/// Pentium secondary compatible CPU model
#define CPU_MODEL_PENTIUM_SECONDARY_C2 0x2525
// CPU_MODEL_PENTIUM_SECONDARY_E0
/// Pentium secondary compatible CPU model
#define CPU_MODEL_PENTIUM_SECONDARY_E0 0x2526

//
// Intel Pentium Overdrive compatible CPU models
//

// CPU_MODEL_PENTIUM_OVERDRIVE
/// Pentium Overdrive compatible CPU model
#define CPU_MODEL_PENTIUM_OVERDRIVE    0x51A
// CPU_MODEL_PENTIUM_OVERDRIVE_B1
/// Pentium Overdrive compatible CPU model
#define CPU_MODEL_PENTIUM_OVERDRIVE_B1 0x152B
// CPU_MODEL_PENTIUM_OVERDRIVE_B2
/// Pentium Overdrive compatible CPU model
#define CPU_MODEL_PENTIUM_OVERDRIVE_B2 0x1531
// CPU_MODEL_PENTIUM_OVERDRIVE_B3
/// Pentium Overdrive compatible CPU model
#define CPU_MODEL_PENTIUM_OVERDRIVE_B3 0x1522
// CPU_MODEL_PENTIUM_OVERDRIVE_B5
/// Pentium Overdrive compatible CPU model
#define CPU_MODEL_PENTIUM_OVERDRIVE_B5 0x1524
// CPU_MODEL_PENTIUM_OVERDRIVE_cC0
/// Pentium Overdrive compatible CPU model
#define CPU_MODEL_PENTIUM_OVERDRIVE_C0 0x152C
// CPU_MODEL_PENTIUM_OVERDRIVE_C2
/// Pentium Overdrive compatible CPU model
#define CPU_MODEL_PENTIUM_OVERDRIVE_C2 0x1525
// CPU_MODEL_PENTIUM_OVERDRIVE_E0
/// Pentium Overdrive compatible CPU model
#define CPU_MODEL_PENTIUM_OVERDRIVE_E0 0x1526

//
// Intel Pentium MMX Overdrive compatible CPU models
//

// CPU_MODEL_PENTIUM_MMX_OVERDRIVE
/// Pentium MMX compatible CPU model
#define CPU_MODEL_PENTIUM_MMX_OVERDRIVE    0x1543
// CPU_MODEL_PENTIUM_MMX_OVERDRIVE_B1
/// Pentium MMX compatible CPU model
#define CPU_MODEL_PENTIUM_MMX_OVERDRIVE_B1 0x1544

//
// Intel P6/Pentium Pro compatible CPU models
//

// CPU_MODEL_PENTIUM_PRO
/// P6/Pentium Pro compatible CPU model
#define CPU_MODEL_PENTIUM_PRO     0x610
// CPU_MODEL_PENTIUM_PRO_B0
/// P6/Pentium Pro compatible CPU model
#define CPU_MODEL_PENTIUM_PRO_B0  0x611
// CPU_MODEL_PENTIUM_PRO_C0
/// P6/Pentium Pro compatible CPU model
#define CPU_MODEL_PENTIUM_PRO_C0  0x612
// CPU_MODEL_PENTIUM_PRO_sA0
/// P6/Pentium Pro compatible CPU model
#define CPU_MODEL_PENTIUM_PRO_sA0 0x616
// CPU_MODEL_PENTIUM_PRO_sA1
/// P6/Pentium Pro compatible CPU model
#define CPU_MODEL_PENTIUM_PRO_sA1 0x617
// CPU_MODEL_PENTIUM_PRO_sB0
/// P6/Pentium Pro compatible CPU model
#define CPU_MODEL_PENTIUM_PRO_sB0 0x619

//
// Intel Pentium II compatible CPU models
//

// CPU_MODEL_PENTIUM_II
/// Pentium II compatible CPU model
#define CPU_MODEL_PENTIUM_II     0x633
// CPU_MODEL_PENTIUM_II_C1
/// Pentium II compatible CPU model
#define CPU_MODEL_PENTIUM_II_C1  0x634
// CPU_MODEL_PENTIUM_II_dA0
/// Pentium II compatible CPU model
#define CPU_MODEL_PENTIUM_II_dA0 0x650
// CPU_MODEL_PENTIUM_II_dA1
/// Pentium II compatible CPU model
#define CPU_MODEL_PENTIUM_II_dA1 0x651
// CPU_MODEL_PENTIUM_II_dB0
/// Pentium II compatible CPU model
#define CPU_MODEL_PENTIUM_II_dB0 0x652
// CPU_MODEL_PENTIUM_II_dB0
/// Pentium II compatible CPU model
#define CPU_MODEL_PENTIUM_II_dB1 0x653

//
// Intel Pentium II Overdrive compatible CPU model
//

// CPU_MODEL_PENTIUM_II_OVERDRIVE
/// Pentium II Overdrive compatible CPU model
#define CPU_MODEL_PENTIUM_II_OVERDRIVE 0x1632

//
// Intel Mobile Pentium II compatible CPU models
//

// CPU_MODEL_MOBILE_PENTIUM_II
/// Mobile Pentium II compatible CPU model
#define CPU_MODEL_MOBILE_PENTIUM_II 0x66A

//
// Intel Pentium III compatible CPU models
//

// CPU_MODEL_PENTIUM_III
/// Pentium III compatible CPU model
#define CPU_MODEL_PENTIUM_III     0x672
// CPU_MODEL_PENTIUM_III_kC0
/// Pentium III compatible CPU model
#define CPU_MODEL_PENTIUM_III_kC0 0x673
// CPU_MODEL_PENTIUM_III_cA0
/// Pentium III compatible CPU model
#define CPU_MODEL_PENTIUM_III_cA0 0x681
// CPU_MODEL_PENTIUM_III_cB0
/// Pentium III compatible CPU model
#define CPU_MODEL_PENTIUM_III_cB0 0x683
// CPU_MODEL_PENTIUM_III_cC0
/// Pentium III compatible CPU model
#define CPU_MODEL_PENTIUM_III_cC0 0x686
// CPU_MODEL_PENTIUM_III_cD0
/// Pentium III compatible CPU model
#define CPU_MODEL_PENTIUM_III_cD0 0x68A
// CPU_MODEL_PENTIUM_III_tA0
/// Pentium III compatible CPU model
#define CPU_MODEL_PENTIUM_III_tA0 0x6B0
// CPU_MODEL_PENTIUM_III_tA1
/// Pentium III compatible CPU model
#define CPU_MODEL_PENTIUM_III_tA1 0x6B1
// CPU_MODEL_PENTIUM_III_tB1
/// Pentium III compatible CPU model
#define CPU_MODEL_PENTIUM_III_tB1 0x6B4

//
// Intel Xeon compatible CPU models
//

// CPU_MODEL_XEON_PENTIUM_III
/// Pentium III Xeon compatible CPU model
#define CPU_MODEL_XEON_PENTIUM_III 0x6A4

#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

//
// Intel Pentium 4 compatible CPU models
//

// CPU_MODEL_PENTIUM_4
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4      0xF02
// CPU_MODEL_PENTIUM_4_B0
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_B0   0xF24
// CPU_MODEL_PENTIUM_4_B2
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_B2   0xF07
// CPU_MODEL_PENTIUM_4_C0
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_C0   0xF33
// CPU_MODEL_PENTIUM_4_C1
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_C1   0xF0A
// CPU_MODEL_PENTIUM_4_C1_2
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_C1_2 0xF27
// CPU_MODEL_PENTIUM_4_C1_3
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_C1_3 0xF37
// CPU_MODEL_PENTIUM_4_D0
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_D0   0xF12
// CPU_MODEL_PENTIUM_4_D0_2
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_D0_2 0xF34
// CPU_MODEL_PENTIUM_4_D1
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_D1   0xF29
// CPU_MODEL_PENTIUM_4_E0
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_E0   0xF41
// CPU_MODEL_PENTIUM_4_G1
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_G1   0xF49
// CPU_MODEL_PENTIUM_4_M0
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_M0   0xF25
// CPU_MODEL_PENTIUM_4_N0
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_N0   0xF43
// CPU_MODEL_PENTIUM_4_R0
/// Pentium 4 compatible CPU model
#define CPU_MODEL_PENTIUM_4_R0   0xF4A

//
// Intel Pentium D compatible CPU models
//

// CPU_MODEL_PENTIUM_D
/// Pentium D compatible CPU model
#define CPU_MODEL_PENTIUM_D    0xF44
// CPU_MODEL_PENTIUM_D_B0
/// Pentium D compatible CPU model
#define CPU_MODEL_PENTIUM_D_B0 0xF47
// CPU_MODEL_PENTIUM_D_B1
/// Pentium D compatible CPU model
#define CPU_MODEL_PENTIUM_D_B1 0xF62
// CPU_MODEL_PENTIUM_D_C1
/// Pentium D compatible CPU model
#define CPU_MODEL_PENTIUM_D_C1 0xF64
// CPU_MODEL_PENTIUM_D_D0
/// Pentium D compatible CPU model
#define CPU_MODEL_PENTIUM_D_D0 0xF65

//
// Intel Pentium M compatible CPU models
//

// CPU_MODEL_PENTIUM_M
/// Pentium M compatible CPU model
#define CPU_MODEL_PENTIUM_M    0x695
// CPU_MODEL_PENTIUM_M_B1
/// Pentium M Dothan compatible CPU model
#define CPU_MODEL_PENTIUM_M_B1 0x6D6
// CPU_MODEL_PENTIUM_M_C0
/// Pentium M Dothan compatible CPU model
#define CPU_MODEL_PENTIUM_M_C0 0x6D8

//
// Intel embedded compatible CPU models
//

// CPU_MODEL_INTEL_EMBEDDED
/// Intel embedded compatible CPU model
#define CPU_MODEL_INTEL_EMBEDDED    0x10650
// CPU_MODEL_INTEL_EMBEDDED_B1
/// Intel embedded compatible CPU model
#define CPU_MODEL_INTEL_EMBEDDED_B1 0x10651

//
// Intel Celeron D compatible CPU models
//

// CPU_MODEL_CELERON
/// Celeron compatible CPU model
#define CPU_MODEL_CELERON    0x660
// CPU_MODEL_CELERON
/// Celeron compatible CPU model
#define CPU_MODEL_CELERON_A1 0x661
// CPU_MODEL_CELERON
/// Celeron compatible CPU model
#define CPU_MODEL_CELERON_B0 0x665

//
// Intel Celeron D compatible CPU models
//

// CPU_MODEL_CELERON
/// Celeron D compatible CPU model
#define CPU_MODEL_CELERON_D 0x10661

//
// Intel Core compatible CPU models
//

// CPU_MODEL_CORE
/// Core compatible CPU model
#define CPU_MODEL_CORE    0x6E8
// CPU_MODEL_CORE_D0
/// Core compatible CPU model
#define CPU_MODEL_CORE_D0 0x6EC

//
// Intel Core2 compatible CPU models
//

// CPU_MODEL_CORE2
/// Core2 compatible CPU model
#define CPU_MODEL_CORE2    0x6F2
// CPU_MODEL_CORE2_B1
/// Core2 compatible CPU model
#define CPU_MODEL_CORE2_B1 0x6F5
// CPU_MODEL_CORE2_B2
/// Core2 compatible CPU model
#define CPU_MODEL_CORE2_B2 0x6F6
// CPU_MODEL_CORE2_B3
/// Core2 compatible CPU model
#define CPU_MODEL_CORE2_B3 0x6F7
// CPU_MODEL_CORE2_G0
/// Core2 compatible CPU model
#define CPU_MODEL_CORE2_G0 0x6FB
// CPU_MODEL_CORE2_M0
/// Core2 compatible CPU model
#define CPU_MODEL_CORE2_M0 0x10671
// CPU_MODEL_CORE2_M1
/// Core2 compatible CPU model
#define CPU_MODEL_CORE2_M1 0x10676
// CPU_MODEL_CORE2_R0
/// Core2 compatible CPU model
#define CPU_MODEL_CORE2_R0 0x1067A

//
// Intel Core2 Mobile compatible CPU models
//

// CPU_MODEL_CORE2_MOBILE
/// Core2 Mobile compatible CPU model
#define CPU_MODEL_CORE2_MOBILE    0x6F4
// CPU_MODEL_CORE2_MOBILE_E0
/// Core2 Mobile compatible CPU model
#define CPU_MODEL_CORE2_MOBILE_E0 0x6F9
// CPU_MODEL_CORE2_MOBILE_E1
/// Core2 Mobile compatible CPU model
#define CPU_MODEL_CORE2_MOBILE_E1 0x6FA
// CPU_MODEL_CORE2_MOBILE_M1
/// Core2 Mobile compatible CPU model
#define CPU_MODEL_CORE2_MOBILE_M1 0x10674

//
// Intel Core i compatible CPU models
//

// CPU_MODEL_CORE_i
/// Core i compatible CPU model
#define CPU_MODEL_CORE_i    0x106A4
// CPU_MODEL_CORE_i_C0
/// Core i compatible CPU model
#define CPU_MODEL_CORE_i_C0 0x106E5
// CPU_MODEL_CORE_i_D0
/// Core i compatible CPU model
#define CPU_MODEL_CORE_i_D0 0x106A5

//
// Intel 2nd Gen Core i compatible CPU models
//

// CPU_MODEL_2ND_CORE_i
/// 2nd Gen Core i compatible CPU model
#define CPU_MODEL_2ND_CORE_i    0x206A4
// CPU_MODEL_CORE_i_D2
/// 2nd Gen Core i compatible CPU model
#define CPU_MODEL_2ND_CORE_i_D2 0x206A5
// CPU_MODEL_2ND_CORE_i_J1
/// 2nd Gen Core i compatible CPU model
#define CPU_MODEL_2ND_CORE_i_J1 0x206A7

//
// Intel 3rd Gen Core i compatible CPU models
//

// CPU_MODEL_3RD_CORE_i
/// 3rd Gen Core i compatible CPU model
#define CPU_MODEL_3RD_CORE_i    0x306A4
// CPU_MODEL_3RD_CORE_i_E2
/// 3rd Gen Core i compatible CPU model
#define CPU_MODEL_3RD_CORE_i_E2 0x306A5

//
// Intel 4th Gen Core i compatible CPU models
//

// CPU_MODEL_4TH_CORE_i
/// 4th Gen Core i compatible CPU model
#define CPU_MODEL_4TH_CORE_i 0x306E4

//
// Intel 5th Gen Core i compatible CPU models
//

// CPU_MODEL_5TH_CORE_i
/// 5th Gen Core i compatible CPU model
#define CPU_MODEL_5TH_CORE_i 0x40671

//
// Intel 6th Gen Core i compatible CPU models
//

// CPU_MODEL_6TH_CORE_i
/// 6th Gen Core i compatible CPU model
#define CPU_MODEL_6TH_CORE_i 0x506E3

//
// Intel 7th Gen Core i compatible CPU models
//

//
// Intel Core i Mobile compatible CPU models
//

// CPU_MODEL_CORE_i_MOBILE
/// Core i Mobile compatible CPU model
#define CPU_MODEL_CORE_i_MOBILE 0x106F1

//
// Intel 2nd Gen Core i Mobile compatible CPU models
//

// CPU_MODEL_2ND_CORE_i_MOBILE
/// 2nd Gen Core i Mobile compatible CPU model
#define CPU_MODEL_2ND_CORE_i_MOBILE    0x20650
// CPU_MODEL_2ND_CORE_i_MOBILE_A1
/// 2nd Gen Core i Mobile compatible CPU model
#define CPU_MODEL_2ND_CORE_i_MOBILE_A1 0x20651
// CPU_MODEL_2ND_CORE_i_MOBILE_A2
/// 2nd Gen Core i Mobile compatible CPU model
#define CPU_MODEL_2ND_CORE_i_MOBILE_A2 0x20652
// CPU_MODEL_2ND_CORE_i_MOBILE
/// 2nd Gen Core i Mobile compatible CPU model
#define CPU_MODEL_2ND_CORE_i_MOBILE_K0 0x20655

//
// Intel 3rd Gen Core i Mobile compatible CPU models
//

// CPU_MODEL_3RD_CORE_i_MOBILE
/// 3rd Gen Core i Mobile compatible CPU model
#define CPU_MODEL_3RD_CORE_i_MOBILE    0x306C3
// CPU_MODEL_3RD_CORE_i_MOBILE_D0
/// 3rd Gen Core i Mobile compatible CPU model
#define CPU_MODEL_3RD_CORE_i_MOBILE_D0 0x306D4
// CPU_MODEL_3RD_CORE_i_MOBILE_E1
/// 3rd Gen Core i Mobile compatible CPU model
#define CPU_MODEL_3RD_CORE_i_MOBILE_E1 0x306A9

//
// Intel 4th Gen Core i Mobile compatible CPU models
//

// CPU_MODEL_4TH_CORE_i_MOBILE
/// 4th Gen Core i Mobile compatible CPU model
#define CPU_MODEL_4TH_CORE_i_MOBILE 0x40651

//
// Intel 5th Gen Core i Model compatible CPU models
//

// CPU_MODEL_5TH_CORE_i_MOBILE
/// 5th Gen Core i Mobile compatible CPU model
#define CPU_MODEL_5TH_CORE_i_MOBILE 0x40661

//
// Intel 6th Gen Core i Mobile compatible CPU models
//

// CPU_MODEL_6TH_CORE_i_MOBILE
/// 6th Gen Core i Mobile compatible CPU model
#define CPU_MODEL_6TH_CORE_i_MOBILE 0x406E3

//
// 7th Gen Core i compatible CPU models
//

//
// Intel Xeon compatible CPU models
//

// CPU_MODEL_XEON
/// Xeon compatible CPU model
#define CPU_MODEL_XEON    0xF48
// CPU_MODEL_XEON_B0
/// Xeon compatible CPU model
#define CPU_MODEL_XEON_B0 0xF31
// CPU_MODEL_XEON_B1
/// Xeon compatible CPU model
#define CPU_MODEL_XEON_B1 0xF32

//
// Intel Xeon MP compatible CPU models
//

// CPU_MODEL_XEON_MP
/// Xeon MP compatible CPU model
#define CPU_MODEL_XEON_MP    0xF11
// CPU_MODEL_XEON_MP_A0
/// Xeon MP compatible CPU model
#define CPU_MODEL_XEON_MP_A0 0xF22
// CPU_MODEL_XEON_MP_C0
/// Xeon MP compatible CPU model
#define CPU_MODEL_XEON_MP_C0 0xF26

//
// Intel Core2 Xeon compatible CPU models
//

// CPU_MODEL_XEON_A1
/// Xeon compatible CPU model
#define CPU_MODEL_XEON_CORE2 0x106D1

//
// Intel Core i Xeon compatible CPU models
//

// CPU_MODEL_XEON_CORE_i
/// Xeon Core i compatible CPU model
#define CPU_MODEL_XEON_CORE_i    0x206F2
// CPU_MODEL_XEON_CORE_i_D0
/// Xeon Core i compatible CPU model
#define CPU_MODEL_XEON_CORE_i_D0 0x206E6

//
// Intel 2nd Gen Core i Xeon compatible CPU models
//

// CPU_MODEL_XEON_2ND_CORE_i
/// Xeon 2nd Gen Core i compatible CPU model
#define CPU_MODEL_XEON_2ND_CORE_i    0x206A1
// CPU_MODEL_XEON_2ND_CORE_i_B0
/// Xeon 2nd Gen Core i compatible CPU model
#define CPU_MODEL_XEON_2ND_CORE_i_B0 0x206C1
// CPU_MODEL_XEON_2ND_CORE_i_B1
/// Xeon 2nd Gen Core i compatible CPU model
#define CPU_MODEL_XEON_2ND_CORE_i_B1 0x206C2
// CPU_MODEL_XEON_2ND_CORE_i_C0
/// Xeon 2nd Gen Core i compatible CPU model
#define CPU_MODEL_XEON_2ND_CORE_i_C0 0x206D5
// CPU_MODEL_XEON_2ND_CORE_i_C1
/// Xeon 2nd Gen Core i compatible CPU model
#define CPU_MODEL_XEON_2ND_CORE_i_C1 0x206D6
// CPU_MODEL_XEON_2ND_CORE_i_C2
/// Xeon 2nd Gen Core i compatible CPU model
#define CPU_MODEL_XEON_2ND_CORE_i_C2 0x206D7
  

//
// Intel 3rd Gen Core i Xeon compatible CPU models
//

// CPU_MODEL_XEON_3RD_CORE_i
/// Xeon 3rd Gen Core i compatible CPU model
#define CPU_MODEL_XEON_3RD_CORE_i    0x306A8
// CPU_MODEL_XEON_3RD_CORE_i_R1
/// Xeon 3rd Gen Core i compatible CPU model
#define CPU_MODEL_XEON_3RD_CORE_i_R1 0x306F1
// CPU_MODEL_XEON_3RD_CORE_i_R2
/// Xeon 3rd Gen Core i compatible CPU model
#define CPU_MODEL_XEON_3RD_CORE_i_R2 0x306F2

//
// Intel 4th Gen Core i Xeon compatible CPU models
//

// CPU_MODEL_XEON_4TH_CORE_i
/// Xeon 4th Gen Core i compatible CPU model
#define CPU_MODEL_XEON_4TH_CORE_i    0x406F1
// CPU_MODEL_XEON_4TH_CORE_i_M0
/// Xeon 4th Gen Core i compatible CPU model
#define CPU_MODEL_XEON_4TH_CORE_i_M0 0x406F3

//
// Intel 5th Gen Core i Xeon compatible CPU models
//

//
// Intel 6th Gen Core i Xeon compatible CPU models
//

//
// Intel 7th Gen Core i Xeon compatible CPU models
//

//
// Intel Xeon Phi compatible CPU models
//

//
// Intel Atom compatible CPU models
//

// CPU_MODEL_ATOM
/// Atom compatible CPU model
#define CPU_MODEL_ATOM    0x106C2
// CPU_MODEL_ATOM_A0
/// Atom compatible CPU model
#define CPU_MODEL_ATOM_A0 0x106CA
// CPU_MODEL_ATOM_B1
/// Atom compatible CPU model
#define CPU_MODEL_ATOM_B1 0x20661
// CPU_MODEL_ATOM_B2
/// Atom compatible CPU model
#define CPU_MODEL_ATOM_B2 0x30661

//
// Intel 2nd Gen Atom compatible CPU models
//

// CPU_MODEL_2ND_ATOM
/// 2nd Gen Atom compatible CPU model
#define CPU_MODEL_2ND_ATOM    0x30673
// CPU_MODEL_2ND_ATOM_C0
/// 2nd Gen Atom compatible CPU model
#define CPU_MODEL_2ND_ATOM_C0 0x30678

//
// Intel 3rd Gen Atom compatible CPU models
//

// CPU_MODEL_3RD_ATOM
// 3rd Gen Atom compatible CPU model
#define CPU_MODEL_3RD_ATOM    0x406C3
// CPU_MODEL_3RD_ATOM_C1
// 3rd Gen Atom compatible CPU model
#define CPU_MODEL_3RD_ATOM_C1 0x406C4

//
// Intel 4th Gen Atom compatible CPU models
//

#endif

#endif // __CPU_MODELS_INTEL_HEADER__
