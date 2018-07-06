/*
 *  Error models
 *
 *  Michael Sullivan, University of Texas
 *
 */

#ifndef __EMODELS_H__
#define __EMODELS_H__

/* Internally, Pin recognizes registers with the following union (from Include/gen/types_vmapi.TLH)

   union PIN_REGISTER
   {
   UINT8  byte[MAX_BYTES_PER_PIN_REG];
   UINT16 word[MAX_WORDS_PER_PIN_REG];
   UINT32 dword[MAX_DWORDS_PER_PIN_REG];
   UINT64 qword[MAX_QWORDS_PER_PIN_REG];

   INT8   s_byte[MAX_BYTES_PER_PIN_REG];
   INT16  s_word[MAX_WORDS_PER_PIN_REG];
   INT32  s_dword[MAX_DWORDS_PER_PIN_REG];
   INT64  s_qword[MAX_QWORDS_PER_PIN_REG];

   FLT32  flt[MAX_FLOATS_PER_PIN_REG];
   FLT64  dbl[MAX_DOUBLES_PER_PIN_REG];
   };

   All error modes operate on the data in UINT formats.
 */

#include <stdlib.h>
#include <string>
#include <map>

/* redefine Pin types (to allow unit testing) */
#ifdef UNIT_TESTING

#include <stdint.h>

// from ${PIN_HOME}/source/include/pin/gen/types_foundation.TLH
typedef uint8_t  UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef int8_t  INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;

// from ${PIN_HOME}/source/include/pin/gen/pin_util.H
#define KILO 1024
#define MEGA (KILO*KILO)
#define GIGA (KILO*MEGA)

// from ${PIN_HOME}/source/include/pin/gen/types_marker.TLH
#define GLOBALFUN extern
#define LOCALFUN static
#define MEMBERVAR
#define GLOBALVAR 
#define LOCALVAR static

// from ${PIN_HOME}/source/include/pin/gen/message.PH
#define ASSERTX(condition) {}

#else

#include "instlib.H"

#endif

/* =====================================================================
 * RANDOM HELPERS
 * ===================================================================== */

// used to be mersenne twister, for simplicity just use rand() for now
UINT64 mt_random()
{
	UINT64 a = UINT64(rand());
	UINT64 b = UINT64(rand());
	UINT64 c = UINT64(rand());
	return (a << 34) + (b << 4) + c;
}

// small random number in between two bounds
UINT32 mt_randr(UINT32 min, UINT32 max)
{
	double scaled = (double)rand()/(RAND_MAX+1.0);
	return (UINT32)((max - min + 1)*scaled + min);
}

/* =====================================================================
 * SPECIAL ERROR MODES
 * ===================================================================== */

// NONE
// don't inject an error

UINT8 NONE_inject8(UINT8 tgt)
{
	return tgt;
}
UINT16 NONE_inject16(UINT16 tgt)
{
	return tgt;
}
UINT32 NONE_inject32(UINT32 tgt)
{
	return tgt;
}
UINT64 NONE_inject64(UINT64 tgt)
{
	return tgt;
}

/* =====================================================================
 * ERROR (Helper, not used directly)
 * used for unsupported or meaningless error modes
 * ===================================================================== */

// flip a burst of bits in a sub-word

	template <class intType>
intType ERR_inject(intType tgt)
{
	ASSERTX(0);
	return 0;
}

/* =====================================================================
 * BURSTBITS (Helper, not used directly)
 * ===================================================================== */

// flip a burst of bits in a sub-word

	template <class intType>
intType formBurst(intType bmask, intType min_shift, intType max_shift)
{
	intType ret = bmask << (intType)mt_randr((UINT32)min_shift, (UINT32)max_shift);
	return ret;
}

	template <class intType>
intType BB_inject(intType tgt, intType bmask, intType min_shift, intType max_shift)
{
	return tgt ^ formBurst <intType> (bmask, min_shift, max_shift);
}

/* =====================================================================
 * MASKEDRANDOM (Helper, not used directly)
 * ===================================================================== */

// insert a random value, subject to a mask

// note: assumes unchecked casts
	template <class intType>
intType MR_inject(intType tgt, intType bmask)
{
	return (tgt & ~bmask) | ((intType)mt_random() & bmask);
}

/* =====================================================================
 * MASKEDRANDOMBITS (Helper, not used directly)
 * ===================================================================== */

// flip N random bits, subject to a mask

