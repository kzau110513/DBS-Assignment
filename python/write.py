import os

# Prompt the user to enter a file path
file_path = "/tmp/j24su/mount/myfile.txt"

# Open the file for writing
try:
    open = input("input number to execute open...")
    file_descriptor = os.open(file_path, os.O_WRONLY)
    print("File descriptor for %s: %d" % (file_path, file_descriptor))

    open = input("input number to execute write...")
    data = raw_input("write:")
    # data = 'This is some data to write to the file.'
    os.write(file_descriptor, data)

    open = input("input number to execute fsync...")
    os.fsync(file_descriptor)
    fsync = input("fsync pending")

    open = input("input number to execute close...")
    os.close(file_descriptor)

    open = input("input number to execute stat...")
    os.stat(file_path)
    
except OSError:
    print("Error: could not open file.")