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
  int tag = 1;
  uint64_t whole = 0;
  uint64_t frac = 0;

  char w_part[64];
  char f_part[64] = "";
  char t_str[64]; 

  strcpy(t_str, hex);

  char *ptr = strtok(t_str, "."); 

  if (strcmp(hex, ptr) == 0) {//case that there is no delimiter in the string
    for (size_t i = 0; i < strlen(hex); i++) {
      if (i == 0) { 
        if (!(isalnum(hex[i]))) { 
          if (w_part[i] != '-') { 
            tag = 0; 
            Fixedpoint result = {whole, frac, tag}; 
            return result; 
          }
          
        }
      }
      else if (!(isalnum(hex[i]))) {
        tag = 0; 
        Fixedpoint result = {whole, frac, tag}; 
        return result; 
      }
    }

    if (strlen(hex) <= 17) {
      if (hex[0] == '-') { 
        tag = -1;
        whole = hex_to_int(hex); 
      }
    }
    else if (strlen(hex) > 17) { 
      tag = 0;
      Fixedpoint result = {whole, frac, tag}; 
      return result; 
    }
  }

  else if (ptr != NULL) { // delimiter is present
    strcpy(w_part, ptr);
    
    if(strlen(w_part) > 17) { 
      tag = 0;
      Fixedpoint result = {whole, frac, tag};
      return result; 
    } 
    
    for (size_t i = 0; i < strlen(w_part); i++) { //check for invalid form 
      if (i == 0) { 
        if (!(isalnum(hex[i]))) { 
          if (w_part[i] != '-') { 
            tag = 0; 
            Fixedpoint result = {whole, frac, tag}; 
            return result;
          }
        }
      }

      else if (!(isalnum(w_part[i]))) { 
        tag = 0; 
        Fixedpoint result = {whole, frac, tag}; 
        return result; 
      }
    }

    if (w_part[0] == '-') { 
      tag = -1; 
      whole = hex_to_int(w_part);
    }

    whole = hex_to_int(w_part);

    ptr = strtok(NULL, "."); 
    strcpy(f_part, ptr); 
    
    if (strlen(f_part) > 16) { 
      tag = 0; 
      Fixedpoint result = {whole, frac, tag};
      return result; 
    }

    else if (strlen(f_part) < 16) { 
      char hold[64] = ""; 

      for (size_t i = 0; i < 16 - strlen(f_part); i++) {
        hold[i] = '0'; 
      }

      char *fin_frac = malloc(17); 

      strcpy(fin_frac, f_part); 
      strcat(fin_frac, hold); 
      fin_frac[17] = '\0'; 

      for (size_t i = 0; i < strlen(fin_frac); i++) {
        if(!(isalnum(fin_frac[i]))) {
          tag = 0; 
          Fixedpoint result = {whole, frac, tag}; 
          return result; 
        }
      }
      frac = hex_to_int(fin_frac); 
      free(fin_frac);
      Fixedpoint result = {whole, frac, tag}; 
      return result;
    }

    for (size_t i = 0; i < strlen(f_part); i++) {
      if(!(isalnum(f_part[i]))) {
        tag = 0; 
        Fixedpoint result = {whole, frac, tag}; 
        return result; 
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
  if(fixedpoint_is_zero(val)) { 
    return val; 
  }
  else if (val.tag == 1) { 
    val.tag = -1;  
  }
  else if (val.tag == -1) { 
    val.tag = 1; 
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
    if (val.tag == 1) { 
      val.tag = 2; 
    } 
    else if (val.tag == -1) { 
      val.tag = -2; 
    }
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
  if(val.whole == 0 && val.frac == 0){
    return 1;
  } 
  else {
    return 0;
  }
}

int fixedpoint_is_err(Fixedpoint val) {
  if (val.tag == 0) { 
    return 1; 
  }
  return 0;
}

int fixedpoint_is_neg(Fixedpoint val) {
  if (val.tag == -1) { 
      return 1; 
  }
  return 0;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  if (val.tag == -2) { 
      return 1; 
  }
  return 0;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  if (val.tag == 2) {
      return 1; 
  }
  return 0;
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  if (val.tag == -3) { 
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
  if (val.tag == -1 || val.tag == 1) { 
    return 1; 
  }
  return 0;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {

  char *s = malloc(35);

  size_t hold = 0;

  if (fixedpoint_is_neg(val)) { 
    s[0] = '-';

    
    if (val.frac == 0) { 
      sprintf(s + 1, "%lx", val.whole); 
    }
    else { 
      sprintf(s + 1, "%lx.%016lx", val.whole, val.frac); 
      int len = strlen(s); 
      for (size_t i = len - 1; i > 0; i--) { 
        if (s[i] != '0') { 
          hold = i; 
          break;   
        }
      }
      s[hold+1] = '\0'; 
    }

    return s; 
  }


  if (val.frac == 0) { 
    sprintf(s, "%lx", val.whole); 
  }
  else { 
    sprintf(s, "%lx.%016lx", val.whole, val.frac); 
    int len = strlen(s); 
    for (size_t i = len - 1; i > 0; i--) { 
      if (s[i] != '0') { 
        hold = i; 
        break;
      }
    }
    s[hold+1] = '\0'; 
  }


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