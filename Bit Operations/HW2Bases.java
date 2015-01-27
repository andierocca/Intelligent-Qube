/**
 * CS 2110 Spring 2015 HW2
 * Part 1 - Coding with bases
 * 
 * @author Andrea Rocca
 *
 * Global rules for this file:
 * - You may not use more than 2 conditionals per method. Conditionals are
 *   if-statements, if-else statements, or ternary expressions. The else block
 *   associated with an if-statement does not count toward this sum.
 * - You may not use more than 2 looping constructs per method. Looping
 *   constructs include for loops, while loops and do-while loops.
 * - You may not use nested loops.
 * - You may not declare any file-level variables.
 * - You may not declare any objects, other than String in select methods.
 * - You may not use switch statements.
 * - You may not use the unsigned right shift operator (>>>)
 * - You may not write any helper methods, or call any other method from this or
 *   another file to implement any method.
 * - The only Java API methods you are allowed to invoke are:
 *     String.length()
 *     String.charAt()
 *     String.equals()
 * - You may not invoke the above methods from string literals.
 *     Example: "12345".length()
 * - When concatenating numbers with Strings, you may only do so if the number
 *   is a single digit.
 *
 * Method-specific rules for this file:
 * - You may not use multiplication, division or modulus in any method, EXCEPT
 *   strdtoi.
 * - You may declare exactly one String variable each in itostrb, and itostrx.
 */
public class HW2Bases
{
    /**
     * strdtoi - Decimal String to int
     *
     * Convert a string containing ASCII characters (in decimal) to an int.
     * You do not have to handle negative numbers. The Strings we will pass in will be
     * valid decimal numbers, and able to fit in a 32-bit signed integer.
     * 
     * Example: strdtoi("123"); // => 123
     */
    public static int strdtoi(String decimal)
    {
        int output = 0;
        for (int i = 0; i < decimal.length(); i++) {
              output = (output * 10) + (((int) decimal.charAt(i)) - 48);
        }
        return output;
    }

    /**
     * strbtoi - Binary String to int
     *
     * Convert a string containing ASCII characters (in binary) to an int.
     * You do not have to handle negative numbers. The Strings we will pass in will be
     * valid binary numbers, and able to fit in a 32-bit signed integer.
     * 
     * Example: strbtoi("111"); // => 7
     */
    public static int strbtoi(String binary)
    {
        int output = 0;
        for (int i = 0; i < binary.length(); i++) {
              output = (output + output) + (((int) binary.charAt(i)) - 48);
        }
        return output;
    }

    /**
     * strxtoi - Hexadecimal String to int
     *
     * Convert a string containing ASCII characters (in hex) to an int.
     * The input string will only contain numbers and uppercase letters A-F.
     * You do not have to handle negative numbers. The Strings we will pass in will be
     * valid hexadecimal numbers, and able to fit in a 32-bit signed integer.
     * 
     * Example: strxtoi("A6"); // => 166
     */
    public static int strxtoi(String hex)
    {
        int ascii = 0, output = 0;
        for(int i = 0; i < hex.length(); i++) {
            ascii = ((int) hex.charAt(i));
            if (ascii >= 65) {
                ascii -= 55;
            } else {
                ascii -= 48;
            }
            output = (output << 4) + ascii;
        }
        return output;
    }

    /**
     * itostrb - int to Binary String
     *
     * Convert a int into a String containing ASCII characters (in binary).
     * You do not have to handle negative numbers.
     * The String returned should contain the minimum number of characters necessary to
     * represent the number that was passed in.
     * 
     * Example: itostrb(7); // => "111"
     */
    public static String itostrb(int binary)
    {
        String output = "";
        boolean leadingZeros = true;
        boolean one;
        for (int i = 31; i >= 0; i--) {
            one = (binary & (1 << i)) != 0;

            if ((!one && !leadingZeros)
            	|| (i == 0 && output.equals(""))) {
                output += "0";
            } else if (one) {
                leadingZeros = false;
                output += "1";
            }
        }

        return output;
    }

    /**
     * itostrx - int to Hexadecimal String
     *
     * Convert a int into a String containing ASCII characters (in hexadecimal).
     * The output string should only contain numbers and uppercase letters A-F.
     * You do not have to handle negative numbers.
     * The String returned should contain the minimum number of characters necessary to
     * represent the number that was passed in.
     * 
     * Example: itostrx(166); // => "A6"
     */
    public static String itostrx(int hex)
    {
        String output = "";
        boolean leadingZeros = true;

        for (int i = 28; i >= 0; i -= 4) {
            //similar to getNibble function
            int nibble = ((0x0000000F << i) & hex) >> i;

            if((nibble == 0 && !leadingZeros)
            	|| (i == 0 && output.equals(""))) {
                output += "0";
            } else if (nibble != 0) {
                leadingZeros = false;

                if (nibble > 9) {
                    output += ((char) (nibble + 55));
                } else {
                    output += ((char) (nibble + 48));
                }
            }
        }
        return output;
    }
}
