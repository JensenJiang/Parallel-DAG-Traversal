import subprocess
import os

MAX_THREADS_NUM = 8
RUN_TIMES = 3
BUILD_PATH = r'../build'
SERIAL = r'serial'
PARALLEL = r'parallel'
DATA_PATH = r'../data'
DATA_NAME = r'_graph_40000_10000000.dat'

def speedup(s_t, p_t):
    return s_t / p_t

def efficiency(s_t, p_t, n):
    return speedup(s_t, p_t) / n

def run_and_count(bin_path, data_path, threads_num):
    avg_IO_time = .0
    avg_compute_time = .0
    for i in range(1, RUN_TIMES + 1):
        process = subprocess.run([bin_path, data_path, str(threads_num)], stderr=subprocess.PIPE, stdout=subprocess.PIPE)
        _count = 0
        for line in process.stderr.splitlines():
            if _count == 0:
                avg_IO_time += float(line.rstrip())
            elif _count == 1:
                avg_compute_time += float(line.rstrip())
            else: break
            _count += 1

    avg_IO_time /= RUN_TIMES
    avg_compute_time /= RUN_TIMES
    return (avg_IO_time, avg_compute_time)

if __name__ == '__main__':
    # Serial Program
    print("Running Serial Program ...")
    serial_IO_time, serial_compute_time = run_and_count(os.path.join(BUILD_PATH, SERIAL), os.path.join(DATA_PATH, DATA_NAME), 1)
    serial_total = serial_IO_time + serial_compute_time

    print('Serial: IO:{:.3}, computing:{:.3}'.format(serial_IO_time, serial_compute_time))

    # Parallel Program
    for i in range(1, MAX_THREADS_NUM + 1):
        print('Running Parallel {} threads Program:'.format(i))
        avg_IO_time, avg_compute_time = run_and_count(os.path.join(BUILD_PATH, PARALLEL), os.path.join(DATA_PATH, DATA_NAME), i)
        avg_total = avg_IO_time + avg_compute_time

        print('Serial: IO: {:.3}, computing: {:.3}, speedup: {:.2%}, efficiency: {:.2%}'.format(avg_IO_time, avg_compute_time, speedup(serial_total, avg_total), efficiency(serial_total, avg_total, i)))
