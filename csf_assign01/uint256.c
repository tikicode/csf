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
  int i = 0;
  int k = 0;
  while (*hex) {
    if (i < 8) {
      if (k < 8) {
        uint32_t j = *hex++;
        if (j >= '0' && j <= '9') {
          j = j - '0';
        } else if (j >= 'A' && j <= 'F') {
          j = j - 'A' + 10;
        } else if (j >= 'a' && j <= 'f') {
          j = j - 'a' + 10;
        }
        result.data[i] = (result.data[i] << 4) | j;
        k++;
      }
      if (k == 8) {
        i++;
        k = 0;
      }
      if (i == 8) {
        break;
      }
    }
  }
  return result;
}

void removeLeadingZeros(char *str) {
  int len = strlen(str);
    int nonZeroIndex = -1; // Index of the first non-zero character
    
    // Find the first non-zero character
    for (int i = 0; i < len; i++) {
        if (str[i] != '0') {
            nonZeroIndex = i;
            break;
        }
    }

    if (nonZeroIndex >= 0) {
        // Shift the string to remove leading zeros
        for (int i = 0; i < len - nonZeroIndex; i++) {
            str[i] = str[i + nonZeroIndex];
        }
        str[len - nonZeroIndex] = '\0'; // Null-terminate the string
    } else {
        // If the string contains only zeros, leave one zero
        str[0] = '0';
        str[1] = '\0';
    }
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
 char *hex = (char *)malloc(64 * sizeof(char) + 1);
    if (hex == NULL) {
        // Handle memory allocation error
        return NULL;
    }
    hex[64] = '\0';  // Null-terminate the string

    // Start with an empty string
    hex[0] = '\0';

    int isZero = 1; // Flag to check if all segments are zero

    for (int i = 0; i < 8; i++) {
        // Use sprintf to format and concatenate the hexadecimal values
        char temp[9];
        sprintf(temp, "%08X", val.data[i]);
        
        // Remove leading zeros from temp
        removeLeadingZeros(temp);

        if (isZero && temp[0] != '0') {
            isZero = 0; // Mark as non-zero when a non-zero segment is found
        }

        // Append the segment to the hex string (skip leading zeros if all segments are not zero)
        strcat(hex, temp);
    }

    printf(" %s ", hex);
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
  // TODO: implement
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}
