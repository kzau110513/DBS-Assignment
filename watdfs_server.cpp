//
// Starter code for CS 454/654
// You SHOULD change this file
//
#define PRINT_ERR 1

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

	// Let sys_ret be the return code from the stat system call.
	uint64_t open_ret = 0;

	// TODO: Make the stat system call, which is the corresponding system call needed
	// to support getattr. You should use the statbuf as an argument to the stat system call.
	open_ret = open(full_path, fi->flags);
	if (open_ret > 0)
	{
		fi->fh = open_ret;
	}
	else
	{
		*ret = -errno;
	}

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

	// Let sys_ret be the return code from the stat system call.
	int sys_ret = 0;

	// TODO: Make the stat system call, which is the corresponding system call needed
	// to support getattr. You should use the statbuf as an argument to the stat system call.
	sys_ret = close(fi->fh);

	if (sys_ret < 0)
	{
		// If there is an error on the system call, then the return code should
		// be -errno.
		*ret = -errno;
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
	// register read(not complete)
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