/*template <class intType>
  intType bitsSet (intType v)
  {
  intType c; // c accumulates the total bits set in v

  for (c = 0; v; v >>= 1)
  {
  c += v & 1;
  }

  return c;
  }*/

// Brian Kernighan's method
// requires one iteration per bit set
	template <class intType>
intType bitsSet (intType v)
{
	intType c;      // c accumulates the total bits set in v
	for (c = 0; v; c++) {
		v &= v - 1; // clear the least significant bit set
	}
	return c;
}

// Note: can hang on invalid inputs! (No input checking.)
	template <class intType>
intType genMaskedRandBits (intType num_bits, intType bmask)
{
	intType tgt = 0;
	/* set one bit, using BB_Inject, until num_bits have been set */
	while (bitsSet(tgt) < num_bits) {
		tgt ^= formBurst <intType> (0x1, 0, (intType)(sizeof(intType)<<3)) & bmask;
	}
	return tgt;
}

	template <class intType>
intType MRB_inject (intType tgt, intType bmask, intType num_bits)
{
	return tgt ^ genMaskedRandBits <intType> (num_bits, bmask);
}

/* =====================================================================
 * Derived Error Modes, Integer -- BURSTY
 * ===================================================================== */

// BURST1
// appropriate for logical operations

UINT8 BURST1_inject8(UINT8 tgt)
{
	return BB_inject <UINT8> (tgt, 0x1, 0, 7);
}
UINT16 BURST1_inject16(UINT16 tgt)
{
	return BB_inject <UINT16> (tgt, 0x1, 0, 15);
}
UINT32 BURST1_inject32(UINT32 tgt)
{
	return BB_inject <UINT32> (tgt, 0x1, 0, 31);
}
UINT64 BURST1_inject64(UINT64 tgt)
{
	return BB_inject <UINT64> (tgt, 0x1, 0, 63);
}

// BURST2
// burst of two bits

UINT8 BURST2_inject8(UINT8 tgt)
{
	return BB_inject <UINT8> (tgt, 0x3, 0, 6);
}
UINT16 BURST2_inject16(UINT16 tgt)
{
	return BB_inject <UINT16> (tgt, 0x3, 0, 14);
}
UINT32 BURST2_inject32(UINT32 tgt)
{
	return BB_inject <UINT32> (tgt, 0x3, 0, 30);
}
UINT64 BURST2_inject64(UINT64 tgt)
{
	return BB_inject <UINT64> (tgt, 0x3, 0, 62);
}

// BURST4
// burst of four bits

UINT8 BURST4_inject8(UINT8 tgt)
{
	return BB_inject <UINT8> (tgt, 0xF, 0, 4);
}
UINT16 BURST4_inject16(UINT16 tgt)
{
	return BB_inject <UINT16> (tgt, 0xF, 0, 12);
}
UINT32 BURST4_inject32(UINT32 tgt)
{
	return BB_inject <UINT32> (tgt, 0xF, 0, 28);
}
UINT64 BURST4_inject64(UINT64 tgt)
{
	return BB_inject <UINT64> (tgt, 0xF, 0, 60);
}

// BURST8
// burst of eight bits

UINT8 BURST8_inject8(UINT8 tgt)
{
	return BB_inject <UINT8> (tgt, 0xFF, 0, 0);
}
UINT16 BURST8_inject16(UINT16 tgt)
{
	return BB_inject <UINT16> (tgt, 0xFF, 0, 8);
}
UINT32 BURST8_inject32(UINT32 tgt)
{
	return BB_inject <UINT32> (tgt, 0xFF, 0, 24);
}
UINT64 BURST8_inject64(UINT64 tgt)
{
	return BB_inject <UINT64> (tgt, 0xFF, 0, 56);
}

/* =====================================================================
 * Derived Error Modes, Integer -- RANDOM
 * ===================================================================== */

// RANDOM, LOW HALF
// random confined to low half of bits

UINT8 RANDOM_LH_inject8(UINT8 tgt)
{
	return MR_inject <UINT8> (tgt, 0x0F);
}
UINT16 RANDOM_LH_inject16(UINT16 tgt)
{
	return MR_inject <UINT16> (tgt, 0x00FF);
}
UINT32 RANDOM_LH_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x0000FFFF);
}
UINT64 RANDOM_LH_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x00000000FFFFFFFF);
}

// RANDOM
// strong synthetic error model

