//
// Starter code for CS 454/654
// You SHOULD change this file
//
// #define PRINT_ERR 1

#include "rpc.h"
#include "debug.h"
#include <iostream>
INIT_LOG

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

#include <fcntl.h>
#include <fuse.h>
#include <map>
#include "rw_lock.h"

struct file_status_s
{
	int serverMode; // 0 for read, 1 for write
	int openNum;
	rw_lock_t *lock;
};

std::map<std::string, struct file_status_s> openFilesStatus_s;

// Global state server_persist_dir.
char *server_persist_dir = nullptr;

// Important: the server needs to handle multiple concurrent client requests.
// You have to be carefuly in handling global variables, esp. for updating them.
// Hint: use locks before you update any global variable.

// We need to operate on the path relative to the the server_persist_dir.
// This function returns a path that appends the given short path to the
// server_persist_dir. The character array is allocated on the heap, therefore
// it should be freed after use.
// Tip: update this function to return a unique_ptr for automatic memory management.
char *get_full_path(char *short_path)
{
	int short_path_len = strlen(short_path);
	int dir_len = strlen(server_persist_dir);
	int full_len = dir_len + short_path_len + 1;

	char *full_path = (char *)malloc(full_len);

	// First fill in the directory.
	strcpy(full_path, server_persist_dir);
	// Then append the path.
	strcat(full_path, short_path);
	DLOG("Full path: %s\n", full_path);

	return full_path;
}

// The server implementation of getattr.
int watdfs_getattr(int *argTypes, void **args)
{
	// Get the arguments.
	// The first argument is the path relative to the mountpoint.
	char *short_path = (char *)args[0];
	// The second argument is the stat structure, which should be filled in
	// by this function.
	struct stat *statbuf = (struct stat *)args[1];
	// The third argument is the return code, which should be set be 0 or -errno.
	int *ret = (int *)args[2];

	// Get the local file name, so we call our helper function which appends
	// the server_persist_dir to the given path.
	char *full_path = get_full_path(short_path);

	// Initially we set set the return code to be 0.
	*ret = 0;

	// Let sys_ret be the return code from the stat system call.
	int sys_ret = 0;

	// TODO: Make the stat system call, which is the corresponding system call needed
	// to support getattr. You should use the statbuf as an argument to the stat system call.
	sys_ret = stat(full_path, statbuf);

	if (sys_ret < 0)
	{
		// If there is an error on the system call, then the return code should
		// be -errno.
		*ret = -errno;
	}

	// Clean up the full path, it was allocated on the heap.
	free(full_path);

	DLOG("Returning code of getattr: %d", *ret);
	// The RPC call succeeded, so return 0.
	return 0;
}

// The server implementation of mknod.
int watdfs_mknod(int *argTypes, void **args)
{
	// Get the arguments.
	// The first argument is the path relative to the mountpoint.
	char *short_path = (char *)args[0];
	// The second argument: mode
	int *mode = (int *)args[1];
	// The third argument: dev
	long *dev = (long *)args[2];
	// The fourth argument is the return code, which should be set be 0 or -errno.
	int *ret = (int *)args[3];

	// Get the local file name, so we call our helper function which appends
	// the server_persist_dir to the given path.
	char *full_path = get_full_path(short_path);

	// Initially we set set the return code to be 0.
	*ret = 0;

	// Let sys_ret be the return code from the stat system call.
	int sys_ret = 0;

	// TODO: Make the stat system call, which is the corresponding system call needed
	// to support getattr. You should use the statbuf as an argument to the stat system call.
	sys_ret = mknod(full_path, *mode, *dev);

	if (sys_ret < 0)
	{
		// If there is an error on the system call, then the return code should
		// be -errno.
		*ret = -errno;
	}

	// Clean up the full path, it was allocated on the heap.
	free(full_path);

	DLOG("Returning code of mknod: %d", *ret);
	// The RPC call succeeded, so return 0.
	return 0;
}

