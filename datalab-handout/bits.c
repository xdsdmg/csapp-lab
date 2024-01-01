/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y)
{
  return ~(x & y) & ~(~x & ~y);
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void)
{
  int tmin = 1 << 31;
  return tmin;
}
// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x)
{
  int a = x + 1;
  return !(a ^ (~a + 1)) & !!(a ^ 0);
}
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x)
{
  int e = 0xAA;
  int a = e;
  a = (a << 8) + e;
  a = (a << 8) + e;
  a = (a << 8) + e;

  return !((x & a) ^ a);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x)
{
  return ~x + 1;
}
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x)
{
  int flag_0 = !(x >> 31);
  int flag_1 = !((x + (~0x30 + 1)) >> 31);
  int flag_2 = (x + (~0x3A + 1)) >> 31;

  return flag_0 & flag_1 & flag_2;
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z)
{
  int a = !!x;
  int b = !x;

  int m = ~a + 1;
  int n = ~b + 1;

  return (y & m) + (z & n);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y)
{
  int z = x + (~y + 1);
  int x_31 = (x >> 31) & 1;
  int y_31 = (y >> 31) & 1;
  int z_31 = (z >> 31) & 1;
  int flag = x_31 ^ y_31;

  int flag_0 = (!!z_31 | !(z ^ 0)) & !flag;
  int flag_1 = flag & x_31;
  int flag_2 = flag & !y_31;

  return flag_0 | flag_1 | flag_2;
}
// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x)
{
  int is_negative = x >> 31;
  int is_positive = (~x + 1) >> 31;

  return ~(is_negative | is_positive) & 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x)
{
  int shadow = 0;
  int cond_0 = 0;
  int cond_1 = 0;
  int offset = 0;
  int offset_32 = 32;
  int offset_16 = 16;
  int offset_08 = 8;
  int offset_04 = 4;
  int offset_02 = 2;
  int offset_01 = 1;
  int index = 0;
  int x_tmp = 0;
  int flag = 0;

  shadow = (x & (1 << 31)) >> 31;
  cond_0 = ~x & shadow;
  cond_1 = x & ~shadow;
  x = cond_0 + cond_1;

  x_tmp = x >> offset_16;
  flag = !(x_tmp);
  shadow = (flag << 31) >> 31;
  cond_0 = offset_16 & shadow;
  cond_1 = offset_32 & ~shadow;
  index = cond_0 + cond_1;

  offset = index + (~offset_08 + 1);
  x_tmp = x >> offset;
  flag = !(x_tmp);
  shadow = (flag << 31) >> 31;
  cond_0 = offset & shadow;
  cond_1 = index & ~shadow;
  index = cond_0 + cond_1;

  offset = index + (~offset_04 + 1);
  x_tmp = x >> offset;
  flag = !(x_tmp);
  shadow = (flag << 31) >> 31;
  cond_0 = offset & shadow;
  cond_1 = index & ~shadow;
  index = cond_0 + cond_1;

  offset = index + (~offset_02 + 1);
  x_tmp = x >> offset;
  flag = !(x_tmp);
  shadow = (flag << 31) >> 31;
  cond_0 = offset & shadow;
  cond_1 = index & ~shadow;
  index = cond_0 + cond_1;

  offset = index + (~offset_01 + 1);
  x_tmp = x >> offset;
  flag = !(x_tmp);
  shadow = (flag << 31) >> 31;
  cond_0 = offset & shadow;
  cond_1 = index & ~shadow;
  index = cond_0 + cond_1;

  return index + !!(x ^ 0);
}
// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf)
{
  // 规格化：e - (2^(k - 1) - 1)   1 - (2^(k - 1) - 1) = -126
  // 非规格化：-126 0.75

  int m_shadow = (1 << 23) - 1;
  unsigned uf_22 = (uf >> 22) & 1;
  unsigned uf_31 = uf >> 31;

  unsigned s = uf_31 << 31;
  unsigned exp = (uf >> 23) & 0xFF;
  unsigned m = uf & m_shadow;

  if (exp == 0xFF)
    return uf;
  else if (exp == 0)
  {
    if (uf_22 == 1 && m != 0)
      exp = exp + 1;
    m = m << 1;
  }
  else
  {
    exp = exp + 1;
  }

  unsigned new_exp = (exp & 0xFF) << 23;
  unsigned new_m = m & m_shadow;
  unsigned ret = s | new_exp | new_m;

  return ret;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf)
{
  const int m_shadow = 8388607; // (1 << 23) - 1;
  const int m_len = 23;
  const int bias = 127;
  const unsigned unvalid = 0x80000000u;

  int exp = (uf >> m_len) & 0xFF;
  unsigned m = uf & m_shadow;

  int exp_ = exp - bias;
  if (exp_ > 30)
    return unvalid;
  if (exp_ < 0)
    return 0;
  if (exp_ == 30 && m)
    return unvalid;

  int gap = exp_ - m_len;
  int offset_0 = exp_ > m_len ? m_len : exp_;
  int shadow = (1 << offset_0) - 1;
  m = m & (shadow << (m_len - offset_0));
  m = m << gap;

  int abs = (1 << exp_) | m;
  return (uf >> 31) ? (~abs + 1) : abs;
}
/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x)
{
  // min 2^(-126) * 2^(-23)
  // max 2^(127)

  const unsigned INF = 0X7F800000;
  const int bias = 127;
  const int e_min = -126;
  int e = 0;
  unsigned m = 0;

  if (x < -149)
    return 0;
  if (x >= 128)
    return INF;

  if (x <= 127 && x >= -126)
    e = x + bias;

  if (x < -126 && x >= -149)
    m = 1 << (23 - (e_min - x));

  return e << 23 | m;
}