UINT8 RANDOM_inject8(UINT8 tgt)
{
	return MR_inject <UINT8> (tgt, 0xFF);
}
UINT16 RANDOM_inject16(UINT16 tgt)
{
	return MR_inject <UINT16> (tgt, 0xFFFF);
}
UINT32 RANDOM_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0xFFFFFFFF);
}
UINT64 RANDOM_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0xFFFFFFFFFFFFFFFF);
}

/* =====================================================================
 * Derived Error Modes, Integer -- RANDOM BIT FLIP
 * ===================================================================== */

// RANDBIT1
// appropriate for logical operations

UINT8 RANDBIT1_inject8(UINT8 tgt)
{
	return MRB_inject <UINT8> (tgt, 0xFF, 1);
}
UINT16 RANDBIT1_inject16(UINT16 tgt)
{
	return MRB_inject <UINT16> (tgt, 0xFFFF, 1);
}
UINT32 RANDBIT1_inject32(UINT32 tgt)
{
	return MRB_inject <UINT32> (tgt, 0xFFFFFFFF, 1);
}
UINT64 RANDBIT1_inject64(UINT64 tgt)
{
	return MRB_inject <UINT64> (tgt, 0xFFFFFFFFFFFFFFFF, 1);
}

// RANDBIT2
// random bit flip of two bits

UINT8 RANDBIT2_inject8(UINT8 tgt)
{
	return MRB_inject <UINT8> (tgt, 0xFF, 2);
}
UINT16 RANDBIT2_inject16(UINT16 tgt)
{
	return MRB_inject <UINT16> (tgt, 0xFFFF, 2);
}
UINT32 RANDBIT2_inject32(UINT32 tgt)
{
	return MRB_inject <UINT32> (tgt, 0xFFFFFFFF, 2);
}
UINT64 RANDBIT2_inject64(UINT64 tgt)
{
	return MRB_inject <UINT64> (tgt, 0xFFFFFFFFFFFFFFFF, 2);
}

// RANDBIT4
// random bit flip of four bits

UINT8 RANDBIT4_inject8(UINT8 tgt)
{
	return MRB_inject <UINT8> (tgt, 0xFF, 4);
}
UINT16 RANDBIT4_inject16(UINT16 tgt)
{
	return MRB_inject <UINT16> (tgt, 0xFFFF, 4);
}
UINT32 RANDBIT4_inject32(UINT32 tgt)
{
	return MRB_inject <UINT32> (tgt, 0xFFFFFFFF, 4);
}
UINT64 RANDBIT4_inject64(UINT64 tgt)
{
	return MRB_inject <UINT64> (tgt, 0xFFFFFFFFFFFFFFFF, 4);
}

// RANDBIT8
// random bit flip of eight bits

UINT8 RANDBIT8_inject8(UINT8 tgt)
{
	return MRB_inject <UINT8> (tgt, 0xFF, 8);
}
UINT16 RANDBIT8_inject16(UINT16 tgt)
{
	return MRB_inject <UINT16> (tgt, 0xFFFF, 8);
}
UINT32 RANDBIT8_inject32(UINT32 tgt)
{
	return MRB_inject <UINT32> (tgt, 0xFFFFFFFF, 8);
}
UINT64 RANDBIT8_inject64(UINT64 tgt)
{
	return MRB_inject <UINT64> (tgt, 0xFFFFFFFFFFFFFFFF, 8);
}

/* =====================================================================
 * Derived Error Modes, Floating Point -- BURSTY MANTISSA
 * ===================================================================== */

// MANTB1
// 1-bit error, confined to mantissa

UINT8 MANTB1_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTB1_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTB1_inject32(UINT32 tgt)
{
	return BB_inject <UINT32> (tgt, 0x1, 0, 22);
}
UINT64 MANTB1_inject64(UINT64 tgt)
{
	return BB_inject <UINT64> (tgt, 0x1, 0, 51);
}

// MANTB2
// burst of two bits, confined to mantissa

UINT8 MANTB2_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTB2_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTB2_inject32(UINT32 tgt)
{
	return BB_inject <UINT32> (tgt, 0x3, 0, 21);
}
UINT64 MANTB2_inject64(UINT64 tgt)
{
	return BB_inject <UINT64> (tgt, 0x3, 0, 50);
}

// MANTB4
// burst of four bits, confined to mantissa

