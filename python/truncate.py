import os

# Prompt the user to enter a file path
file_path = "/tmp/j24su/mount/myfile4.txt"

# Open the file for writing
try:
    file_descriptor = os.open(file_path, os.O_RDONLY)
    print("File descriptor for %s: %d" % (file_path, file_descriptor))
    open = input("open pending")

    # data = 'This is some data to write to the file.'
    # os.write(file_descriptor, data)
    # write = input("write pending")

    os.truncate(file_descriptor, 10)
    fsync = input("truncate pending")

    os.close(file_descriptor)
    close = input("close pending")

    os.stat(file_path)
    
except OSError:
    print("Error: could not open file.")