#include <stdio.h>
#include <stdint.h>
#include <float.h>

// stdint.h-like typedefs
// TODO: check machine "float" and "double" representations?
typedef double  float64_t;

typedef union {
	struct
	{
		uint64_t normalized_significand : 52, biased_exponent : 11, sign_bit : 1;
		/* uint64_t normalized_significand : 52; */
		/* uint16_t biased_exponent : 11; */
		/* uint8_t sign_bit : 1; */
	};  // anonymous structure
	float64_t fp_bits;
	uint64_t uint_bits;
} DoubleU;

/* Print n as a binary number */
	template <class intType>
void printbits_fp64(intType n)
{
	intType i;
	i = (intType)0x1 << ((sizeof(n) << 3) - 1);

	intType sign_bit = (intType)0x1 << ((sizeof(n) << 3) - 2);
	intType exponent_bit = (intType)0x1 << ((sizeof(n) << 3) - 13);

	while (i > 0)
	{
		if (n & i)
			printf("1");
		else
			printf("0");
		i >>= 1;

		if (i == sign_bit || i == exponent_bit)
			printf(" ");
	}
}

void print_fp(float64_t const &f, bool print_binary=false) {
	DoubleU fu;
	fu.fp_bits = f;
	if (print_binary) {
		printbits_fp64<uint64_t>(fu.uint_bits);
		printf("\n");
	}
	else {
		printf("%f -> %a -> %u %u %lu\n", f, fu.fp_bits, fu.sign_bit, fu.biased_exponent, fu.normalized_significand);
	}
	return;
}

inline float64_t trucate_value(float64_t const &d, int32_t fraction_bits) __attribute__((const));
float64_t trucate_value(float64_t const &d, int32_t fraction_bits) {

	DoubleU du = { .fp_bits = d };  // initialize the union "view" of d

	// special cases
	if (du.biased_exponent == 0 || du.biased_exponent == 2047) // zero, NaN, inf
		return d;

	// round down
	int32_t signed_exponent = ((int32_t)du.biased_exponent - 1023);
	int32_t bits_to_keep = fraction_bits + signed_exponent;
	if (bits_to_keep >= 0) {  // number has some non-truncated part
		/* uint64_t ns_copy = du.normalized_significand; */
		du.normalized_significand >>= (52 - bits_to_keep);
		du.normalized_significand <<= (52 - bits_to_keep);
		/* du.normalized_significand = ns_copy; */
	}
	else {  // truncate the full number
		du.biased_exponent = 0;
		du.normalized_significand = 0;
	}

	return du.fp_bits;
}

template <int32_t fraction_bits>
inline float64_t new_trucate_value(float64_t const &d) __attribute__((const));
template <int32_t fraction_bits>
float64_t new_trucate_value(float64_t const &d) {

	DoubleU du = { .fp_bits = d };  // initialize the union "view" of d

	// special cases
	if (du.biased_exponent == 0 || du.biased_exponent == 2047) // zero, NaN, inf
		return d;

	// round down
	int32_t signed_exponent = ((int32_t)du.biased_exponent - 1023);
	int32_t bits_to_keep = fraction_bits + signed_exponent;
	if (bits_to_keep >= 0) {  // number has some non-truncated part
		/* uint64_t ns_copy = du.normalized_significand; */
		du.normalized_significand >>= (52 - bits_to_keep);
		du.normalized_significand <<= (52 - bits_to_keep);
		/* du.normalized_significand = ns_copy; */
	}
	else {  // truncate the full number
		du.biased_exponent = 0;
		du.normalized_significand = 0;
	}

	return du.fp_bits;
}

inline float64_t round_towards_zero(float64_t const d) {
	if (d < 0.0)
		return ceil(d);
	else
		return floor(d);
}

inline float64_t old_trucate_value(float64_t d, int32_t fraction_bits) __attribute__((const));
float64_t old_trucate_value(float64_t d, int32_t fraction_bits) {
	d *= pow(2, fraction_bits);
	d = round_towards_zero(d);
	d /= pow(2, fraction_bits);
	return d;
}

template<uint32_t sat_bits>
inline float64_t sat(float64_t const &d) __attribute__((const));
template<uint32_t sat_bits>
float64_t sat(float64_t const &d) {

	DoubleU du = { .fp_bits = d };  // initialize the union "view" of d

	// special cases
	if (du.biased_exponent == 0 || du.biased_exponent == 2047) // zero, NaN, inf
		return d;

	// saturate the value
	int32_t signed_exponent = ((int32_t)du.biased_exponent - 1023);
	if (signed_exponent > (int32_t)sat_bits-1){
		du.biased_exponent = sat_bits + 1023 - 1;
		du.normalized_significand = 0xFFFFFFFFFFFFF;
	}

	return du.fp_bits;
}

#define RADIX 256
float64_t orig_old_sat(float64_t v) {
	if(v*RADIX > 32767) return (float64_t(32767) / RADIX);
	else if(v*RADIX < -32767) return (float64_t(-32767) / RADIX);
	return v;
}

template<uint32_t fraction_bits, uint32_t sat_bits>
inline float64_t old_sat(float64_t d) __attribute__((const));
template<uint32_t fraction_bits, uint32_t sat_bits>
float64_t old_sat(float64_t v) {
	if (v*(2^fraction_bits) > (2^sat_bits)-1) return ((float64_t)(2^sat_bits)-1)/(2^fraction_bits);
	else if (v*(2^fraction_bits) < -1*(2^sat_bits)-1) return ((float64_t)-1*(2^sat_bits)-1)/(2^fraction_bits);
	return v;
}

template<uint32_t sat_bits>
inline float64_t alt_sat(float64_t d) __attribute__((const));
template<uint32_t sat_bits>
float64_t alt_sat(float64_t d) {
	if (d >= (2^sat_bits)){
		d = (2^sat_bits)-DBL_MIN;
	}
	else if (d <= -(2^sat_bits)+1){
		d = -(2^sat_bits)+DBL_MIN;
	}
	return d;
}