UINT8 MANTB4_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTB4_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);    // unsupported FP format
}
UINT32 MANTB4_inject32(UINT32 tgt)
{
	return BB_inject <UINT32> (tgt, 0xF, 0, 19);
}
UINT64 MANTB4_inject64(UINT64 tgt)
{
	return BB_inject <UINT64> (tgt, 0xF, 0, 48);
}

// MANTB8
// burst of eight bits, confined to mantissa

UINT8 MANTB8_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTB8_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);    // unsupported FP format
}
UINT32 MANTB8_inject32(UINT32 tgt)
{
	return BB_inject <UINT32> (tgt, 0xFF, 0, 15);
}
UINT64 MANTB8_inject64(UINT64 tgt)
{
	return BB_inject <UINT64> (tgt, 0xFF, 0, 44);
}

/* =====================================================================
 * Derived Error Modes, Floating Point -- RANDOM BIT FLIP IN MANTISSA
 * ===================================================================== */

// MANTR1
// 1-bit error, confined to mantissa

UINT8 MANTR1_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTR1_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTR1_inject32(UINT32 tgt)
{
	return MRB_inject <UINT32> (tgt, 0x007FFFFF, 1);
}
UINT64 MANTR1_inject64(UINT64 tgt)
{
	return MRB_inject <UINT64> (tgt, 0x000FFFFFFFFFFFFF, 1);
}

// MANTR2
// random bit flip of two bits, confined to mantissa

UINT8 MANTR2_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTR2_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTR2_inject32(UINT32 tgt)
{
	return MRB_inject <UINT32> (tgt, 0x007FFFFF, 2);
}
UINT64 MANTR2_inject64(UINT64 tgt)
{
	return MRB_inject <UINT64> (tgt, 0x000FFFFFFFFFFFFF, 2);
}

// MANTR4
// random bit flip of four bits, confined to mantissa

UINT8 MANTR4_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTR4_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);    // unsupported FP format
}
UINT32 MANTR4_inject32(UINT32 tgt)
{
	return MRB_inject <UINT32> (tgt, 0x007FFFFF, 4);
}
UINT64 MANTR4_inject64(UINT64 tgt)
{
	return MRB_inject <UINT64> (tgt, 0x000FFFFFFFFFFFFF, 4);
}

// MANTR8
// random bit flip of eight bits, confined to mantissa

UINT8 MANTR8_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTR8_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);    // unsupported FP format
}
UINT32 MANTR8_inject32(UINT32 tgt)
{
	return MRB_inject <UINT32> (tgt, 0x007FFFFF, 8);
}
UINT64 MANTR8_inject64(UINT64 tgt)
{
	return MRB_inject <UINT64> (tgt, 0x000FFFFFFFFFFFFF, 8);
}

/* =====================================================================
 * Derived Error Modes, Floating Point -- RANDOM SCRAMBLE IN MSB OF MANTISSA
 * ===================================================================== */

// MANTRAND
// scramble mantissa

UINT8 MANTRAND_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTRAND_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTRAND_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x007FFFFF);
}
UINT64 MANTRAND_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x000FFFFFFFFFFFFF);
}

// MANTRAND2
// scramble any but top 2 bits of mantissa

UINT8 MANTRAND2_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTRAND2_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTRAND2_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x001FFFFF);
}
UINT64 MANTRAND2_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x0003FFFFFFFFFFFF);
}

// MANTRAND4
// scramble any but top 4 bits of mantissa

UINT8 MANTRAND4_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTRAND4_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTRAND4_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x0007FFFF);
}
UINT64 MANTRAND4_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x0000FFFFFFFFFFFF);
}

// MANTRAND8
// scramble any but top 8 bits of mantissa

UINT8 MANTRAND8_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTRAND8_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTRAND8_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x00007FFF);
}
UINT64 MANTRAND8_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x00000FFFFFFFFFFF);
}

// MANTRAND16
// scramble any but top 16 bits of mantissa

UINT8 MANTRAND16_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTRAND16_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTRAND16_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x0000007F);
}
UINT64 MANTRAND16_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x0000000FFFFFFFFF);
}

// MANTRAND2I
// scramble top 2 bits of mantissa

UINT8 MANTRAND2I_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTRAND2I_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTRAND2I_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x00600000);
}
UINT64 MANTRAND2I_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x000C000000000000);
}

// MANTRAND4I
// scramble top 4 bits of mantissa

UINT8 MANTRAND4I_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTRAND4I_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTRAND4I_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x00780000);
}
UINT64 MANTRAND4I_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x000F000000000000);
}

