#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <inttypes.h>
#include "fixedpoint.h"

// You can remove this once all of the functions are fully implemented
static Fixedpoint DUMMY;

Fixedpoint fixedpoint_create(uint64_t whole) {
  Fixedpoint result = {whole, 0, 1};
  return result;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint result = {whole, frac, 1}; 
  return result;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  uint64_t tag = 1;
  uint64_t whole = 0;
  uint64_t frac = 0;

  char w_part[64];
  char f_part[64];
  char t_str[64]; 

  strcpy(t_str, hex);

  char *ptr = strtok(t_str, ".");
  if (ptr == NULL) {
    if (strlen(hex) <= 17) {
      if (hex[0] == '-') { 
        tag = 0;
        whole = hex_to_int(hex); 
      }
    }
  }
  else if (ptr != NULL) { 
    //check for negative and stuff
    strcpy(w_part, ptr);
    whole = hex_to_int(w_part); 
    ptr = strtok(NULL, "."); 
    strcpy(f_part, ptr); 
    frac = hex_to_int(f_part); 
  }
  
  Fixedpoint result = {whole, frac, 1}; 
  assert(0);
  return result;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  // TODO: implement
  return val.whole;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  // TODO: implement
  return val.frac;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_zero(Fixedpoint val) {
  // TODO: implement
  if(val.whole == 0 && val.frac == 0 && val.tag){
    return 1;
    } else {
      return 0;
    }
}

int fixedpoint_is_err(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_neg(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_valid(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return 0;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  // TODO: implement
  assert(0);
  char *s = malloc(20);
  strcpy(s, "<invalid>");
  return s;
}

uint64_t hex_to_int(const char *hex) {
  uint64_t decimal = 0, base = 1; 
  
  int length = strlen(hex);

  for (int i = length--; i >= 0; i--) {

    if (hex[i] >= '0' && hex[i] <= '9') { 
      decimal += (hex[i] - 48) * base; 
      base *= 16; 
    }
    else if (hex[i] >= 'A' && hex[i] <= 'F') { 
      decimal += (hex[i] - 55) * base; 
      base *= 16; 
    }
    else if (hex[i] >= 'a' && hex[i] <= 'f') { 
      decimal += (hex[i] - 87) * base;
      base *= 16; 
    }
  }

  return decimal;

}