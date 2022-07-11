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

INTEGER CODING RULES :

Replace the "return" statement in each function with one
or more lines of C code that implements the function.Your code
must conform to the following style :

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

Each "Expr" is an expression using ONLY the following :
1. Integer constants 0 through 255 (0xFF), inclusive.You are
not allowed to use big constants such as 0xffffffff.
2. Function arguments and local variables(no global variables).
3. Unary integer operations !~
4. Binary integer operations & ^| +<< >>

Some of the problems restrict the set of allowed operators even further.
Each "Expr" may consist of multiple operators.You are not restricted to
one operator per line.

You are expressly forbidden to :
1. Use any control constructs such as if, do, while, for, switch, etc.
2. Define or use any macros.
3. Define any additional functions in this file.
4. Call any functions.
5. Use any other operations, such as&&, || , -, or ? :
    6. Use any form of casting.
    7. Use any data type other than int.This implies that you
    cannot use arrays, structs, or unions.


    You may assume that your machine :
1. Uses 2s complement, 32 - bit representations of integers.
2. Performs right shifts arithmetically.
3. Has unpredictable behavior when shifting an integer by more
than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE :
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

For the problems that require you to implent floating - point operations,
the coding rules are less strict.You are allowed to use loopingand
conditional control.You are allowed to use both intsand unsigneds.
You can use arbitrary integerand unsigned constants.

You are expressly forbidden to :
1. Define or use any macros.
2. Define any additional functions in this file.
3. Call any functions.
4. Use any form of casting.
5. Use any data type other than int or unsigned.This means that you
cannot use arrays, structs, or unions.
6. Use any floating point data types, operations, or constants.


NOTES:
1. Use the dlc(data lab checker) compiler(described in the handout) to
check the legality of your solutions.
2. Each function has a maximum number of operators(!~&^| +<< >>)
that you are allowed to use for your implementation of the function.
The max operator count is checked by dlc.Note that '=' is not
counted; you may use as many of these as you want without penalty.
3. Use the btest test harness to check your functions for correctness.
4. Use the BDD checker to formally verify your functions
5. The maximum number of ops for each function is given in the
header comment for each function.If there are any inconsistencies
between the maximum ops in the writeupand in this file, consider
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
 /* Copyright (C) 1991-2018 Free Software Foundation, Inc.
    This file is part of the GNU C Library.
    The GNU C Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    The GNU C Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public
    License along with the GNU C Library; if not, see
    <http://www.gnu.org/licenses/>.  */
    /* This header is separate from features.h so that the compiler can
       include it implicitly at the start of every compilation.  It must
       not itself include <features.h> or any other header that includes
       <features.h> because the implicit include comes before any feature
       test macros that may be defined in a source file before it first
       explicitly includes a system header.  GCC knows the name of this
       header in order to preinclude it.  */
       /* glibc's intent is to support the IEC 559 math functionality, real
          and complex.  If the GCC (4.9 and later) predefined macros
          specifying compiler intent are available, use them to determine
          whether the overall intent is to support these features; otherwise,
          presume an older compiler has intent to support these features and
          define these macros by default.  */
          /* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
             synchronized with ISO/IEC 10646:2017, fifth edition, plus
             the following additions from Amendment 1 to the fifth edition:
             - 56 emoji characters
             - 285 hentaigana
             - 3 additional Zanabazar Square characters */
             /* We do not support C11 <threads.h>.  */
             /*
              * bitNot - ~x without using ~
              *   Example: bitNot(0) = 0xffffffff
              *   Legal ops: ! & ^ | + << >>
              *   Max ops: 12
              *   Rating: 1
              */
    int bitNot(int x) {
    int mask = 0xff;
    mask = (mask << 8) + mask;
    mask = (mask << 16) + mask;
    //得到0xffffffff
    return x ^ mask;
}
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
    int m1 = (~x) & y;
    int m2 = (x & (~y));
    return(~((~m1) & (~m2)));
}
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
    int xx,mask = 0xAA;//10101010
    mask = mask | (mask << 8);
    mask = mask | (mask << 16);
    //mask是0xaaaaaaaa
    xx = x & mask;//把x的偶数位置为0
    return !(xx ^ mask);
}
/*
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3
 */
int rotateRight(int x, int n) {

    int first,last = x >> n;
    int mask1 = (~(1 << 31) >> (31 + ~n + 1));//00...011..1 n个1
    int mask2 = ~(((1 << 31) >> n) << 1);//00...011...1 n个0
    //cout << hex << mask1 << endl;
    //cout << hex << mask2 << endl;
    last = last & mask2;
    first = x & mask1;
    first = first << (32 + ~n + 1);
    return first | last;
}
/*
 * palindrome - return 1 if x is palindrome 回文in binary form,
 *   return 0 otherwise
 *   A number is palindrome if it is the same when reversed
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   Example: palindrome(0xff0000ff) = 1,
 *            palindrome(0xff00ff00) = 0
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 40
 *   Rating: 4
 */