// MANTRAND8I
// scramble top 8 bits of mantissa

UINT8 MANTRAND8I_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTRAND8I_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTRAND8I_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x007F8000);
}
UINT64 MANTRAND8I_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x000FF00000000000);
}

// MANTRAND16I
// scramble top 16 bits of mantissa

UINT8 MANTRAND16I_inject8(UINT8 tgt)
{
	return ERR_inject <UINT8> (tgt);    // unsupported FP format
}
UINT16 MANTRAND16I_inject16(UINT16 tgt)
{
	return ERR_inject <UINT16> (tgt);   // unsupported FP format
}
UINT32 MANTRAND16I_inject32(UINT32 tgt)
{
	return MR_inject <UINT32> (tgt, 0x007FFF80);
}
UINT64 MANTRAND16I_inject64(UINT64 tgt)
{
	return MR_inject <UINT64> (tgt, 0x000FFFF000000000);
}

/* containers for error model function pointers */

typedef UINT8 (*EMF8)(UINT8);
typedef UINT16 (*EMF16)(UINT16);
typedef UINT32 (*EMF32)(UINT32);
typedef UINT64 (*EMF64)(UINT64);

typedef struct EMODEL_FUNCS {
	EMF8 f8;
	EMF16 f16;
	EMF32 f32;
	EMF64 f64;
} EMODEL_FUNCS;

/* string -> func pointer struct mapping */