// The server implementation of open.
int watdfs_open(int *argTypes, void **args)
{
	// Get the arguments.
	// The first argument is the path relative to the mountpoint.
	char *short_path = (char *)args[0];
	// The second argument is the stat structure, which should be filled in
	// by this function.
	struct fuse_file_info *fi = (struct fuse_file_info *)args[1];
	// The third argument is the return code, which should be set be 0 or -errno.
	int *ret = (int *)args[2];

	// Get the local file name, so we call our helper function which appends
	// the server_persist_dir to the given path.
	char *full_path = get_full_path(short_path);

	// Initially we set set the return code to be 0.
	*ret = 0;

	int openReqFlag = fi->flags & O_ACCMODE;

	struct file_status_s fileStatus;

	// file has been opened
	if (openFilesStatus_s.find(short_path) != openFilesStatus_s.end() && openFilesStatus_s[short_path].openNum > 0)
	{
		DLOG("file has been opened");
		int serMode = openFilesStatus_s[short_path].serverMode;
		// current open is read mode
		if (serMode == 0)
		{
			DLOG("current open is read mode");
			// the open request flag is write flag
			if (openReqFlag != O_RDONLY)
			{
				DLOG("the open request flag is write flag");
				openFilesStatus_s[short_path].serverMode = 1;
			}
			// the open request flag is read flag, do nothing
			else
			{
				DLOG("the open request flag is read flag");
			}
		}
		// current open is write mode
		else
		{
			DLOG("current open is write mode");
			// the open request flag is write flag
			if (openReqFlag != O_RDONLY)
			{
				// two write open conflict
				DLOG("two write open conflict");
				free(full_path);
				*ret = -EACCES;
				return 0;
			}
			// the open request flag is read flag, do nothing
			else
			{
				DLOG("the open request flag is read flag");
			}
		}
		openFilesStatus_s[short_path].openNum += 1;
	}
	// file has not been opened
	else
	{
		DLOG("file has not been opened");
		// init the new lock
		rw_lock_t *newlock = new rw_lock_t;
		int lockRet = rw_lock_init(newlock);
		if (lockRet < 0)
		{
			DLOG("lock init fail");
			free(full_path);
			*ret = lockRet;
			return 0;
		}

		fileStatus.lock = newlock;
		// init the mode
		if (openReqFlag == O_RDONLY)
		{
			fileStatus.serverMode = 0;
		}
		else
		{
			fileStatus.serverMode = 1;
		}
		// init the num
		fileStatus.openNum = 1;
		// add to map
		openFilesStatus_s[short_path] = fileStatus;
	}

	int openRet = open(full_path, (fi->flags & O_ACCMODE));
	if (fi->fh < 0)
	{
		DLOG("open on server failed");
		free(full_path);
		*ret = -errno;
		return 0;
	}
	DLOG("file fh is %ld", fi->fh);
	fi->fh = openRet;

	// Clean up the full path, it was allocated on the heap.
	free(full_path);

	DLOG("Returning code of open: %d", *ret);
	// The RPC call succeeded, so return 0.
	return 0;
}

// The server implementation of release.
int watdfs_release(int *argTypes, void **args)
{
	// Get the arguments.
	// The first argument is the path relative to the mountpoint.
	char *short_path = (char *)args[0];
	// The second argument is the stat structure, which should be filled in
	// by this function.
	struct fuse_file_info *fi = (struct fuse_file_info *)args[1];
	// The third argument is the return code, which should be set be 0 or -errno.
	int *ret = (int *)args[2];

	// Get the local file name, so we call our helper function which appends
	// the server_persist_dir to the given path.
	char *full_path = get_full_path(short_path);

	// Initially we set set the return code to be 0.
	*ret = 0;

	int openReqFlag = fi->flags & O_ACCMODE;

	DLOG("fi->fh is %ld", fi->fh);
	int sys_ret = close(fi->fh);
	if (sys_ret < 0)
	{
		DLOG("server close fail");
		free(full_path);
		*ret = sys_ret;
		return 0;
	}

	DLOG("before close open number: %d", openFilesStatus_s[short_path].openNum);

	if (openFilesStatus_s.find(short_path) == openFilesStatus_s.end())
	{
		DLOG("erroe: the openFilesStatus_s should have the key", short_path);
		free(full_path);
		*ret = -EPERM;
		return 0;
	}
	else
	{
		openFilesStatus_s[short_path].openNum -= 1;
		DLOG("after close open number: %d", openFilesStatus_s[short_path].openNum);
		// if the close belongs to writer, update to read mode
		if (openReqFlag != O_RDONLY)
		{
			openFilesStatus_s[short_path].serverMode = 0;
		}
	}

	// if there is no opener, destroy the lock and erase the file from map
	if (openFilesStatus_s[short_path].openNum == 0)
	{
		rw_lock_destroy(openFilesStatus_s[short_path].lock);
		openFilesStatus_s.erase(short_path);
	}

	// Clean up the full path, it was allocated on the heap.
	free(full_path);

	DLOG("Returning code of release: %d", *ret);
	// The RPC call succeeded, so return 0.
	return 0;
}

