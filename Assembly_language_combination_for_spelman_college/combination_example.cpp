#include <cstdint>
#include <iostream>
using namespace std;


uint16_t factorial(const uint16_t x) {
    const uint16_t FACTORIAL_OF_ZERO = 1;
    uint16_t result = FACTORIAL_OF_ZERO;

    for (uint16_t i = x; i > 0; --i) {
        result *= i;
    }

    return result;
}


int main() {
    const int INVALID_INPUT_RESULT = -1;
    int n;
    int k;

    // get and validate user input
    cout << "Enter n: ";
    cin >> n;
    cout << "Enter k: ";
    cin >> k;

    if (n <= 0 || k <= 0 || k > n) {
        cout << "result = " << INVALID_INPUT_RESULT << endl;
        return 0;
    }

    // calculate C(n,k) = n! / (k! * (n-k)!)
    uint16_t c_n_k = factorial(n) / (factorial(k) * factorial(n - k));

    // write out results
    cout << "result = " << c_n_k << endl;

    return 0;
}