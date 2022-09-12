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
  Fixedpoint result = {whole, 0, 0};
  return result;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint result = {whole, frac, 0}; 
  return result;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  uint64_t tag = 0;
  uint64_t whole = 0;
  uint64_t frac = 0;

  char w_part[64];
  char f_part[64];
  char t_str[64]; 

  strcpy(t_str, hex);

  char *ptr = strtok(t_str, "."); 

  if (strcmp(hex, ptr) == 0) {//case that there is no delimiter in the string
    for (size_t i = 0; i < strlen(hex); i++) {
      if (i == 0) { 
        if (hex[i] != '-' || !(isalnum(hex[i]))) { 
          tag = 2; 
          Fixedpoint result = {whole, frac, tag}; 
          return result; 
        }
      }
      else if (!(isalnum(hex[i]))) {
        tag = 2; 
        Fixedpoint result = {whole, frac, tag}; 
        return result; 
      }
    }

    if (strlen(hex) <= 17) {
      if (hex[0] == '-') { 
        tag = 1;
        whole = hex_to_int(hex); 
      }
    }
    else if (strlen(hex) > 17) { 
      tag = 2;
      Fixedpoint result = {whole, frac, tag}; 
      return result; 
    }
  }

  else if (ptr != NULL) { // delimiter is present
    strcpy(w_part, ptr);
    
    if(strlen(w_part) > 17) { 
      tag = 2;
      Fixedpoint result = {whole, frac, tag};
      return result; 
    } 
    
    for (size_t i = 0; i < strlen(w_part); i++) { //check for invalid form 
      if (i == 0) { 
        if (w_part[i] != '-' || !(isalnum(hex[i]))) { 
          tag = 2; 
          Fixedpoint result = {whole, frac, tag}; 
          return result; 
        }
      }

      else if (!(isalnum(w_part[i]))) { 
        tag = 2; 
        Fixedpoint result = {whole, frac, tag}; 
        return result; 
      }
    }

    if (w_part[0] == '-') { 
      tag = 1; 
      whole = hex_to_int(w_part);
    }

    ptr = strtok(NULL, "."); 
    strcpy(f_part, ptr); 
    
    if (strlen(f_part) > 16) { 
      tag = 2; 
      Fixedpoint result = {whole, frac, tag};
    }
    for (size_t i = 0; i < strlen(f_part); i++) {
      if(!(isalnum(f_part[i]))) {
        tag = 2; 
        Fixedpoint result = {whole, frac, tag}; 
      }
    }
    frac = hex_to_int(f_part); 
  }
  
  Fixedpoint result = {whole, frac, tag}; 
  return result;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  return val.whole;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  return val.frac;
}

uint64_t len_counter(uint64_t value){
  uint64_t length=0;
  while(value){ l++; value/=10; }
  return l;
}

