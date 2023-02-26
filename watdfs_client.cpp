//
// Starter code for CS 454/654
// You SHOULD change this file
//
#define PRINT_ERR 1

#include "watdfs_client.h"
#include "debug.h"
INIT_LOG

#include "rpc.h"
#include <iostream>
#include <map>

struct file_meta
{
	int clientDesc;
	int clientMode;
	timespec tClient;
	time_t tc;
};

struct files_status
{
	time_t cacheInterval;
	char *pathToCache;
	std::map<std::string, struct file_meta> openFilesStatus;
};

//--------------------the old copy implementation of wat_cli functions in A2-------------------

// GET FILE ATTRIBUTES
int copy_getattr(void *userdata, const char *path, struct stat *statbuf)
{
	// SET UP THE RPC CALL
	DLOG("watdfs_cli_getattr called for '%s'", path);

	// getattr has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument is the stat structure. This argument is an output
	// only argument, and we treat it as a char array. The length of the array
	// is the size of the stat structure, which we can determine with sizeof.
	arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
				   (uint)sizeof(struct stat); // statbuf
	args[1] = (void *)statbuf;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[2] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"getattr", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_getattr will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("getattr rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	if (fxn_ret < 0)
	{
		// If the return code of watdfs_cli_getattr is negative (an error), then
		// we need to make sure that the stat structure is filled with 0s. Otherwise,
		// FUSE will be confused by the contradicting return values.
		memset(statbuf, 0, sizeof(struct stat));
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
}

// CREATE, OPEN AND CLOSE
int copy_mknod(void *userdata, const char *path, mode_t mode, dev_t dev)
{
	// Called to create a file.

	DLOG("watdfs_cli_mknod called for '%s'", path);

	// mknod has 4 arguments.
	int ARG_COUNT = 4;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] = (1u << ARG_INPUT) | (ARG_INT << 16u);
	args[1] = (void *)&mode;

	// The third argument
	arg_types[2] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	args[2] = (void *)&dev;

	// The fourth argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[3] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[3] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[4] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"mknod", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_getattr will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("mknod rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;

	// return -ENOSYS;
}

int copy_open(void *userdata, const char *path, struct fuse_file_info *fi)
{
	// Called during open.
	// You should fill in fi->fh.

	DLOG("watdfs_cli_open called for '%s'", path);

	// open has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] =
		(1u << ARG_INPUT) | (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[1] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"open", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("open rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	if (fxn_ret < 0)
	{
		// If the return code of watdfs_cli_getattr is negative (an error), then
		// we need to make sure that the stat structure is filled with 0s. Otherwise,
		// FUSE will be confused by the contradicting return values.
		memset(fi, 0, sizeof(struct fuse_file_info));
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

int copy_release(void *userdata, const char *path,
				 struct fuse_file_info *fi)
{
	// Called during close, but possibly asynchronously.

	DLOG("watdfs_cli_release called for '%s'", path);

	// open has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[1] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"release", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("release rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

// READ AND WRITE DATA
int copy_read(void *userdata, const char *path, char *buf, size_t size,
			  off_t offset, struct fuse_file_info *fi)
{
	// Read size amount of data at offset of file into buf.

	// Remember that size may be greater then the maximum array size of the RPC
	// library.

	DLOG("watdfs_cli_read called for '%s'", path);

	// read has 6 arguments.
	int ARG_COUNT = 6;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)buflen;
	// incorrect: cannot set as buflen, the arg type of buf for each call will be set below
	// args[1] = (void *)buf;

	arg_types[2] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	// args[2] = (void *)&size; //the size for each call will be set below

	arg_types[3] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	// args[3] = (void *)&offset; //the offset for each call will be set below

	// The second argument
	arg_types[4] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[4] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[5] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode = 1; // need to set an integer > 0
	args[5] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[6] = 0;

	int rpc_ret = 0;  // MAKE THE RPC CALL
	int rpcCount = 0; // the times of rpc call
	int filesize = 0; // the actual size of the read file

	if (size <= MAX_ARRAY_LEN)
	{
		DLOG("the read only need one rpc call");
		rpcCount = 1;
		arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)size; // set the new bufsize length
		args[1] = (void *)buf;
		args[2] = (void *)&size;
		args[3] = (void *)&offset;
		rpc_ret = rpcCall((char *)"read", arg_types, args);
		if (rpc_ret == 0)
		{
			filesize = returnCode;
		}
	}
	else
	{
		char *bufCache = (char *)malloc(MAX_ARRAY_LEN); // because buf will be changed at every call, we need bufCache to store the buf from every call
		char *tempBufCache = buf;						// tempBufCache is used to write bufCache from the server to buf, a pointer for loop
		memset(bufCache, 0, sizeof(MAX_ARRAY_LEN));		// reset the bufCache

		int rpcTimes = size / MAX_ARRAY_LEN + 1; // theoretically, the maximun time of rpc call
		DLOG("the read need %d rpc calls", rpcTimes);
		long bufsize = 0;
		long offset_each = offset;
		// rpc_ret indicates a success of rpc, rpc_ret < 0 indicates rpc failure, returnCode == 0 indicates end of file, returnCode < 0 indicates an error
		for (rpcCount = 1; rpcCount <= rpcTimes && rpc_ret == 0 && returnCode > 0; rpcCount++)
		{
			// the first (rpcCount - 1) times, bufsize is MAX_ARRAY_LEN
			if (rpcCount != rpcTimes)
			{
				bufsize = MAX_ARRAY_LEN;
			}
			// the last time
			else
			{
				bufsize = size - MAX_ARRAY_LEN * (rpcCount - 1);
			}
			arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)bufsize; // set the new bufsize length
			offset_each = offset + MAX_ARRAY_LEN * (rpcCount - 1);									   // the offset for each time should change
			args[1] = (void *)bufCache;
			args[2] = (void *)&bufsize;
			args[3] = (void *)&offset_each;
			rpc_ret = rpcCall((char *)"read", arg_types, args);

			if (rpc_ret == 0)
			{
				filesize += returnCode;
				DLOG("filesize: %d ", filesize);
			}
			// memory copy all the data from buf to bufCache for reading, and mempcpy will returns a pointer to the byte following the last written byte
			// we use this pointer to reset the position in bufCache
			tempBufCache = (char *)mempcpy(tempBufCache, bufCache, returnCode);
			DLOG("read rpc with rpc_ret '%d' on %d calls", rpc_ret, rpcCount);
			DLOG("bufCache: %s, bufsize: %ld, offset_each: %ld, returnCode: %d, after call", bufCache, bufsize, offset_each, returnCode);
			// the actual read size is less than the set bufsize, indicating the reading ends
			if (returnCode < bufsize)
			{
				break;
			}
		}
		free(bufCache);
	}

	// the pread fails
	if (returnCode < 0)
	{
		DLOG("the pread on server side failed with: %d", returnCode);
		filesize = returnCode; // for the convenience of setting fxn_ret, the return code will be negative
	}

	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("read rpc failed with error '%d' on %d calls", rpc_ret, rpcCount);
		DLOG("the return code of read: %d", returnCode);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = filesize; // return the actual read size
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;

	// return -ENOSYS;
}

int copy_write(void *userdata, const char *path, const char *buf,
			   size_t size, off_t offset, struct fuse_file_info *fi)
{
	// Write size amount of data at offset of file from buf.

	// Remember that size may be greater then the maximum array size of the RPC
	// library.

	DLOG("watdfs_cli_write called for '%s'", path);

	// read has 6 arguments.
	int ARG_COUNT = 6;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)buflen;
	// incorrect: cannot set as buflen, the arg type of buf for each call will be set below
	// args[1] = (void *)buf;

	arg_types[2] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	// args[2] = (void *)&size; //the size for each call will be set below

	arg_types[3] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	// args[3] = (void *)&offset; //the offset for each call will be set below

	// The second argument
	arg_types[4] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[4] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[5] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode = 1; // need to set an integer > 0
	args[5] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[6] = 0;

	int rpc_ret = 0;  // MAKE THE RPC CALL
	int rpcCount = 0; // the times of rpc call
	int filesize = 0; // the actual size of the read file

	if (size <= MAX_ARRAY_LEN)
	{
		DLOG("the write only need one rpc call");
		rpcCount = 1;
		arg_types[1] = (1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)size; // set the new bufsize length
		args[1] = (void *)buf;
		args[2] = (void *)&size;
		args[3] = (void *)&offset;
		rpc_ret = rpcCall((char *)"write", arg_types, args);
		if (rpc_ret == 0)
		{
			filesize = returnCode;
		}
	}
	else
	{
		char *bufCache = (char *)malloc(MAX_ARRAY_LEN); // because buf will be changed at every call, we need bufCache to store the buf from every call
		memset(bufCache, 0, sizeof(MAX_ARRAY_LEN));		// reset the bufCache
		char *tempBufCache = (char *)malloc(size);		// tempBufCache is a copy of buf, a pointer for loop
		char *freeTempBufCache = tempBufCache;			// the tempBufCache will change after loop, use freeTempBufCache to mark the original pointer
		memcpy(tempBufCache, buf, size);				// memory copy buf to tempBufCache
		DLOG("the tempBufCache: %s before loop", tempBufCache);

		int rpcTimes = size / MAX_ARRAY_LEN + 1; // theoretically, the maximun time of rpc call
		DLOG("the write need %d rpc calls", rpcTimes);
		long bufsize = 0;
		long offset_each = offset;
		// rpc_ret indicates a success of rpc, rpc_ret < 0 indicates rpc failure, returnCode == 0 indicates end of file, returnCode < 0 indicates an error
		for (rpcCount = 1; rpcCount <= rpcTimes && rpc_ret == 0 && returnCode > 0; rpcCount++)
		{
			// the first (rpcCount - 1) times, bufsize is MAX_ARRAY_LEN
			if (rpcCount != rpcTimes)
			{
				bufsize = MAX_ARRAY_LEN;
			}
			// the last time
			else
			{
				bufsize = size - MAX_ARRAY_LEN * (rpcCount - 1);
			}
			arg_types[1] = (1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)bufsize; // set the new bufsize length
			offset_each = offset + MAX_ARRAY_LEN * (rpcCount - 1);									  // the offset for each time should change
			memcpy(bufCache, tempBufCache, bufsize);
			DLOG("the tempBufCache: %s before rpc call", tempBufCache);
			args[1] = (void *)bufCache;
			args[2] = (void *)&bufsize;
			args[3] = (void *)&offset_each;
			rpc_ret = rpcCall((char *)"write", arg_types, args);

			if (rpc_ret == 0)
			{
				filesize += returnCode;
				tempBufCache += bufsize;
				DLOG("filesize: %d ", filesize);
			}
			// memory copy all the data from buf to bufCache for reading, and mempcpy will returns a pointer to the byte following the last written byte
			// we use this pointer to reset the position in bufCache
			DLOG("write rpc with rpc_ret '%d' on %d calls", rpc_ret, rpcCount);
			DLOG("bufCache: %s, bufsize: %ld, offset_each: %ld, returnCode: %d, after call", bufCache, bufsize, offset_each, returnCode);
			// the actual read size is less than the set bufsize, indicating the reading ends
			if (returnCode < bufsize)
			{
				break;
			}
		}
		free(bufCache);
		free(freeTempBufCache);
	}

	// the pread fails
	if (returnCode < 0)
	{
		DLOG("the pwrite on server side failed with: %d", returnCode);
		filesize = returnCode; // for the convenience of setting fxn_ret, the return code will be negative
	}

	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("write rpc failed with error '%d' on %d calls", rpc_ret, rpcCount);
		DLOG("the return code of write: %d", returnCode);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = filesize; // return the actual read size
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;

	// return -ENOSYS;
}

int copy_truncate(void *userdata, const char *path, off_t newsize)
{
	// Change the file size to newsize.

	DLOG("watdfs_cli_truncate called for '%s'", path);

	// truncate has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	args[1] = (void *)&newsize;

	// The fourth argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"truncate", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_getattr will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("truncate rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

int copy_fsync(void *userdata, const char *path,
			   struct fuse_file_info *fi)
{
	// Force a flush of file data.
	DLOG("watdfs_cli_fsync called for '%s'", path);

	// open has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[1] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"fsync", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("fsync rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

// CHANGE METADATA
int copy_utimensat(void *userdata, const char *path,
				   const struct timespec ts[2])
{
	// Change file access and modification times.
	DLOG("watdfs_cli_utimensat called for '%s'", path);

	// open has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)(sizeof(struct timespec) * 2);
	args[1] = (void *)ts;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"utimensat", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("utimensat rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

//--------------------the new implementation of wat_cli functions in A3-------------------
// SETUP AND TEARDOWN
void *watdfs_cli_init(struct fuse_conn_info *conn, const char *path_to_cache,
					  time_t cache_interval, int *ret_code)
{
	DLOG("Initializing client...");
	// TODO: set up the RPC library by calling `rpcClientInit`.
	int initRet = rpcClientInit();

	// TODO: check the return code of the `rpcClientInit` call
	// `rpcClientInit` may fail, for example, if an incorrect port was exported.
	if (initRet < 0)
	{
#ifdef PRINT_ERR
		std::cerr << "Client Initialization Error: " << initRet << std::endl;
#endif
	}

	// It may be useful to print to stderr or stdout during debugging.
	// Important: Make sure you turn off logging prior to submission!
	// One useful technique is to use pre-processor flags like:
#ifdef PRINT_ERR
	std::cerr << "Failed to initialize RPC Client" << std::endl;
#endif
	// Tip: Try using a macro for the above to minimize the debugging code.

	// TODO Initialize any global state that you require for the assignment and return it.
	// The value that you return here will be passed as userdata in other functions.
	// In A1, you might not need it, so you can return `nullptr`.
	struct files_status *userdata = new struct files_status;
	userdata->cacheInterval = cache_interval;
	int str_len = strlen(path_to_cache) + 1;
	userdata->pathToCache = (char *)malloc(str_len);
	strcpy(userdata->pathToCache, path_to_cache);

	// TODO: save `path_to_cache` and `cache_interval` (for A3).

	// TODO: set `ret_code` to 0 if everything above succeeded else some appropriate
	// non-zero value.
	if (initRet < 0)
	{
		*ret_code = NOT_INIT;
	}
	else
	{
		*ret_code = 0;
	}

	// Return pointer to global state data.
	return (void *)userdata;
}

void watdfs_cli_destroy(void *userdata)
{
	// TODO: clean up your userdata state.
	free(((struct files_status *)userdata)->pathToCache);
	free(userdata);
	// TODO: tear down the RPC library by calling `rpcClientDestroy`.
	int destoryRet = rpcClientDestroy();
	if (destoryRet < 0)
	{
		DLOG("Client Destroy failed");
	}
	else
	{
		DLOG("Client Destroy succeeded");
	}
}

// GET FILE ATTRIBUTES
int watdfs_cli_getattr(void *userdata, const char *path, struct stat *statbuf)
{
	// SET UP THE RPC CALL
	DLOG("watdfs_cli_getattr called for '%s'", path);

	// getattr has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument is the stat structure. This argument is an output
	// only argument, and we treat it as a char array. The length of the array
	// is the size of the stat structure, which we can determine with sizeof.
	arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
				   (uint)sizeof(struct stat); // statbuf
	args[1] = (void *)statbuf;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[2] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"getattr", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_getattr will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("getattr rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	if (fxn_ret < 0)
	{
		// If the return code of watdfs_cli_getattr is negative (an error), then
		// we need to make sure that the stat structure is filled with 0s. Otherwise,
		// FUSE will be confused by the contradicting return values.
		memset(statbuf, 0, sizeof(struct stat));
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
}

// CREATE, OPEN AND CLOSE
int watdfs_cli_mknod(void *userdata, const char *path, mode_t mode, dev_t dev)
{
	// Called to create a file.

	DLOG("watdfs_cli_mknod called for '%s'", path);

	// mknod has 4 arguments.
	int ARG_COUNT = 4;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] = (1u << ARG_INPUT) | (ARG_INT << 16u);
	args[1] = (void *)&mode;

	// The third argument
	arg_types[2] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	args[2] = (void *)&dev;

	// The fourth argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[3] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[3] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[4] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"mknod", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_getattr will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("mknod rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;

	// return -ENOSYS;
}

int watdfs_cli_open(void *userdata, const char *path, struct fuse_file_info *fi)
{
	// Called during open.
	// You should fill in fi->fh.

	DLOG("watdfs_cli_open called for '%s'", path);

	struct files_status *filesStatus = (struct files_status *)userdata;

	int originalFlags = fi->flags; // cli_downloadFile function will change the flag

	// open has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument: the fuse_file_info
	arg_types[1] =
		(1u << ARG_INPUT) | (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[1] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	int rpc_ret; // to be determined???
	std::map<std::string, struct file_meta>::iterator it;
	it = filesStatus->openFilesStatus.find(path);
	if (it == filesStatus->openFilesStatus.end())
	{
		// // MAKE THE RPC CALL
		// rpc_ret = rpcCall((char *)"open", arg_types, args);

		int downloadRet = cli_downloadFile(path, filesStatus, fi);
	}

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("open rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	if (fxn_ret < 0)
	{
		// If the return code of watdfs_cli_getattr is negative (an error), then
		// we need to make sure that the stat structure is filled with 0s. Otherwise,
		// FUSE will be confused by the contradicting return values.
		memset(fi, 0, sizeof(struct fuse_file_info));
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

int watdfs_cli_release(void *userdata, const char *path,
					   struct fuse_file_info *fi)
{
	// Called during close, but possibly asynchronously.

	DLOG("watdfs_cli_release called for '%s'", path);

	// open has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[1] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"release", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("release rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

// READ AND WRITE DATA
int watdfs_cli_read(void *userdata, const char *path, char *buf, size_t size,
					off_t offset, struct fuse_file_info *fi)
{
	// Read size amount of data at offset of file into buf.

	// Remember that size may be greater then the maximum array size of the RPC
	// library.

	DLOG("watdfs_cli_read called for '%s'", path);

	// read has 6 arguments.
	int ARG_COUNT = 6;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)buflen;
	// incorrect: cannot set as buflen, the arg type of buf for each call will be set below
	// args[1] = (void *)buf;

	arg_types[2] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	// args[2] = (void *)&size; //the size for each call will be set below

	arg_types[3] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	// args[3] = (void *)&offset; //the offset for each call will be set below

	// The second argument
	arg_types[4] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[4] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[5] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode = 1; // need to set an integer > 0
	args[5] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[6] = 0;

	int rpc_ret = 0;  // MAKE THE RPC CALL
	int rpcCount = 0; // the times of rpc call
	int filesize = 0; // the actual size of the read file

	if (size <= MAX_ARRAY_LEN)
	{
		DLOG("the read only need one rpc call");
		rpcCount = 1;
		arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)size; // set the new bufsize length
		args[1] = (void *)buf;
		args[2] = (void *)&size;
		args[3] = (void *)&offset;
		rpc_ret = rpcCall((char *)"read", arg_types, args);
		if (rpc_ret == 0)
		{
			filesize = returnCode;
		}
	}
	else
	{
		char *bufCache = (char *)malloc(MAX_ARRAY_LEN); // because buf will be changed at every call, we need bufCache to store the buf from every call
		char *tempBufCache = buf;						// tempBufCache is used to write bufCache from the server to buf, a pointer for loop
		memset(bufCache, 0, sizeof(MAX_ARRAY_LEN));		// reset the bufCache

		int rpcTimes = size / MAX_ARRAY_LEN + 1; // theoretically, the maximun time of rpc call
		DLOG("the read need %d rpc calls", rpcTimes);
		long bufsize = 0;
		long offset_each = offset;
		// rpc_ret indicates a success of rpc, rpc_ret < 0 indicates rpc failure, returnCode == 0 indicates end of file, returnCode < 0 indicates an error
		for (rpcCount = 1; rpcCount <= rpcTimes && rpc_ret == 0 && returnCode > 0; rpcCount++)
		{
			// the first (rpcCount - 1) times, bufsize is MAX_ARRAY_LEN
			if (rpcCount != rpcTimes)
			{
				bufsize = MAX_ARRAY_LEN;
			}
			// the last time
			else
			{
				bufsize = size - MAX_ARRAY_LEN * (rpcCount - 1);
			}
			arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)bufsize; // set the new bufsize length
			offset_each = offset + MAX_ARRAY_LEN * (rpcCount - 1);									   // the offset for each time should change
			args[1] = (void *)bufCache;
			args[2] = (void *)&bufsize;
			args[3] = (void *)&offset_each;
			rpc_ret = rpcCall((char *)"read", arg_types, args);

			if (rpc_ret == 0)
			{
				filesize += returnCode;
				DLOG("filesize: %d ", filesize);
			}
			// memory copy all the data from buf to bufCache for reading, and mempcpy will returns a pointer to the byte following the last written byte
			// we use this pointer to reset the position in bufCache
			tempBufCache = (char *)mempcpy(tempBufCache, bufCache, returnCode);
			DLOG("read rpc with rpc_ret '%d' on %d calls", rpc_ret, rpcCount);
			DLOG("bufCache: %s, bufsize: %ld, offset_each: %ld, returnCode: %d, after call", bufCache, bufsize, offset_each, returnCode);
			// the actual read size is less than the set bufsize, indicating the reading ends
			if (returnCode < bufsize)
			{
				break;
			}
		}
		free(bufCache);
	}

	// the pread fails
	if (returnCode < 0)
	{
		DLOG("the pread on server side failed with: %d", returnCode);
		filesize = returnCode; // for the convenience of setting fxn_ret, the return code will be negative
	}

	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("read rpc failed with error '%d' on %d calls", rpc_ret, rpcCount);
		DLOG("the return code of read: %d", returnCode);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = filesize; // return the actual read size
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;

	// return -ENOSYS;
}

int watdfs_cli_write(void *userdata, const char *path, const char *buf,
					 size_t size, off_t offset, struct fuse_file_info *fi)
{
	// Write size amount of data at offset of file from buf.

	// Remember that size may be greater then the maximum array size of the RPC
	// library.

	DLOG("watdfs_cli_write called for '%s'", path);

	// read has 6 arguments.
	int ARG_COUNT = 6;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// arg_types[1] = (1u << ARG_OUTPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)buflen;
	// incorrect: cannot set as buflen, the arg type of buf for each call will be set below
	// args[1] = (void *)buf;

	arg_types[2] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	// args[2] = (void *)&size; //the size for each call will be set below

	arg_types[3] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	// args[3] = (void *)&offset; //the offset for each call will be set below

	// The second argument
	arg_types[4] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[4] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[5] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode = 1; // need to set an integer > 0
	args[5] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[6] = 0;

	int rpc_ret = 0;  // MAKE THE RPC CALL
	int rpcCount = 0; // the times of rpc call
	int filesize = 0; // the actual size of the read file

	if (size <= MAX_ARRAY_LEN)
	{
		DLOG("the write only need one rpc call");
		rpcCount = 1;
		arg_types[1] = (1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)size; // set the new bufsize length
		args[1] = (void *)buf;
		args[2] = (void *)&size;
		args[3] = (void *)&offset;
		rpc_ret = rpcCall((char *)"write", arg_types, args);
		if (rpc_ret == 0)
		{
			filesize = returnCode;
		}
	}
	else
	{
		char *bufCache = (char *)malloc(MAX_ARRAY_LEN); // because buf will be changed at every call, we need bufCache to store the buf from every call
		memset(bufCache, 0, sizeof(MAX_ARRAY_LEN));		// reset the bufCache
		char *tempBufCache = (char *)malloc(size);		// tempBufCache is a copy of buf, a pointer for loop
		char *freeTempBufCache = tempBufCache;			// the tempBufCache will change after loop, use freeTempBufCache to mark the original pointer
		memcpy(tempBufCache, buf, size);				// memory copy buf to tempBufCache
		DLOG("the tempBufCache: %s before loop", tempBufCache);

		int rpcTimes = size / MAX_ARRAY_LEN + 1; // theoretically, the maximun time of rpc call
		DLOG("the write need %d rpc calls", rpcTimes);
		long bufsize = 0;
		long offset_each = offset;
		// rpc_ret indicates a success of rpc, rpc_ret < 0 indicates rpc failure, returnCode == 0 indicates end of file, returnCode < 0 indicates an error
		for (rpcCount = 1; rpcCount <= rpcTimes && rpc_ret == 0 && returnCode > 0; rpcCount++)
		{
			// the first (rpcCount - 1) times, bufsize is MAX_ARRAY_LEN
			if (rpcCount != rpcTimes)
			{
				bufsize = MAX_ARRAY_LEN;
			}
			// the last time
			else
			{
				bufsize = size - MAX_ARRAY_LEN * (rpcCount - 1);
			}
			arg_types[1] = (1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)bufsize; // set the new bufsize length
			offset_each = offset + MAX_ARRAY_LEN * (rpcCount - 1);									  // the offset for each time should change
			memcpy(bufCache, tempBufCache, bufsize);
			DLOG("the tempBufCache: %s before rpc call", tempBufCache);
			args[1] = (void *)bufCache;
			args[2] = (void *)&bufsize;
			args[3] = (void *)&offset_each;
			rpc_ret = rpcCall((char *)"write", arg_types, args);

			if (rpc_ret == 0)
			{
				filesize += returnCode;
				tempBufCache += bufsize;
				DLOG("filesize: %d ", filesize);
			}
			// memory copy all the data from buf to bufCache for reading, and mempcpy will returns a pointer to the byte following the last written byte
			// we use this pointer to reset the position in bufCache
			DLOG("write rpc with rpc_ret '%d' on %d calls", rpc_ret, rpcCount);
			DLOG("bufCache: %s, bufsize: %ld, offset_each: %ld, returnCode: %d, after call", bufCache, bufsize, offset_each, returnCode);
			// the actual read size is less than the set bufsize, indicating the reading ends
			if (returnCode < bufsize)
			{
				break;
			}
		}
		free(bufCache);
		free(freeTempBufCache);
	}

	// the pread fails
	if (returnCode < 0)
	{
		DLOG("the pwrite on server side failed with: %d", returnCode);
		filesize = returnCode; // for the convenience of setting fxn_ret, the return code will be negative
	}

	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("write rpc failed with error '%d' on %d calls", rpc_ret, rpcCount);
		DLOG("the return code of write: %d", returnCode);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = filesize; // return the actual read size
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;

	// return -ENOSYS;
}

int watdfs_cli_truncate(void *userdata, const char *path, off_t newsize)
{
	// Change the file size to newsize.

	DLOG("watdfs_cli_truncate called for '%s'", path);

	// truncate has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] = (1u << ARG_INPUT) | (ARG_LONG << 16u);
	args[1] = (void *)&newsize;

	// The fourth argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"truncate", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_getattr will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("truncate rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

int watdfs_cli_fsync(void *userdata, const char *path,
					 struct fuse_file_info *fi)
{
	// Force a flush of file data.
	DLOG("watdfs_cli_fsync called for '%s'", path);

	// open has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)sizeof(struct fuse_file_info);
	args[1] = (void *)fi;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"fsync", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("fsync rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

// CHANGE METADATA
int watdfs_cli_utimensat(void *userdata, const char *path,
						 const struct timespec ts[2])
{
	// For both calls, the new file timestamps are specified in the array times:
	// times[0] specifies the new "last access time" (atime);
	// times[1] specifies the new "last modification time" (mtime).

	// Change file access and modification times.
	DLOG("watdfs_cli_utimensat called for '%s'", path);

	// open has 3 arguments.
	int ARG_COUNT = 3;

	// Allocate space for the output arguments.
	void **args = new void *[ARG_COUNT];

	// Allocate the space for arg types, and one extra space for the null
	// array element.
	int arg_types[ARG_COUNT + 1];

	// The path has string length (strlen) + 1 (for the null character).
	int pathlen = strlen(path) + 1;

	// Fill in the arguments
	// The first argument is the path, it is an input only argument, and a char
	// array. The length of the array is the length of the path.
	arg_types[0] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) | (uint)pathlen;
	// For arrays the argument is the array pointer, not a pointer to a pointer.
	args[0] = (void *)path;

	// The second argument
	arg_types[1] =
		(1u << ARG_INPUT) | (1u << ARG_ARRAY) | (ARG_CHAR << 16u) |
		(uint)(sizeof(struct timespec) * 2);
	args[1] = (void *)ts;

	// The third argument is the return code, an output only argument, which is
	// an integer.
	// TODO: fill in this argument type.
	arg_types[2] = (1u << ARG_OUTPUT) | (ARG_INT << 16u);

	// The return code is not an array, so we need to hand args[3] an int*.
	// The int* could be the address of an integer located on the stack, or use
	// a heap allocated integer, in which case it should be freed.
	// TODO: Fill in the argument
	int returnCode;
	args[2] = (void *)&returnCode;

	// Finally, the last position of the arg types is 0. There is no
	// corresponding arg.
	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"utimensat", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("utimensat rpc failed with error '%d'", rpc_ret);
		// Something went wrong with the rpcCall, return a sensible return
		// value. In this case lets return, -EINVAL
		fxn_ret = -EINVAL;
	}
	else
	{
		// Our RPC call succeeded. However, it's possible that the return code
		// from the server is not 0, that is it may be -errno. Therefore, we
		// should set our function return value to the retcode from the server.

		// TODO: set the function return value to the return code from the server.
		fxn_ret = returnCode;
	}

	// Clean up the memory we have allocated.
	delete[] args;

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

int cli_downloadFile(const char *path, struct files_status *filesStatus, struct fuse_file_info *fi)
{
	int originalFlags = fi->flags;
	int readFlag = O_RDONLY;
	// step 0: getattr to get file size
	struct fuse_file_info *fi = new struct fuse_file_info;
	struct stat *statbuf = new struct stat;

	int getattrRet = copy_getattr((void *)filesStatus, path, statbuf);
	if (getattrRet < 0)
	{
		DLOG("server file getattr fail");
		delete statbuf;
		delete fi;
		return getattrRet;
	}
	size_t size = statbuf->st_size;

	// step 1: create file
	int short_path_len = strlen(path);
	int dir_len = strlen(filesStatus->pathToCache);
	int full_len = dir_len + short_path_len + 1;
	char *full_path = (char *)malloc(full_len);
	strcpy(full_path, filesStatus->pathToCache);
	strcat(full_path, path);
	DLOG("The new file full path on client: %s\n", full_path);
	// create file and get the file handler
	int createRet = creat(full_path, 0);
	if (createRet < 0)
	{
		DLOG("local file create fail");
		delete statbuf;
		delete fi;
		free(full_path);
		return -errno;
	}

	// step 2: truncate file
	int truncateRet = truncate(full_path, (off_t)size);
	if (truncateRet < 0)
	{
		DLOG("local file truncate fail");
		delete statbuf;
		delete fi;
		free(full_path);
		return -errno;
	}

	// step 3: read from server file
	char *buf = (char *)malloc(((off_t)size) * sizeof(char));
	fi->flags = readFlag; // open with read only mode
	// open file
	int openRet = copy_open((void *)filesStatus, path, fi);
	if (openRet < 0)
	{
		DLOG("server file open fail");
		delete statbuf;
		delete fi;
		free(buf);
		free(full_path);
		return openRet;
	}
	// read file
	int readRet = copy_read((void *)filesStatus, path, buf, size, 0, fi);
	if (readRet < 0)
	{
		DLOG("server file read fail");
		delete statbuf;
		delete fi;
		free(buf);
		free(full_path);
		return readRet;
	}

	// step 4: write local file
	int writeRet = pwrite(createRet, buf, size, 0);
	if (writeRet < 0)
	{
		DLOG("server file read fail");
		delete statbuf;
		delete fi;
		free(buf);
		free(full_path);
		return -errno;
	}

	// step 5: update metadata
	// use statbuf->st_mtim to achieve T_client == T_server, set tc as current time
	struct file_meta newFile = {createRet, originalFlags, statbuf->st_mtim, time(0)};
	// insert newFile to openFilesStatus
	filesStatus->openFilesStatus[path] =  newFile;
	//release the server file
	int releaseRet = copy_release((void *)filesStatus, path, fi);
	if (releaseRet < 0)
	{
		DLOG("server file releaseRet fail");
		delete statbuf;
		delete fi;
		free(buf);
		free(full_path);
		return releaseRet;
	}

	DLOG("download succeed");
	delete statbuf;
	delete fi;
	free(buf);
	free(full_path);
	return 0;
}
