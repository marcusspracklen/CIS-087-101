#include <stdint.h>

#include <bitset>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>

using namespace std;

#define NUM_TESTS 10
#define MAX_VALUE 100
#define MIN_VALUE -100
uint8_t const table_width[] = {12, 12, 35, 12};

// IEEE 754 single-precision float constants
uint8_t const width = 32U;
uint8_t const exp_width = 8U;
uint8_t const mantissa_width = width - exp_width - 1;
uint8_t const bias = 127U;

/*
 * *** STUDENTS SHOULD WRITE CODE FOR THIS FUNCTION ***
 * Students should create or add any data structures needed.
 * Students should create or add any functions or classes they may need.
 */
constexpr uint32_t lsb_mask = 1U;
constexpr uint32_t sign_mask = lsb_mask << (width - 1U);
constexpr uint32_t exp_mask = (lsb_mask << exp_width) - 1U;
constexpr uint32_t mantissa_mask = (lsb_mask << mantissa_width) - 1U;
constexpr uint32_t exp_all_zeros = 0U;
constexpr uint32_t exp_all_ones = exp_mask;
constexpr uint32_t empty_mantissa = 0U;
constexpr int32_t subnormal_exponent = 1 - bias;
 
constexpr float float_zero = 0.0f;
constexpr float float_one = 1.0f;
constexpr float float_two = 2.0f;
constexpr float float_half = 0.5f;
 
// Walks the mantissa from its least significant bit, halving each step, so the result is 0.mantissa.
float mantissa_fraction(uint32_t mantissa) {
    float fraction = float_zero;
    for (uint8_t bit = 0U; bit < mantissa_width; ++bit) {
        if (mantissa & lsb_mask) {
            fraction += float_one;
        }
        fraction *= float_half;
        mantissa >>= 1U;
    }
    return fraction;
}
 
// Exponentiation by squaring: O(log n) multiplies; every step is an exact power of two.
float power_of_two(int32_t const power) {
    float base = (power < 0) ? float_half : float_two;
    uint32_t remaining = (power < 0) ? -power : power;
    float result = float_one;
    while (remaining) {
        if (remaining & lsb_mask) {
            result *= base;
        }
        base *= base;
        remaining >>= 1U;
    }
    return result;
}
 
float decode_subnormal(uint32_t const mantissa) {
    return mantissa_fraction(mantissa) * power_of_two(subnormal_exponent);
}
 
float decode_normal(int32_t const exponent, uint32_t const mantissa) {
    return (float_one + mantissa_fraction(mantissa)) * power_of_two(exponent - bias);
}
 
float decode_special(uint32_t const mantissa) {
    return (mantissa == empty_mantissa) ? numeric_limits<float>::infinity() : numeric_limits<float>::quiet_NaN();
}
 
float ieee_754(uint32_t const data) {
    bool const is_negative = (data & sign_mask) != 0U;
    uint32_t const exponent = (data >> mantissa_width) & exp_mask;
    uint32_t const mantissa = data & mantissa_mask;
 
    float magnitude;
    switch (exponent) {
        case exp_all_zeros:
            magnitude = decode_subnormal(mantissa);
            break;
        case exp_all_ones:
            magnitude = decode_special(mantissa);
            break;
        default:
            magnitude = decode_normal(exponent, mantissa);
            break;
    }
    return is_negative ? -magnitude : magnitude;
}

/*
 * *** STUDENTS SHOULD NOT NEED TO CHANGE THE CODE BELOW. IT IS A CUSTOM TEST HARNESS. ***
 */

void header() {
    cout << left << setw(table_width[0]) << setfill(' ') << "pass/fail";
    cout << left << setw(table_width[1]) << setfill(' ') << "value";
    cout << left << setw(table_width[2]) << setfill(' ') << "bits";
    cout << left << setw(table_width[3]) << setfill(' ') << "IEEE-754" << endl;

    cout << left << setw(table_width[0]) << setfill(' ') << "--------";
    cout << left << setw(table_width[1]) << setfill(' ') << "--------";
    cout << left << setw(table_width[2]) << setfill(' ') << "--------";
    cout << left << setw(table_width[3]) << setfill(' ') << "--------" << endl;
}

void print_row(bool const test_success, float const rand_val, uint32_t const val_int, float const ieee_754_value) {
    // print results
    string const pass_fail = test_success ? "PASS" : "FAIL";
    cout << left << setw(table_width[0]) << setfill(' ') << pass_fail;
    cout << left << setw(table_width[1]) << setfill(' ') << rand_val;
    cout << left << setw(table_width[2]) << setfill(' ') << bitset<width>(val_int);
    cout << left << setw(table_width[3]) << setfill(' ') << ieee_754_value << endl;
}

template <typename T>
T rand_min_max(T const min, T const max) {
    T const rand_val =
        min + static_cast<double>(static_cast<double>(rand())) / (static_cast<double>(RAND_MAX / (max - min)));
    return rand_val;
}

bool test() {
    // the union
    union float_uint {
        float val_float;
        uint32_t val_int;
    } data;

    // print header
    header();

    // seed the random number generator
    srand(time(NULL));

    bool success = true;
    uint16_t pass = 0;
    for (size_t i = 0; i < NUM_TESTS; i++) {
        // random value
        float const rand_val = rand_min_max<float>(MIN_VALUE, MAX_VALUE);

        data.val_float = rand_val;

        // calculate using ieee_754 function
        float ieee_754_value = ieee_754(data.val_int);

        // test the results
        float const epsilon = std::numeric_limits<float>::epsilon();
        bool test_success = (abs(ieee_754_value - rand_val) < epsilon);
        if (test_success) {
            pass += 1;
        }

        // print row
        print_row(test_success, rand_val, data.val_int, ieee_754_value);
    }

    // summarize results
    cout << "-------------------------------------------" << endl;
    if (pass == NUM_TESTS) {
        cout << "SUCCESS ";
    } else {
        cout << "FAILURE ";
    }
    cout << pass << "/" << NUM_TESTS << " passed" << endl;
    cout << "-------------------------------------------" << endl;

    return success;
}

int main() {
    if (!test()) {
        return -1;
    }
    return 0;
}