// The server implementation of read.
int watdfs_read(int *argTypes, void **args)
{
	// Get the arguments.
	// The first argument is the path relative to the mountpoint.
	char *short_path = (char *)args[0];

	char *buf = (char *)args[1];

	long *size = (long *)args[2];

	long *offset = (long *)args[3];

	// The second argument is the stat structure, which should be filled in
	// by this function.
	struct fuse_file_info *fi = (struct fuse_file_info *)args[4];
	// The third argument is the return code, which should be set be 0 or -errno.
	int *ret = (int *)args[5];

	// Get the local file name, so we call our helper function which appends
	// the server_persist_dir to the given path.
	char *full_path = get_full_path(short_path);

	// Initially we set set the return code to be 0.
	*ret = 0;

	// Let sys_ret be the return code from the stat system call.
	int pread_ret = 0;

	// TODO: Make the stat system call, which is the corresponding system call needed
	// to support getattr. You should use the statbuf as an argument to the stat system call.

	DLOG("bufsize: %ld, offset_each: %ld, before pread", *size, *offset);
	pread_ret = pread(fi->fh, buf, *size, *offset); // pread return the actual read size
	DLOG("buf: %s, returnCode: %d, offset: %ld, after pread", buf, *ret, *offset);

	if (pread_ret > 0)
	{
		*ret = pread_ret;
	}
	else
	{
		*ret = -errno;
	}

	// Clean up the full path, it was allocated on the heap.
	free(full_path);

	DLOG("Returning code of read: %d", *ret);
	// The RPC call succeeded, so return 0.
	return 0;
}

// The server implementation of write.
int watdfs_write(int *argTypes, void **args)
{
	// Get the arguments.
	// The first argument is the path relative to the mountpoint.
	char *short_path = (char *)args[0];

	char *buf = (char *)args[1];

	long *size = (long *)args[2];

	long *offset = (long *)args[3];

	// The second argument is the stat structure, which should be filled in
	// by this function.
	struct fuse_file_info *fi = (struct fuse_file_info *)args[4];
	// The third argument is the return code, which should be set be 0 or -errno.
	int *ret = (int *)args[5];

	// Get the local file name, so we call our helper function which appends
	// the server_persist_dir to the given path.
	char *full_path = get_full_path(short_path);

	// Initially we set set the return code to be 0.
	*ret = 0;

	// Let sys_ret be the return code from the stat system call.
	int pwrite_ret = 0;

	// TODO: Make the stat system call, which is the corresponding system call needed
	// to support getattr. You should use the statbuf as an argument to the stat system call.

	DLOG("bufsize: %ld, offset_each: %ld, before pwrite", *size, *offset);
	pwrite_ret = pwrite(fi->fh, buf, *size, *offset); // pread return the actual read size
	DLOG("buf: %s, returnCode: %d, offset: %ld, after pwrite", buf, *ret, *offset);

	if (pwrite_ret > 0)
	{
		*ret = pwrite_ret;
	}
	else
	{
		*ret = -errno;
	}

	// Clean up the full path, it was allocated on the heap.
	free(full_path);

	DLOG("Returning code of write: %d", *ret);
	// The RPC call succeeded, so return 0.
	return 0;
}

// The server implementation of truncate.
int watdfs_truncate(int *argTypes, void **args)
{
	// Get the arguments.
	// The first argument is the path relative to the mountpoint.
	char *short_path = (char *)args[0];
	// The second argument: newsize
	long *newsize = (long *)args[1];
	// The fourth argument is the return code, which should be set be 0 or -errno.
	int *ret = (int *)args[2];

	// Get the local file name, so we call our helper function which appends
	// the server_persist_dir to the given path.
	char *full_path = get_full_path(short_path);

	// Initially we set set the return code to be 0.
	*ret = 0;

	// Let sys_ret be the return code from the stat system call.
	int sys_ret = 0;

	// TODO: Make the stat system call, which is the corresponding system call needed
	// to support getattr. You should use the statbuf as an argument to the stat system call.
	sys_ret = truncate(full_path, *newsize);

	if (sys_ret < 0)
	{
		// If there is an error on the system call, then the return code should
		// be -errno.
		*ret = -errno;
	}

	// Clean up the full path, it was allocated on the heap.
	free(full_path);

	DLOG("Returning code of truncate: %d", *ret);
	// The RPC call succeeded, so return 0.
	return 0;
}