int palindrome(int x) {
    int y = ((x >> 16) & 0xffff) | (x << 16);
    y = ((((y & 0xff00ff00) >> 8) & 0xffffff) | ((y & 0x00ff00ff) << 8));
    y = ((((y & 0xf0f0f0f0) >> 4) & 0xfffffff) | ((y & 0x0f0f0f0f) << 4));
    y = (((y & 0xcccccccc) >> 2) & 0x3fffffff) | ((y & 0x33333333) << 2);
    y = ((((y & 0xaaaaaaaa) >> 1) & 0x7fffffff) | ((y & 0x55555555) << 1));
    return !(x ^ y);
}
/*
 * countConsecutive1 - return the number of consecutive 1‘s
 *   in the binary form of x
 *   Examples: countConsecutive1(0xff00ff00) = 2,
 *             countConsecutive1(0xf070f070) = 4,
 *             countConsecutive1(0b00101010001111110101110101110101) = 10
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Rating: 4
 */
int countConsecutive1(int x) {
    int add;
    int mask1, mask2, mask3, mask4, mask5,com;
    mask1 = 0x55;
    mask1 = mask1 + (mask1 << 8);
    mask1 = mask1 + (mask1 << 16);
    mask2 = 0x33;
    mask2 = mask2 + (mask2 << 8);
    mask2 = mask2 + (mask2 << 16);
    mask3 = 0xf;
    mask3 = mask3 + (mask3 << 8);
    mask3 = mask3 + (mask3 << 16);
    mask4 = 0xff;
    mask4 = mask4 + (mask4 << 16);
    mask5 = 0xff + (0xff << 8);
    com = x ^ (x << 1);
    com = com + ~((com >> 1) & mask1)+1;
    com = (com & mask2) + ((com >> 2) & mask2);
    com = (com & mask3) + ((com >> 4) & mask3);
    com = (com & mask4) + ((com >> 8) & mask4);
    com = (com + (com >> 16)) & mask5;
    add = !!((com >> 1) ^ ((com + 1) >> 1));
    return (com + add) >> 1;
}
/*
 * counter1To5 - return 1 + x if x < 5, return 1 otherwise, we ensure that 1<=x<=5
 *   Examples counter1To5(2) = 3,  counter1To5(5) = 1
 *   Legal ops: ~ & ! | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int counter1To5(int x) {
    int equal5 = (x & 1) & (!((x >> 1) & 1)) & ((x >> 2) & 1);
    return ((x + 1) + equal5 + equal5 + equal5) & 0x7;
    //若x为5,则x+1为110，加上3*equal5为1001
}
/*
 * fullSub - 4-bits sub using bit-wise operations only.
 *   (0 <= x, y < 16)
 *   Example: fullSub(12, 7) = 0x5,
 *            fullSub(7, 8) = 0xf,
 *   Legal ops: ~ | ^ & << >>
 *   Max ops: 35
 *   Rating: 2
 */
int fullSub(int x, int y) {
    //return (x - y) & 0xf;
    int temp1, temp2, temp;
    y = ~y;//-y
    temp1 = (y ^ 1);
    temp2 = (y & 1) << 1;//-y+1(y为参数的值)

    temp = temp1 ^ temp2;
    temp2 = (temp1 & temp2) << 1;
    temp1 = temp;

    temp = temp1 ^ temp2;
    temp2 = (temp1 & temp2) << 1;
    y = temp ^ temp2;

    temp1 = (y ^ x);
    temp2 = (y & x) << 1;//-y+1(y为参数的值)

    temp = temp1 ^ temp2;
    temp2 = (temp1 & temp2) << 1;
    temp1 = temp;

    temp = temp1 ^ temp2;
    temp2 = (temp1 & temp2) << 1;
    y = temp ^ temp2;
    return y & 0xf;
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int ybigger = !((y + ~x + 1) >> 31);
    int px = !(x >> 31);
    int py = !(y >> 31);
    return (ybigger & (!(px & (!py)))) | !(x ^ y) | (py & !px);
    //减去之后应该是正的，但不应该是y负-x正导致的负溢出
    //x y相等
    //避免y正-x负导致正溢出，px为负py为正的情况单独考虑
}
/*
 * sm2tc - Convert from sign-magnitude to two's complement
 *   where the MSB is the sign bit
 *   Example: sm2tc(0x80000005) = -5.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int sm2tc(int x) {
    int neg = (x >> 31) & 0x1;
    int y = x & (~(1 << 31));
    int mask = (neg << 31) >> 31;//若负数，则11...1，若正数，则0
    int num = (y ^ mask) + neg;
    return num;
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y) {
    int sum = x + y;
    int px = !(x >> 31);
    int py = !(y >> 31);
    int ps = !(sum >> 31);
    int pflow = px & py & (!ps);
    int nflow = !px & !py & ps;
    int y1 = pflow << 31;
    int y2 = nflow << 31;
    int y3 = y2 >> 31;
    int ans = (sum | (y1 >> 31) | y3) + (~y1 + 1) + (~(y3 & (~(1 << 31))) + 1);
    return ans;
    //思路:(nflow<<31)>>31=11...1(负溢出）或0（不溢出），pflow同理
    //若正溢出，应该用11...1-100...0=011...1,其中10...0=pflow<<31
    //若负溢出，应该用11...1-011...1=100...0，其中011...1=y3 & (~(1 << 31))
}
/*
 * trueFiveEighths - multiplies by 5/8 rounding toward 0,
 *  avoiding errors due to overflow
 *  Examples: trueFiveEighths(11) = 6
 *            trueFiveEighths(-9) = -5
 *            trueFiveEighths(0x30000000) = 0x1E000000 (no overflow)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 4
 */