std::map< std::string, EMODEL_FUNCS > create_emodel_map()
{
	std::map< std::string, EMODEL_FUNCS > m;

	// NONE
	EMODEL_FUNCS none_farray = {&NONE_inject8, &NONE_inject16, &NONE_inject32, &NONE_inject64};
	m[std::string("NONE")] = none_farray;

	// BURSTY
	EMODEL_FUNCS burst1_farray = {&BURST1_inject8, &BURST1_inject16, &BURST1_inject32, &BURST1_inject64};
	EMODEL_FUNCS burst2_farray = {&BURST2_inject8, &BURST2_inject16, &BURST2_inject32, &BURST2_inject64};
	EMODEL_FUNCS burst4_farray = {&BURST4_inject8, &BURST4_inject16, &BURST4_inject32, &BURST4_inject64};
	EMODEL_FUNCS burst8_farray = {&BURST8_inject8, &BURST8_inject16, &BURST8_inject32, &BURST8_inject64};
	m[std::string("BURST1")] = burst1_farray;
	m[std::string("BURST2")] = burst2_farray;
	m[std::string("BURST4")] = burst4_farray;
	m[std::string("BURST8")] = burst8_farray;

	// RANDOM
	EMODEL_FUNCS random_farray = {&RANDOM_inject8, &RANDOM_inject16, &RANDOM_inject32, &RANDOM_inject64};
	EMODEL_FUNCS random_lh_farray = {&RANDOM_LH_inject8, &RANDOM_LH_inject16, &RANDOM_LH_inject32, &RANDOM_LH_inject64};
	m[std::string("RANDOM")] = random_farray;
	m[std::string("RANDOM_LH")] = random_lh_farray;

	// RANDOM BIT FLIP
	EMODEL_FUNCS randbit1_farray = {&RANDBIT1_inject8, &RANDBIT1_inject16, &RANDBIT1_inject32, &RANDBIT1_inject64};
	EMODEL_FUNCS randbit2_farray = {&RANDBIT2_inject8, &RANDBIT2_inject16, &RANDBIT2_inject32, &RANDBIT2_inject64};
	EMODEL_FUNCS randbit4_farray = {&RANDBIT4_inject8, &RANDBIT4_inject16, &RANDBIT4_inject32, &RANDBIT4_inject64};
	EMODEL_FUNCS randbit8_farray = {&RANDBIT8_inject8, &RANDBIT8_inject16, &RANDBIT8_inject32, &RANDBIT8_inject64};
	m[std::string("RANDBIT1")] = randbit1_farray;
	m[std::string("RANDBIT2")] = randbit2_farray;
	m[std::string("RANDBIT4")] = randbit4_farray;
	m[std::string("RANDBIT8")] = randbit8_farray;

	// BURSTY MANTISSA
	EMODEL_FUNCS mantb1_farray = {&MANTB1_inject8, &MANTB1_inject16, &MANTB1_inject32, &MANTB1_inject64};
	EMODEL_FUNCS mantb2_farray = {&MANTB2_inject8, &MANTB2_inject16, &MANTB2_inject32, &MANTB2_inject64};
	EMODEL_FUNCS mantb4_farray = {&MANTB4_inject8, &MANTB4_inject16, &MANTB4_inject32, &MANTB4_inject64};
	EMODEL_FUNCS mantb8_farray = {&MANTB8_inject8, &MANTB8_inject16, &MANTB8_inject32, &MANTB8_inject64};
	m[std::string("MANTB1")] = mantb1_farray;
	m[std::string("MANTB2")] = mantb2_farray;
	m[std::string("MANTB4")] = mantb4_farray;
	m[std::string("MANTB8")] = mantb8_farray;

	// RANDBIT MANTISSA
	EMODEL_FUNCS mantR1_farray = {&MANTR1_inject8, &MANTR1_inject16, &MANTR1_inject32, &MANTR1_inject64};
	EMODEL_FUNCS mantR2_farray = {&MANTR2_inject8, &MANTR2_inject16, &MANTR2_inject32, &MANTR2_inject64};
	EMODEL_FUNCS mantR4_farray = {&MANTR4_inject8, &MANTR4_inject16, &MANTR4_inject32, &MANTR4_inject64};
	EMODEL_FUNCS mantR8_farray = {&MANTR8_inject8, &MANTR8_inject16, &MANTR8_inject32, &MANTR8_inject64};
	m[std::string("MANTR1")] = mantR1_farray;
	m[std::string("MANTR2")] = mantR2_farray;
	m[std::string("MANTR4")] = mantR4_farray;
	m[std::string("MANTR8")] = mantR8_farray;

	// RANDOM MANTISSA
	EMODEL_FUNCS mantRAND_farray = {&MANTRAND_inject8, &MANTRAND_inject16, &MANTRAND_inject32, &MANTRAND_inject64};
	EMODEL_FUNCS MANTRAND2I_farray = {&MANTRAND2I_inject8, &MANTRAND2I_inject16, &MANTRAND2I_inject32, &MANTRAND2I_inject64};
	EMODEL_FUNCS MANTRAND4I_farray = {&MANTRAND4I_inject8, &MANTRAND4I_inject16, &MANTRAND4I_inject32, &MANTRAND4I_inject64};
	EMODEL_FUNCS MANTRAND8I_farray = {&MANTRAND8I_inject8, &MANTRAND8I_inject16, &MANTRAND8I_inject32, &MANTRAND8I_inject64};
	EMODEL_FUNCS MANTRAND16I_farray = {&MANTRAND16I_inject8, &MANTRAND16I_inject16, &MANTRAND16I_inject32, &MANTRAND16I_inject64};
	m[std::string("MANTRAND")] = mantRAND_farray;
	m[std::string("MANTRAND2I")] = MANTRAND2I_farray;
	m[std::string("MANTRAND4I")] = MANTRAND4I_farray;
	m[std::string("MANTRAND8I")] = MANTRAND8I_farray;
	m[std::string("MANTRAND16I")] = MANTRAND16I_farray;

	EMODEL_FUNCS MANTRAND2_farray = {&MANTRAND2_inject8, &MANTRAND2_inject16, &MANTRAND2_inject32, &MANTRAND2_inject64};
	EMODEL_FUNCS MANTRAND4_farray = {&MANTRAND4_inject8, &MANTRAND4_inject16, &MANTRAND4_inject32, &MANTRAND4_inject64};
	EMODEL_FUNCS MANTRAND8_farray = {&MANTRAND8_inject8, &MANTRAND8_inject16, &MANTRAND8_inject32, &MANTRAND8_inject64};
	EMODEL_FUNCS MANTRAND16_farray = {&MANTRAND16_inject8, &MANTRAND16_inject16, &MANTRAND16_inject32, &MANTRAND16_inject64};
	m[std::string("MANTRAND")] = mantRAND_farray;
	m[std::string("MANTRAND2")] = MANTRAND2_farray;
	m[std::string("MANTRAND4")] = MANTRAND4_farray;
	m[std::string("MANTRAND8")] = MANTRAND8_farray;
	m[std::string("MANTRAND16")] = MANTRAND16_farray;

	return m;
}
static std::map< std::string, EMODEL_FUNCS > emodel_map = create_emodel_map();

#endif // #ifndef __EMODELS_H__
