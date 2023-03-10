import os

# Prompt the user to enter a file path
file_path = "/tmp/j24su/mount/myfile.txt"

# Open the file for writing
try:
    open = input("input number to execute open...")
    file_descriptor = os.open(file_path, os.O_RDONLY)
    print("File descriptor for %s: %d" % (file_path, file_descriptor))

    open = input("input number to execute stat(get the file size)...")
    size = os.stat(file_path).st_size

    open = input("input number to execute read...")
    readbuf = os.read(file_descriptor, size)
    print(readbuf)
    

    open = input("input number to execute close...")
    os.close(file_descriptor)

    open = input("input number to execute stat...")
    os.stat(file_path)
    
except OSError:
    print("Error: could not open file.")