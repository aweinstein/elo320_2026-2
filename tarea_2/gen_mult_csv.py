#!/usr/bin/env python3
"""Generate a CSV of random big-integer multiplication test cases.

Each line: a,b,a*b
a and b are unsigned integers with exactly n limbs in base beta.

An integer with n limbs in base beta has n * beta_exp decimal digits, where
beta = 10**beta_exp.
"""

import argparse
import csv
import random
import sys


# Python 3.11+ limits int<->str conversion to 4300 digits by default
if hasattr(sys, "set_int_max_str_digits"):
    sys.set_int_max_str_digits(0)


def random_n_digit(rng: random.Random, n: int) -> int:
    """Uniform random integer with exactly n decimal digits (no leading zero)."""
    if n < 1:
        raise ValueError("NUM_DIGITS must be >= 1")
    lo = 10 ** (n - 1) if n > 1 else 0
    hi = 10 ** n - 1
    return rng.randint(lo, hi)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("n", type=int, help="number of limbs (base 10^19)")
    args = parser.parse_args()

    seed = 42               # set to an int for reproducible output
    n = args.n
    beta_exp = 19
    num_digits = n * beta_exp
    num_lines = 100          # number of lines in the file
    output_file = f"mult_cases_{n}.csv"

    rng = random.Random(seed)
    with open(output_file, "w", newline="") as f:
        writer = csv.writer(f)
        for i in range(num_lines):
            print(f"Generando linea {i + 1}")
            a = random_n_digit(rng, num_digits)
            b = random_n_digit(rng, num_digits)
            writer.writerow([a, b, a * b])

    print(f"Resultado en {output_file}")


if __name__ == "__main__":
    main()



# #!/usr/bin/env python3
# """Generate a CSV of random big-integer multiplication test cases.

# Each line: a,b,a*b
# a and b are unsigned integers with exactly n limbs in base beta.

# An integer with n limbs in base beta has n * beta_exp decimal digits, where
# beta = 10**beta_exp.
# """

# import csv
# import random
# import sys




# # Python 3.11+ limits int<->str conversion to 4300 digits by default
# if hasattr(sys, "set_int_max_str_digits"):
#     sys.set_int_max_str_digits(0)


# def random_n_digit(rng: random.Random, n: int) -> int:
#     """Uniform random integer with exactly n decimal digits (no leading zero)."""
#     if n < 1:
#         raise ValueError("NUM_DIGITS must be >= 1")
#     lo = 10 ** (n - 1) if n > 1 else 0
#     hi = 10 ** n - 1
#     return rng.randint(lo, hi)


# def main() -> None:
#     seed = 42               # set to an int for reproducible output
#     n = 100
#     beta_exp = 19
#     num_digits = n * beta_exp
#     num_lines = 100          # number of lines in the file
#     output_file = f"mult_cases_{n}.csv"

#     rng = random.Random(seed)
#     with open(output_file, "w", newline="") as f:
#         writer = csv.writer(f)
#         for i in range(num_lines):
#             print(f"Generando linea {i + 1}")
#             a = random_n_digit(rng, num_digits)
#             b = random_n_digit(rng, num_digits)
#             writer.writerow([a, b, a * b])

#     print(f"Resultado en {output_file}")


# if __name__ == "__main__":
#     main()
