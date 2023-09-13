#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result = {0,0,0,0,0,0,0,0}; 
  result.data[0] = val;
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result = {0,0,0,0,0,0,0,0}; 
  for (int i = 0; i < 8; i++) {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
UInt256 result = {0,0,0,0,0,0,0,0};

int len = strlen(hex);

int iters = 8;


if (len % 8 != 0) {
  if ((len / 8 + 1) < iters) {
    iters = len / 8 + 1;
  }
} else {
  if (len / 8 < iters)
    iters = len / 8;
}

for (int i = 1; i <= iters; i++) {
  char buffer[8];
  for (int j = 7; j >= 0; j--){
    buffer[j] = (len - i * 8) + j >= 0 ? hex[(len - i * 8) + j] : '0';
  }
  result.data[i - 1] = (uint32_t)strtoul(buffer, NULL, 16);
}

return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = malloc(sizeof(char) * 65);
    int index = 0;
    int non_zero_found = 0;

    for (int i = 7; i >= 0; --i) {
        for (int j = 7; j >= 0; --j) {
            char hex_val = '\0';
            uint32_t buff = (val.data[i] >> (j * 4)) & 0xF;

            if (buff < 10) {
                hex_val = '0' + buff;
            } else {
                hex_val = 'a' + buff - 10;
            }

            if (hex_val == '0' && !non_zero_found) {
                continue;
            } else {
                hex[index++] = hex_val;
                non_zero_found = 1;
            }
        }
    }

    if (index == 0) {
        hex[0] = '0';
        hex[1] = '\0';
    } else {
        hex[index] = '\0';
    }
    return hex;
}


// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 3 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  uint32_t carry_over = 0; 
  for (int i = 0; i < 8; ++i) {
      uint64_t temp_sum = (uint64_t)left.data[i] + right.data[i] + carry_over;
      sum.data[i] = (uint32_t)temp_sum; 
      carry_over = (uint32_t)(temp_sum >> 32); 
  }
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  UInt256 neg_right = uint256_negate(right);
  result = uint256_add(left, neg_right);
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  UInt256 one;
  for (int i = 0; i < 8; ++i) {
    result.data[i] = ~val.data[i];
    one.data[i] = 0;
  }
  one.data[0] = 1;
  result = uint256_add(result, one);
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  nbits = nbits % 256;
  for (int i = 0; i < 8; ++i) {
    result.data[i] = (val.data[i] << nbits) | (val.data[7 - i] >> (32 - nbits));
  }
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  nbits = nbits % 256;

  size_t full_rotations = nbits/32;
  for (int i = 0; i < 8; ++i) {
    result.data[i] = (val.data[i] >> nbits) | (val.data[7 - i] << (32 - nbits));
  }
  return result;
}
