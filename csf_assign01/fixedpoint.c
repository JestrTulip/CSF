#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
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

int valid_hex(char c) { 
  if ((c < '0' || c > '9') && (c < 'A' || c > 'F')) { 
    return 1; 
  }
  return 0; 
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
    if (strlen(ptr) > 16 && ptr[0] != '-') { 
      tag = 0;
      Fixedpoint result = {whole, frac, tag}; 
      return result; 
    }

    for (size_t i = 0; i < strlen(hex); i++) {
      if (i == 0) { 
        if (!(isxdigit(hex[i]))) { 
          if (w_part[i] != '-') { 
            tag = 0; 
            Fixedpoint result = {whole, frac, tag}; 
            return result; 
          }
          
        }
      }
      else if (!(isxdigit(hex[i]))) {
        tag = 0; 
        Fixedpoint result = {whole, frac, tag}; 
        return result; 
      }
    }

    
    if (hex[0] == '-') { 
      tag = -1;
      whole = hex_to_int(hex); 
    }
    
  }

  else if (ptr != NULL) { // delimiter is present
    strcpy(w_part, ptr);
    
    if(strlen(w_part) > 16 && w_part[0] != '-') { 
      tag = 0;
      Fixedpoint result = {whole, frac, tag};
      return result; 
    } 
    
    for (size_t i = 0; i < strlen(w_part); i++) { //check for invalid form 
      if (i == 0) { 
        if (!(isxdigit(hex[i]))) { 
          if (w_part[i] != '-') { 
            tag = 0; 
            Fixedpoint result = {whole, frac, tag}; 
            return result;
          }
        }
      }

      else if (!(isxdigit(w_part[i]))) { 
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
        if(!(isxdigit(fin_frac[i]))) {
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
      if(!(isxdigit(f_part[i]))) {
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
 
  if (left.tag == right.tag) { 
    uint64_t w_sum = left.whole + right.whole; 
    uint64_t f_sum = left.frac + right.frac; 
    int fin_tag = left.tag;

<<<<<<< HEAD
  printf("reached1");
  uint64_t left_len = len_counter(left.frac);
  printf("reached");
  uint64_t right_len = len_counter(right.frac);

  // left and right fractional parts must have same number of places
  if (right_len >= left_len) {
    left.frac = power(10 , right_len - left_len) * left.frac;
    left_len = right_len;
  } else {
    right.frac = power(10 , left_len - right_len) * right.frac;
    right_len = left_len;
  }
  printf("reached");
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
      int frac_result_len = len_counter(frac_result);
      if (frac_result_len > right_len || frac_result < right.frac){
        //check if works?
        frac_result = frac_result - power(10, frac_result_len-1);
        Fixedpoint whole_adjust = {1, 0, 1};
=======
    if (f_sum < left.frac || f_sum < right.frac) { 
      w_sum++; 
    }
    
    else if ((w_sum < left.whole) || (w_sum < right.whole)) { 
      if (left.tag == 1) { 
        fin_tag = 2; 
>>>>>>> ec4038d36505b878decbe9edf5a4335b7e21779f
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

  
    if (right.frac > left.frac && left.whole != right.whole) { 
      w_sum--; 
      f_sum++; 
      result.frac = f_sum; 
      result.whole = w_sum; 
    }
    return result; 
  }
  else { 
    int fin_tag = right.tag; 
    uint64_t w_res = right.whole - left.whole; 
    uint64_t f_res = right.frac - left.frac;

    Fixedpoint result = {w_res, f_res, fin_tag}; 

    if (left.frac > right.frac && left.whole != right.whole) { 
      w_res--;
      f_res++; 
      result.frac = f_res; 
      result.whole = w_res; 
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
  // TODO: implement
  uint64_t frac_len = (val.frac==0) ? 1 : (uint64_t)log10(val.frac)+1;
  uint64_t frac_adjust = 0;
  uint64_t whole_result;
  uint64_t frac_result;
  
  //carry over .5 to fractional part if whole part is odd
  if(val.whole % 2 != 0){
    frac_adjust = 5 * pow(10, frac_len-1);
  } 
  whole_result = val.whole / 2;

  if(val.frac % 2 != 0){
    frac_result = (val.frac / 2) * 10 + 5;
    if (frac_result < val.frac){
      //overflow has occured not sure how to signal it
    }
  } else {
    frac_result = val.frac / 2;
  }

  Fixedpoint temp = {0, frac_adjust, val.tag};
  Fixedpoint result = {whole_result, frac_result, val.tag};
  return fixedpoint_add(result, temp);
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
  if(left.tag == 1 && right.tag == 1){
    Fixedpoint temp = fixedpoint_sub(left, right);
    return (fixedpoint_is_zero(temp)) ? 0 : temp.tag;
  } else if(left.tag == -1 && right.tag == -1){
    Fixedpoint temp = fixedpoint_sub(left, right);
    return (fixedpoint_is_zero(temp)) ? 0 : temp.tag * -1;
  } else if(left.tag == -1 && right.tag == 1){
    return -1;
  } else if(left.tag == 1 && right.tag == -1){
    return 1;
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