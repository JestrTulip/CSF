#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include "fixedpoint.h"

// You can remove this once all of the functions are fully implemented

Fixedpoint fixedpoint_create(uint64_t whole) {
  Fixedpoint result = {whole, 0, 1};
  return result;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  Fixedpoint result = {whole, frac, 1}; 
  return result;
}

int valid_hex(char c) { 
  if ((c < '0' || c > '9') && (c < 'A' || c > 'F')) { 
    return 1; 
  }
  return 0; 
}

void add_padding(char* str) { 
  size_t len = strlen(str);

  for (size_t i = len; i < 16; i++) {
    str[i] = '0'; 
  }
  str[17] = '\0';

}

int string_is_valid_hex(char* str) {
  size_t len = strlen(str);

  for (size_t i = 0; i < len; i++) {
    if (!(isxdigit(str[i]))) {
      return 0; 
    }
  }
  return 1; 
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  int tag = 1;
  uint64_t whole = 0;
  uint64_t frac = 0;

  char w_part[17] = {0};
  char f_part[17] = {0};

  if(hex[0] == '-'){
    tag = -1;
    hex++;
  }

  if(strlen(hex)>33){
    Fixedpoint result = {0,0,0};
    return result;
  }

  int passed_point = 0;
  int w_inc = 0;
  int f_inc = 0;

  for(size_t i = 0; i < strlen(hex); ++i){
    if (hex[i] == '.') { 
      passed_point++; 
      i++; 
      w_inc--;
    }
    if (hex[i] == '\0') { 
      break; 
    }

    if((hex[i] != '.' && !isxdigit(hex[i])) || passed_point > 1 ||w_inc > 15 || f_inc > 15){
      Fixedpoint result = {0,0,0};
      return result;
    }

    if(!passed_point){
      w_part[w_inc] = hex[i];
      w_inc++;
    } else {
      f_part[f_inc] = hex[i];
      f_inc++;
    }
  }

  add_padding(f_part);

  whole = strtoul(w_part, NULL, 16);
  frac = strtoul(f_part, NULL, 16); 

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
 
  if (left.tag == right.tag) { 
    uint64_t w_sum = left.whole + right.whole; 
    uint64_t f_sum = left.frac + right.frac; 
    int fin_tag = left.tag;

    if (f_sum < left.frac) { 
      w_sum++; 
      if (w_sum == left.whole && w_sum == right.whole) {
        if (left.tag == 1) { 
        fin_tag = 2; 
        }
        else if (left.tag == -1 ){ 
        fin_tag = -2; 
        }
      }
    }
    
    if ((w_sum < left.whole) || (w_sum < right.whole)) { 
      if (left.tag == 1) { 
        fin_tag = 2; 
      }
      else if (left.tag == -1 ){ 
        fin_tag = -2; 
      }
    }
    Fixedpoint result = {w_sum, f_sum, fin_tag};

    return result;  
  }
  else if (left.whole > right.whole || (left.whole == right.whole && left.frac > right.frac)){ //left bigger
    int fin_tag = left.tag; 
    uint64_t w_sum = left.whole - right.whole; 
    uint64_t f_sum = left.frac - right.frac; 

    Fixedpoint result = {w_sum, f_sum, fin_tag}; 

    if(fixedpoint_is_zero(result)) { 
      result.tag = 1; 
    }
  
    if (right.frac > left.frac && left.whole != right.whole) { 
      w_sum--; 
      //f_sum++; 
      //result.frac = f_sum; 
      result.whole = w_sum; 
    }

    if (w_sum > left.whole) { 
      if (left.tag == 1) { 
        fin_tag = 2; 
      }
      else if (left.tag == -1 ){ 
        fin_tag = -2; 
      }
    }
    return result; 
  }
  else { 
    int fin_tag = right.tag; 
    uint64_t w_res = right.whole - left.whole; 
    uint64_t f_res = right.frac - left.frac;

    Fixedpoint result = {w_res, f_res, fin_tag};

    if(fixedpoint_is_zero(result)) {
      result.tag = 1; 
    }

    if (left.frac > right.frac && left.whole != right.whole) { 
      w_res--;
      //f_res++; 
      //result.frac = f_res; 
      result.whole = w_res; 
    }

    if (w_res > right.whole) { 
      if (left.tag == 1) { 
        fin_tag = 2; 
      }
      else if (left.tag == -1 ){ 
        fin_tag = -2; 
      }
    }

    return result; 
  }

  Fixedpoint result = {0,0,1}; 
  return result;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  return fixedpoint_add(left, fixedpoint_negate(right));
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
  uint64_t w_part = val.whole >> 1;
  uint64_t f_part = val.frac >> 1;

  if (val.whole % 2) { 
    f_part += 0x8000000000000000UL; 
  }

  if (val.frac % 2) {
     if (val.tag == 1) { 
      val.tag = 3; 
    } 
    else if (val.tag == -1) { 
      val.tag = -3; 
    }
  }

  Fixedpoint result = {w_part, f_part, val.tag}; 
  return result; 
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
  if (left.tag == 1 && right.tag == -1) { 
    return 1; 
  }
  else if(left.tag == -1 && right.tag == 1) {
    return -1; 
  }

  if (left.whole == right.whole) {
    if (left.frac > right.frac) { 
      if (left.tag == 1) {
        return 1; 
      } else { 
        return -1; 
      }
    }
    else if (left.frac < right.frac) {
      if (left.tag == 1) {
        return -1; 
      } else { 
        return 1; 
      }
    } else { return 0; }
  } else if (left.whole > right.whole) {
    if (left.tag == 1) { return 1; }
    else { return -1; }
  } else { 
    if (left.tag == 1) { 
      return -1;
    }
    else { return 1; }
  }
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