uint64_t power(uint64_t base, uint64_t exp){
  if(exp == 1){
    return base;
  } else {
    return power(base * base, exp - 1);
  }
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  // TODO: implement


  uint64_t left_len = length(left.frac);
  uint64_t right_len = length(right.frac);

  // left and right fractional parts must have same number of places
  if (right_len >= left_len) {
    left.frac = power(10 , right_len - left_len) * left.frac;
    left_len = right_len;
  } else {
    right.frac = power(10 , left_len - right_len) * right.frac;
    right_len = left_len;
  }

  //check if fractional result must be carried over to whole part if right is greater in magnitude
  Fixedpoint whole_adjust = {0, 0, 1};
  uint64_t frac_result;
  if (right.frac >= left.frac) {
    if(right.tag == -1 && left.tag == 1){
      frac_result = power(10, right_len) - right.frac + left.frac;
      Fixedpoint whole_adjust = {1, 0, -1};

    } else if(right.tag == 1 && left.tag == -1){
      frac_result = right.frac - left.frac;

    } else if(right.tag == 1 && left.tag == 1){
      frac_result = right.frac + left.frac;
      int frac_result_len = length(frac_result);
      if (frac_result_len > right_len || frac_result < right.frac){
        //check if works?
        frac_result = frac_result - pow(10, frac_result_len-1);
        Fixedpoint whole_adjust = {1, 0, 1};
      }

    } else if(right.tag == -1 && left.tag == -1){
      frac_result = right.frac + left.frac;
      int frac_result_len = length(frac_result);
      if (frac_result_len > right_len || frac_result < right.frac){
        //check if works?
        frac_result = frac_result + power(10, frac_result_len-1);
        Fixedpoint whole_adjust = {1, 0, -1};
      }
    }
  //check if fractional result must be carried over to whole part if left is greater in magnitude
  } else {
    if(right.tag == -1 && left.tag == 1){
      frac_result = power(10, left_len) - left.frac + right.frac;
      Fixedpoint whole_adjust = {1, 0, -1};

    } else if(right.tag == 1 && left.tag == -1){
      frac_result = left.frac - right.frac;

    } else if(right.tag == 1 && left.tag == 1){
      frac_result = right.frac + left.frac;
      int frac_result_len = length(frac_result);
      if (frac_result_len > right_len || frac_result < left.frac){
        //check if works?
        frac_result = frac_result - power(10, frac_result_len-1);
        Fixedpoint whole_adjust = {1, 0, 1};
      }

    } else if(right.tag == -1 && left.tag == -1){
      frac_result = right.frac + left.frac;
      int frac_result_len = length(frac_result)+1;
      if (frac_result_len > right_len || frac_result < left.frac){
        //check if works?
        frac_result = frac_result + power(10, frac_result_len-1);
        Fixedpoint whole_adjust = {1, 0, 1};
      }
    }
  }

  

  //check if whole part overflows
  int tag;
  uint64_t whole_result;
  if (right.whole > left.whole) {
    if(right.tag == -1 && left.tag == 1){
      whole_result = power(10, right_len) - right.whole + left.whole;
      tag = -1;

    } else if(right.tag == 1 && left.tag == -1){
      whole_result = right.whole - left.whole;
      tag = 1;

    } else if(right.tag == 1 && left.tag == 1){
      whole_result = right.whole + left.whole;
      //checking for positive overflow
      if (whole_result < left.whole){
        tag = 2;
      }

    } else if(right.tag == -1 && left.tag == -1){
      whole_result = right.frac + left.frac;
      //checking for negative overflow
      if (whole_result < left.whole){
        tag = -2;
      }
    }
  } else {
    if(right.tag == -1 && left.tag == 1){
      whole_result = power(10, left_len) - left.whole + right.whole;
      tag = -1;

    } else if(right.tag == 1 && left.tag == -1){
      whole_result = left.whole - right.whole;
      tag = 1;

    } else if(right.tag == 1 && left.tag == 1){
      whole_result = right.whole + left.whole;
      if (whole_result < left.whole){
        tag = 2;
      }

    } else if(right.tag == -1 && left.tag == -1){
      whole_result = right.frac + left.frac;
      if (whole_result < left.whole){
        tag = -2;
      }
    }
  }

  
  Fixedpoint result = {whole_result, frac_result, tag};
  if (whole_adjust.whole!=0){
    return fixedpoint_add(result, whole_adjust);
  }
  return result;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  right.tag = right.tag * (-1);
  return fixedpoint_add(left, right);
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  if(fixedpoint_is_zero(val)) { 
    return val; 
  }
  else { 
    if (val.tag == 0) {
      val.tag = 1; 
    }
    if (val.tag == 1) { 
      val.tag = 0;
    }
  }
  return val;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  uint64_t w_part = val.whole << 1;
  uint64_t f_part = val.frac << 1;

  if (w_part < val.whole) { 
    val.tag = 2; 
  }
  if (f_part < val.frac) {
    w_part++;
  }
  Fixedpoint result = {w_part, f_part, val.tag};
  
  return result;
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_zero(Fixedpoint val) {
  if(val.whole == 0 && val.frac == 0 && val.tag){
    return 1;
  } 
  else {
    return 0;
  }
}

int fixedpoint_is_err(Fixedpoint val) {
  if (val.tag == 2) { 
    return 1; 
  }
  return 0;
}

int fixedpoint_is_neg(Fixedpoint val) {
  if (val.tag == 1) { 
      return 1; 
  }
  return 0;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  if (val.tag == 6) { 
      return 1; 
  }
  return 0;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  if (val.tag == 5) {
      return 1; 
  }
  return 0;
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  if (val.tag == 4) { 
      return 1; 
  }
  return 0;
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
  if (val.tag == 3) { 
      return 1; 
  }
  return 0;
}

int fixedpoint_is_valid(Fixedpoint val) {
  if (val.tag == 0 || val.tag == 1) { 
    return 1; 
  }
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

    if (hex[i] == '-') { 
      continue; 
    }

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