// The server implementation of fsync.
int watdfs_fsync(int *argTypes, void **args)
{
	// Get the arguments.
	// The first argument is the path relative to the mountpoint.
	char *short_path = (char *)args[0];
	// The second argument is the stat structure, which should be filled in
	// by this function.
	struct fuse_file_info *fi = (struct fuse_file_info *)args[1];
	// The third argument is the return code, which should be set be 0 or -errno.
	int *ret = (int *)args[2];

	// Get the local file name, so we call our helper function which appends
	// the server_persist_dir to the given path.
	char *full_path = get_full_path(short_path);

	// Initially we set set the return code to be 0.
	*ret = 0;

	// Let sys_ret be the return code from the stat system call.
	int sys_ret = 0;

	// TODO: Make the stat system call, which is the corresponding system call needed
	// to support getattr. You should use the statbuf as an argument to the stat system call.
	sys_ret = fsync(fi->fh);

	if (sys_ret < 0)
	{
		// If there is an error on the system call, then the return code should
		// be -errno.
		*ret = -errno;
	}

	// Clean up the full path, it was allocated on the heap.
	free(full_path);

	DLOG("Returning code of fsync: %d", *ret);
	// The RPC call succeeded, so return 0.
	return 0;
}

// The server implementation of utimensat.
int watdfs_utimensat(int *argTypes, void **args)
{
	// Get the arguments.
	// The first argument is the path relative to the mountpoint.
	char *short_path = (char *)args[0];
	// The second argument is the stat structure, which should be filled in
	// by this function.
	struct timespec *ts = (struct timespec *)args[1]; // IMPORTANT: ts is an array!!!!!!!!!!!!!!!!!!!!
	// The third argument is the return code, which should be set be 0 or -errno.
	int *ret = (int *)args[2];

	// Get the local file name, so we call our helper function which appends
	// the server_persist_dir to the given path.
	char *full_path = get_full_path(short_path);

	// Initially we set set the return code to be 0.
	*ret = 0;

	// Let sys_ret be the return code from the stat system call.
	int sys_ret = 0;

	// TODO: Make the stat system call, which is the corresponding system call needed
	// to support getattr. You should use the statbuf as an argument to the stat system call.
	int open_ret = open(full_path, O_RDWR);
	if (open_ret < 0)
	{
		DLOG("the open fails");
		*ret = -errno;
	}

	sys_ret = utimensat(open_ret, full_path, ts, 0);

	if (sys_ret < 0)
	{
		// If there is an error on the system call, then the return code should
		// be -errno.
		DLOG("the utimensat fails");
		*ret = -errno;
	}

	// Clean up the full path, it was allocated on the heap.
	free(full_path);

	DLOG("Returning code of utimensat: %d", *ret);
	// The RPC call succeeded, so return 0.
	return 0;
}

int watdfs_lock(int *argTypes, void **args)
{
	char *short_path = (char *)args[0];
	rw_lock_mode_t *mode = (rw_lock_mode_t *)args[1];
	int *ret = (int *)args[2];

	char *full_path = get_full_path(short_path);

	*ret = 0;

	int sys_ret = rw_lock_lock(openFilesStatus_s[short_path].lock, *mode);

	if (sys_ret < 0)
	{
		*ret = -errno;
	}

	free(full_path);

	return 0;
}

int watdfs_unlock(int *argTypes, void **args)
{
	char *short_path = (char *)args[0];
	rw_lock_mode_t *mode = (rw_lock_mode_t *)args[1];
	int *ret = (int *)args[2];

	char *full_path = get_full_path(short_path);

	*ret = 0;

	int sys_ret = rw_lock_unlock(openFilesStatus_s[short_path].lock, *mode);

	if (sys_ret < 0)
	{
		*ret = -errno;
	}

	free(full_path);

	return 0;
}

