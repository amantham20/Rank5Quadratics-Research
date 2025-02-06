from fractions import Fraction
from sympy.ntheory.primetest import is_square
import concurrent.futures
import subprocess
import time
from tqdm import tqdm

def process_result(line1, line2):
    val1, val2 = Fraction(line1.split(',')[1]), Fraction(line2.split(',')[1])
    product = val1 * val2

    if is_square(product.numerator) and is_square(product.denominator):
        print("Square Free")
        print(line1.split(',')[0], line2.split(',')[0])
        print(val1, val2)
        print(val1 * val2)

def process_file_pairs(filename, line1, index, progress):
    with open(filename, 'r') as file2:
        for i, line2 in enumerate(file2):
            if i <= index:
                continue
            process_result(line1, line2)
    progress.update(1)  # Update progress for each completed line

def main():
    filename = 'Numbers/NumberAndOut150.log'
    number_of_lines = int(subprocess.check_output('wc -l ' + filename, shell=True).split()[0])
    print(number_of_lines)
    
    max_threads = 1
    startTime = time.perf_counter()

    with open(filename, 'r') as file1, concurrent.futures.ThreadPoolExecutor(max_workers=max_threads) as executor:
        progress = tqdm(total=number_of_lines, desc="Processing", unit="line")
        for index, line1 in enumerate(file1):
            executor.submit(process_file_pairs, filename, line1, index, progress)

    endTime = time.perf_counter()
    print(f"Completed in {endTime - startTime} seconds")

if __name__ == "__main__":
    main()
