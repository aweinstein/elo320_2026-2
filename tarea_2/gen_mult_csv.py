#!/usr/bin/env python3
"""Generate a CSV of random big-integer multiplication test cases.

Each line: a,b,a*b
a and b are unsigned integers with exactly NUM_DIGITS decimal digits.
"""

import csv
import random
import sys

# ---- Parameters ----
NUM_DIGITS = 400          # digits in each operand
NUM_LINES = 100          # number of lines in the file
OUTPUT_FILE = f"mult_cases_{NUM_DIGITS}.csv"
SEED = 42               # set to an int for reproducible output
# --------------------

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
    rng = random.Random(SEED)
    with open(OUTPUT_FILE, "w", newline="") as f:
        writer = csv.writer(f)
        for _ in range(NUM_LINES):
            a = random_n_digit(rng, NUM_DIGITS)
            b = random_n_digit(rng, NUM_DIGITS)
            writer.writerow([a, b, a * b])


if __name__ == "__main__":
    main()