int trueFiveEighths(int x) {
    int add1, add2;
    int bias = (x >> 31) & 7;//正数0，负数0x111
    int ans = (x >> 3) + (x >> 1);//x*(5/8)
    //接下来要处理可能舍去末尾多了的情况（正负皆有）
    int x1 = x & 7, x2 = (x & 1)<<2;
    add1 = (x1 + x2) >> 3;
    //接下来处理负数向零舍入的问题
    add2 = !!(((x + bias) >> 3) ^ (x >> 3));
    //若涉及负数向0舍入，一定是x不能被8整除，则判断是否向零舍入只需比较向零舍入和向下舍入是否一样
    //若一样，说明x被8整除，不需再加1
    return ans + add1 + add2;
}
/*
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    int f,sign;
    int e = (uf >> 23) & 0xff;
    if (e == 0xff) return uf;
    f = uf & 0x7fffff;
    sign = (uf >> 31) & 0x1;
    if (e == 0)
    {
        f = f << 1;
        return (sign << 31) | (e << 23) | f;
    }
    else if (e != 0)
    {
        e++;
        if (e == 0xff)
        {
            if (sign)
                return 0xff800000;
            else
                return 0x7f800000;
        }
        else
            return (sign << 31) | (e << 23) | f;
    }
}
/*
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
    int f, add, sign;
    int e = (uf >> 23) & 0xff;
    if (e == 0xff) return uf;
    f = uf & 0x7fffff;
    sign = (uf >> 31) & 0x1;
    add = ((f & 0x3) == 3);
    if (e == 0)
    {
        f = (f + add) >> 1;
        return (sign << 31) | (e << 23) | f;
    }
    else if (e == 1)
    {
        e = 0;
        f = f | (0x800000);
        f = (f + add) >> 1;
        return (sign << 31) | (e << 23) | f;
    }
    else if (e != 0)
    {
        e--;
        return (sign << 31) | (e << 23) | f;
    }
}
/*
 * float_f2i - Return bit-level equivalent of expression (int) f
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
int float_f2i(unsigned uf) {
    int f, E,neg,mi;
    int e = (uf >> 23) & 0xff;
    f = (uf & 0x7fffff) | (0x800000);
    if (e == 0)
        return 0;//取值在-1-1之间，舍去
    else if (e == 0xff)
        return 0x80000000u;
    E = e - 127;
    if (E < 0) return 0;//取值在-1-1之间，舍去
    else if (E >= 31) return 0x80000000u;//超出整数最大表示范围，为2^31*1.几
    //若为31，则最高位为1，f将会表示一个负数，这与f应该是一个正数不符
    neg = (uf >> 31);//正数还是负数
    mi = E - 23;
    if (mi > 0)
        f = f << mi;
    else
        f = f >> (23 - E);
    if (neg)
        return ~f + 1;
    else
        return f;

}
/*
 * float_pwr2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical同样的 bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_pwr2(int x) {
    if (x > 127)
        return 0x7f800000;
    else if (x < -149)//是-149还是-148
        return 0;
    else if (x < -126)//e=0,f中有一个1
    {
        int y = 149 + x;

        //是y=148+x还是149+x
        return 1 << y;
    }
    else if (x >= -126)
    {
        int e = 127 + x;
        return e << 23;
    }
}