// The main function of the server.
int main(int argc, char *argv[])
{
	// argv[1] should contain the directory where you should store data on the
	// server. If it is not present it is an error, that we cannot recover from.
	if (argc != 2)
	{
		// In general you shouldn't print to stderr or stdout, but it may be
		// helpful here for debugging. Important: Make sure you turn off logging
		// prior to submission!
		// See watdfs_client.c for more details
#ifdef PRINT_ERR
		std::cerr << "Usaage:" << argv[0] << " server_persist_dir";
#endif
		return -1;
	}
	// Store the directory in a global variable.
	server_persist_dir = argv[1];

	// TODO: Initialize the rpc library by calling `rpcServerInit`.
	// Important: `rpcServerInit` prints the 'export SERVER_ADDRESS' and
	// 'export SERVER_PORT' lines. Make sure you *do not* print anything
	// to *stdout* before calling `rpcServerInit`.
	DLOG("Initializing server...");
	int initRet = rpcServerInit();

	int ret = 0;
	// TODO: If there is an error with `rpcServerInit`, it maybe useful to have
	// debug-printing here, and then you should return.
	if (initRet < 0)
	{
#ifdef PRINT_ERR
		std::cerr << "Server Initialization Error: " << initRet << std::endl;
#endif
		ret = NOT_INIT;
		return ret;
	}

	// TODO: Register your functions with the RPC library.
	// Note: The braces are used to limit the scope of `argTypes`, so that you can
	// reuse the variable for multiple registrations. Another way could be to
	// remove the braces and use `argTypes0`, `argTypes1`, etc.

	// register getattr
	{
		// There are 3 args for the function (see watdfs_client.c for more
		// detail).
		int argTypes[4];
		// First is the path.
		argTypes[0] =
			(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u; // why 1u: the server could not know the length of array, so set as 1
		// The second argument is the statbuf.
		argTypes[1] =
			(1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;
		// The third argument is the retcode.
		argTypes[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);
		// Finally we fill in the null terminator.
		argTypes[3] = 0;

		// We need to register the function with the types and the name.
		ret = rpcRegister((char *)"getattr", argTypes, watdfs_getattr);
		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of getattr: " << ret << std::endl;
#endif
			return ret;
		}
	}
	// register mknod
	{
		// There are 4 args for the function (see watdfs_client.c for more
		// detail).
		int argTypes[5];
		// First is the path.
		argTypes[0] =
			(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;
		// The second argument is the mode.
		argTypes[1] = (1u << ARG_INPUT) | (ARG_INT << 16u);
		// The third argument is the dev.
		argTypes[2] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
		// The fourth argument is the retcode.
		argTypes[3] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);
		// Finally we fill in the null terminator.
		argTypes[4] = 0;

		// We need to register the function with the types and the name.
		ret = rpcRegister((char *)"mknod", argTypes, watdfs_mknod);
		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of mknod: " << ret << std::endl;
#endif
			return ret;
		}
	}
	// register open
	{
		// There are 3 args for the function (see watdfs_client.c for more
		// detail).
		int argTypes[4];
		// First is the path.
		argTypes[0] =
			(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u; // why 1u: the server could not know the length of array, so set as 1
		// The second argument is the fi.
		argTypes[1] =
			(1u << ARG_INPUT) | (1u << ARG_OUTPUT) |
			(1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;
		// The third argument is the retcode.
		argTypes[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);
		// Finally we fill in the null terminator.
		argTypes[3] = 0;

		// We need to register the function with the types and the name.
		ret = rpcRegister((char *)"open", argTypes, watdfs_open);
		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of open: " << ret << std::endl;
#endif
			return ret;
		}
	}
	// register release
	{
		// There are 3 args for the function (see watdfs_client.c for more
		// detail).
		int argTypes[4];
		// First is the path.
		argTypes[0] =
			(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u; // why 1u: the server could not know the length of array, so set as 1
		// The second argument is the fi.
		argTypes[1] =
			(1u << ARG_INPUT) |
			(1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;
		// The third argument is the retcode.
		argTypes[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);
		// Finally we fill in the null terminator.
		argTypes[3] = 0;

		// We need to register the function with the types and the name.
		ret = rpcRegister((char *)"release", argTypes, watdfs_release);
		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of release: " << ret << std::endl;
#endif
			return ret;
		}
	}
	// register read
	{
		// There are 6 args for the function (see watdfs_client.c for more
		// detail).
		int argTypes[7];
		// First is the path.
		argTypes[0] =
			(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u; // why 1u: the server could not know the length of array, so set as 1
		// buf
		argTypes[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) |
					  (ARG_CHAR << 16u) | 1u;
		// size
		argTypes[2] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
		// offset
		argTypes[3] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
		// The fifth argument is the fi.
		argTypes[4] =
			(1u << ARG_INPUT) |
			(1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;
		// The sixth argument is the retcode.
		argTypes[5] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);
		// Finally we fill in the null terminator.
		argTypes[6] = 0;

		// We need to register the function with the types and the name.
		ret = rpcRegister((char *)"read", argTypes, watdfs_read);
		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of read: " << ret << std::endl;
#endif
			return ret;
		}
	}

	// register write
	{
		// There are 6 args for the function (see watdfs_client.c for more
		// detail).
		int argTypes[7];
		// First is the path.
		argTypes[0] =
			(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u; // why 1u: the server could not know the length of array, so set as 1
		// buf
		argTypes[1] = (1u << ARG_INPUT) | (1u << ARG_ARRAY) |
					  (ARG_CHAR << 16u) | 1u;
		// size
		argTypes[2] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
		// offset
		argTypes[3] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
		// The fifth argument is the fi.
		argTypes[4] =
			(1u << ARG_INPUT) |
			(1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;
		// The sixth argument is the retcode.
		argTypes[5] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);
		// Finally we fill in the null terminator.
		argTypes[6] = 0;

		// We need to register the function with the types and the name.
		ret = rpcRegister((char *)"write", argTypes, watdfs_write);
		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of write: " << ret << std::endl;
#endif
			return ret;
		}
	}

	// register truncate
	{
		// There are 3 args for the function (see watdfs_client.c for more
		// detail).
		int argTypes[4];
		// First is the path.
		argTypes[0] =
			(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;
		// The second argument is the mode.
		argTypes[1] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
		// The fourth argument is the retcode.
		argTypes[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);
		// Finally we fill in the null terminator.
		argTypes[3] = 0;

		// We need to register the function with the types and the name.
		ret = rpcRegister((char *)"truncate", argTypes, watdfs_truncate);
		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of truncate: " << ret << std::endl;
#endif
			return ret;
		}
	}

	// register fsync
	{
		// There are 3 args for the function (see watdfs_client.c for more
		// detail).
		int argTypes[4];
		// First is the path.
		argTypes[0] =
			(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u; // why 1u: the server could not know the length of array, so set as 1
		// The second argument is the fi.
		argTypes[1] =
			(1u << ARG_INPUT) |
			(1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;
		// The third argument is the retcode.
		argTypes[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);
		// Finally we fill in the null terminator.
		argTypes[3] = 0;

		// We need to register the function with the types and the name.
		ret = rpcRegister((char *)"fsync", argTypes, watdfs_fsync);
		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of fsync: " << ret << std::endl;
#endif
			return ret;
		}
	}

	// register utimensat
	{
		// There are 3 args for the function (see watdfs_client.c for more
		// detail).
		int argTypes[4];
		// First is the path.
		argTypes[0] =
			(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u; // why 1u: the server could not know the length of array, so set as 1
		// The second argument is the fi.
		argTypes[1] =
			(1u << ARG_INPUT) |
			(1u << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;
		// The third argument is the retcode.
		argTypes[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);
		// Finally we fill in the null terminator.
		argTypes[3] = 0;

		// We need to register the function with the types and the name.
		ret = rpcRegister((char *)"utimensat", argTypes, watdfs_utimensat);
		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of utimensat: " << ret << std::endl;
#endif
			return ret;
		}
	}
	// register lock
	{
		int argTypes[4];

		argTypes[0] = (1 << ARG_INPUT) | (1 << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;

		argTypes[1] = (1 << ARG_INPUT) | (ARG_INT << 16u);

		argTypes[2] = (1 << ARG_OUTPUT) | (ARG_INT << 16u);

		argTypes[3] = 0;

		ret = rpcRegister((char *)"lock", argTypes, watdfs_lock);

		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of lock: " << ret << std::endl;
#endif
			return ret;
		}
	}
	// register unlock
	{
		int argTypes[4];

		argTypes[0] = (1 << ARG_INPUT) | (1 << ARG_ARRAY) | (ARG_CHAR << 16u) | 1u;

		argTypes[1] = (1 << ARG_INPUT) | (ARG_INT << 16u);

		argTypes[2] = (1 << ARG_OUTPUT) | (ARG_INT << 16u);

		argTypes[3] = 0;

		ret = rpcRegister((char *)"unlock", argTypes, watdfs_unlock);

		if (ret < 0)
		{
			// It may be useful to have debug-printing here.
#ifdef PRINT_ERR
			std::cerr << "RPC Server Register Error of unlock: " << ret << std::endl;
#endif
			return ret;
		}
	}

	// TODO: Hand over control to the RPC library by calling `rpcExecute`.
	int execRet = rpcExecute();

	// rpcExecute could fail so you may want to have debug-printing here, and
	// then you should return.
	if (execRet < 0)
	{
#ifdef PRINT_ERR
		std::cerr << "RPC Execution Error: " << execRet << std::endl;
#endif
		ret = FUNCTION_FAILURE;
		return ret;
	}
	return ret;
}
