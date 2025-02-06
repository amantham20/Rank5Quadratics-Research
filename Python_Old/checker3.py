from fractions import Fraction
from sympy.ntheory.primetest import is_square
import concurrent.futures
import queue
import threading
import subprocess
import time
from tqdm import tqdm

def process_chunk(chunk):
    for line1, line2 in chunk:
        val1, val2 = Fraction(line1.split(',')[1]), Fraction(line2.split(',')[1])
        product = val1 * val2

        if is_square(product.numerator) and is_square(product.denominator):
            print("Square Free")
            print(line1.split(',')[0], line2.split(',')[0])
            print(val1, val2)
            print(val1 * val2)

def producer(filename, task_queue, chunk_size):
    with open(filename, 'r') as file:
        lines = file.readlines()
        for i in range(0, len(lines), chunk_size):
            chunk = [(lines[j], lines[k]) for j in range(i, min(i + chunk_size, len(lines))) for k in range(j + 1, len(lines))]
            task_queue.put(chunk)

def main():
    filename = 'Numbers/NumberAndOut150.log'
    number_of_lines = int(subprocess.check_output('wc -l ' + filename, shell=True).split()[0])
    print(number_of_lines)

    chunk_size = 100  # Adjust based on your file size
    max_threads = 4   # Example: number of CPU cores
    task_queue = queue.Queue()

    startTime = time.perf_counter()

    # Start producer thread
    threading.Thread(target=producer, args=(filename, task_queue, chunk_size), daemon=True).start()

    with concurrent.futures.ThreadPoolExecutor(max_workers=max_threads) as executor:
        progress = tqdm(total=number_of_lines, desc="Processing", unit="line")
        while True:
            try:
                chunk = task_queue.get(timeout=10)  # Adjust timeout as needed
                if chunk is None:  # Signal that producer is done
                    break
                executor.submit(process_chunk, chunk)
                progress.update(len(chunk))
            except queue.Empty:
                break  # Break if no tasks are received within the timeout

    endTime = time.perf_counter()
    print(f"Completed in {endTime - startTime} seconds")

if __name__ == "__main__":
    main()
