//
// Starter code for CS 454/654
// You SHOULD change this file
//
// #define PRINT_ERR 1

#include "watdfs_client.h"
#include "debug.h"
INIT_LOG

#include "rpc.h"
#include <iostream>
#include <map>
#include "rw_lock.h"

struct file_meta
{
	u_int64_t clientDesc;
	int clientMode;
	time_t tc;
	u_int64_t serverDesc;
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
	DLOG("copy_getattr called for '%s'", path);

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

	DLOG("copy_mknod called for '%s'", path);

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

	DLOG("copy_open called for '%s'", path);

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

	DLOG("copy_release called for '%s'", path);

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

	DLOG("copy_read called for '%s'", path);

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

	DLOG("copy_write called for '%s'", path);

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

	DLOG("copy_truncate called for '%s'", path);

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
	DLOG("copy_fsync called for '%s'", path);

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
	DLOG("copy_utimensat called for '%s'", path);

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
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//------------------------lock functions---------------------
int watdfs_cli_lock(const char *path, rw_lock_mode_t mode)
{

	int ARG_COUNT = 3;

	void **args = (void **)malloc(ARG_COUNT * sizeof(void *));

	int arg_types[ARG_COUNT + 1];

	int pathlen = strlen(path) + 1;

	int returnCode;

	arg_types[0] = (1 << ARG_INPUT) | (1 << ARG_ARRAY) | (ARG_CHAR << 16u) | pathlen;
	args[0] = (void *)path;

	arg_types[1] = (1 << ARG_INPUT) | (ARG_INT << 16u);
	args[1] = (void *)&mode;

	arg_types[2] = (1 << ARG_OUTPUT) | (ARG_INT << 16u);
	args[2] = (void *)&returnCode;

	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"lock", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("lock rpc failed with error '%d'", rpc_ret);
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
}

int watdfs_cli_unlock(const char *path, rw_lock_mode_t mode)
{

	int ARG_COUNT = 3;

	void **args = (void **)malloc(ARG_COUNT * sizeof(void *));

	int arg_types[ARG_COUNT + 1];

	int pathlen = strlen(path) + 1;

	int returnCode;

	arg_types[0] = (1 << ARG_INPUT) | (1 << ARG_ARRAY) | (ARG_CHAR << 16u) | pathlen;
	args[0] = (void *)path;

	arg_types[1] = (1 << ARG_INPUT) | (ARG_INT << 16u);
	args[1] = (void *)&mode;

	arg_types[2] = (1 << ARG_OUTPUT) | (ARG_INT << 16u);
	args[2] = (void *)&returnCode;

	arg_types[3] = 0;

	// MAKE THE RPC CALL
	int rpc_ret = rpcCall((char *)"unlock", arg_types, args);

	// HANDLE THE RETURN
	// The integer value watdfs_cli_open will return.
	int fxn_ret = 0;
	if (rpc_ret < 0)
	{
		DLOG("unlock rpc failed with error '%d'", rpc_ret);
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
}
//
//
//
//
//--------------------------util functions--------------------------------

char *get_full_path(const char *short_path, struct files_status *filesStatus)
{
	int short_path_len = strlen(short_path);
	int dir_len = strlen(filesStatus->pathToCache);
	int full_len = dir_len + short_path_len + 1;

	char *full_path = (char *)malloc(full_len);

	// First fill in the directory.
	strcpy(full_path, filesStatus->pathToCache);
	// Then append the path.
	strcat(full_path, short_path);
	DLOG("Full path: %s\n", full_path);

	return full_path;
}

// open return 0, not open return -1
int cli_file_is_open(const char *path, struct files_status *filesStatus)
{
	if (filesStatus->openFilesStatus.find(path) == filesStatus->openFilesStatus.end())
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

// true return 0, false return -1
int cli_freshness_check(const char *path, struct files_status *filesStatus)
{
	DLOG("checking freshness...");

	int fxn_ret = 0;

	char *full_path = get_full_path(path, filesStatus);
	int isOpen = cli_file_is_open(path, filesStatus);

	time_t t = filesStatus->cacheInterval;
	time_t T = time(0);

	// if the file is not opened, don't check check the T - tc < t, because it doesn't have tc
	if (isOpen < 0)
	{
		DLOG("checking T_client == T_server");
		struct stat *statClient = new struct stat;
		struct stat *statServer = new struct stat;

		DLOG("get the stat of local file...");
		int statRet = stat(full_path, statClient);
		if (statRet < 0)
		{
			DLOG("the local file stat fail");
			fxn_ret = -errno;
		}

		DLOG("get the stat of server file...");
		int getattrRet = copy_getattr(filesStatus, path, statServer);
		if (getattrRet < 0)
		{
			free(full_path);
			free(statClient);
			free(statServer);
			return getattrRet;
		}

		time_t T_client = statClient->st_mtime;
		time_t T_server = statServer->st_mtime;

		if (T_client != T_server)
		{
			DLOG("the file freshness check fail");
			fxn_ret = -1;
		}
		else
		{
			DLOG("the file satisfy T_client == T_server");
		}

		free(full_path);
		free(statClient);
		free(statServer);
	}
	// if the file is opened, check the T - tc < t
	else
	{
		struct file_meta fileMeta = filesStatus->openFilesStatus[path];
		// if T - tc < t, do not need to check T_client == T_server
		if (T - fileMeta.tc < t)
		{
			free(full_path);
			DLOG("the file satisfy T - tc < t");
			return 0;
		}
		// if T - tc >= t, check T_client == T_server
		else
		{
			DLOG("the file not satisfy T - tc < t");
			DLOG("checking T_client == T_server");
			struct stat *statClient = new struct stat;
			struct stat *statServer = new struct stat;

			DLOG("get the stat of local file...");
			int statRet = stat(full_path, statClient);
			if (statRet < 0)
			{
				DLOG("the local file stat fail");
				fxn_ret = -errno;
			}

			DLOG("get the stat of server file...");
			int getattrRet = copy_getattr((void *)filesStatus, path, statServer);
			if (getattrRet < 0)
			{
				free(full_path);
				free(statClient);
				free(statServer);
				return getattrRet;
			}

			time_t T_client = statClient->st_mtime;
			time_t T_server = statServer->st_mtime;

			if (T_client != T_server)
			{
				DLOG("the file freshness check fail");
				fxn_ret = -1;
			}
			else
			{
				DLOG("the file satisfy T_client == T_server");
			}

			free(full_path);
			free(statClient);
			free(statServer);
		}
	}

	DLOG("checking freshness end");

	return fxn_ret;
}

int cli_downloadFile(const char *path, struct files_status *filesStatus, int originalFlags)
{
	DLOG("cli_downloadFile called for '%s'", path);
	// step 0: getattr to get file size
	struct fuse_file_info *cpyfi = new struct fuse_file_info;
	struct stat *statbuf = new struct stat;

	int getattrRet = copy_getattr((void *)filesStatus, path, statbuf);
	if (getattrRet < 0)
	{
		DLOG("server file getattr fail");
		delete statbuf;
		delete cpyfi;
		return getattrRet;
	}
	size_t size = statbuf->st_size;

	// step 1: create file
	char *full_path = get_full_path(path, filesStatus);
	DLOG("The new file full path on client: %s\n", full_path);
	// create file and get the file handler
	u_int64_t createRet = open(full_path, O_CREAT | O_RDWR, S_IRWXU);
	if (createRet < 0)
	{
		DLOG("local file create fail");
		delete statbuf;
		delete cpyfi;
		free(full_path);
		return -errno;
	}

	DLOG("The new file size on client: %ld\n", size);

	// step 2: truncate file
	int truncateRet = truncate(full_path, 0);
	if (truncateRet < 0)
	{
		DLOG("local file truncate fail");
		delete statbuf;
		delete cpyfi;
		free(full_path);
		return -errno;
	}

	// step 3: read from server file
	char *buf = (char *)malloc(((off_t)size) * sizeof(char));
	cpyfi->flags = O_RDONLY; // open with read only mode
	// open file
	int openRet = copy_open((void *)filesStatus, path, cpyfi);
	if (openRet < 0)
	{
		DLOG("server file open fail");
		delete statbuf;
		delete cpyfi;
		free(buf);
		free(full_path);
		return openRet;
	}

	int lock_ret = watdfs_cli_lock(path, RW_READ_LOCK);
	if (lock_ret < 0)
	{
		DLOG("read lock fail");
		delete statbuf;
		delete cpyfi;
		free(full_path);
		return lock_ret;
	}
	DLOG("---------read lock here---------");

	// read file
	int readRet = copy_read((void *)filesStatus, path, buf, size, 0, cpyfi);
	if (readRet < 0)
	{
		DLOG("server file read fail");
		delete statbuf;
		delete cpyfi;
		free(buf);
		free(full_path);
		return readRet;
	}

	// step 4: write local file
	if (size > 0)
	{
		int writeRet = pwrite(createRet, buf, size, 0);
		if (writeRet < 0)
		{
			DLOG("server file read fail");
			delete statbuf;
			delete cpyfi;
			free(buf);
			free(full_path);
			return -errno;
		}
	}

	// step 5: update metadata
	// use statbuf->st_mtim to achieve T_client == T_server, set tc as current time
	struct file_meta newFile = {createRet, originalFlags, time(0)};
	struct timespec ts[2];
	ts[0] = (struct timespec)(statbuf->st_atim);
	ts[1] = (struct timespec)(statbuf->st_mtim);
	int sys_ret = utimensat(createRet, full_path, ts, 0);
	if (sys_ret < 0)
	{
		DLOG("the utimensat fails");
		delete statbuf;
		delete cpyfi;
		free(buf);
		free(full_path);
		return -errno;
	}

	int unlock_ret = watdfs_cli_unlock(path, RW_READ_LOCK);
	if (unlock_ret < 0)
	{
		DLOG("read unlock fail");
		delete statbuf;
		delete cpyfi;
		free(buf);
		free(full_path);
		return unlock_ret;
	}
	DLOG("---------read unlock here---------");

	// release the server file
	int releaseRet = copy_release((void *)filesStatus, path, cpyfi);
	if (releaseRet < 0)
	{
		DLOG("server file releaseRet fail");
		delete statbuf;
		delete cpyfi;
		free(buf);
		free(full_path);
		return releaseRet;
	}

	// insert newFile to openFilesStatus
	filesStatus->openFilesStatus[path] = newFile;

	DLOG("cli_downloadFile succeed");
	delete statbuf;
	delete cpyfi;
	free(buf);
	free(full_path);
	return 0;
}

int cli_write_back(const char *path, struct files_status *filesStatus)
{
	int fxn_ret = 0;

	DLOG("write back begin...");

	// TODO: lock the file
	int lock_ret = watdfs_cli_lock(path, RW_WRITE_LOCK);
	if (lock_ret < 0)
	{
		DLOG("write lock fail");
		fxn_ret = lock_ret;
		return fxn_ret;
	}
	DLOG("---------write lock here---------");

	// open file on the server
	struct fuse_file_info *fi = new struct fuse_file_info;
	fi->flags = O_RDWR;
	fi->fh = filesStatus->openFilesStatus[path].serverDesc;

	// get the size of local file
	char *full_path = get_full_path(path, filesStatus);
	struct stat *statbuf = new struct stat;
	fxn_ret = stat(full_path, statbuf);
	if (fxn_ret < 0)
	{
		DLOG("local file stat fail");
		fxn_ret = -errno;
		delete statbuf;
		delete fi;
		free(full_path);
		return fxn_ret;
	}
	size_t fileSize = statbuf->st_size;
	char *buf = (char *)malloc(fileSize * sizeof(char));
	DLOG("the local file size is: %ld, buf before pread: %s", fileSize, buf);

	// Read from file to buf.
	// int local_fd = filesStatus->openFilesStatus[path].clientDesc;
	int openRet = open(full_path, O_RDONLY);

	fxn_ret = pread(openRet, buf, fileSize, 0);
	if (fxn_ret < 0)
	{
		DLOG("local file read fail");
		fxn_ret = -errno;
		delete statbuf;
		delete fi;
		free(full_path);
		free(buf);
		close(openRet);
		return fxn_ret;
	}
	close(openRet);
	DLOG("the read buf is: %s", buf);

	// truncate the file
	fxn_ret = copy_truncate(filesStatus, path, 0);
	if (fxn_ret < 0)
	{
		DLOG("server file truncate fail");
		delete statbuf;
		delete fi;
		free(full_path);
		free(buf);
		return fxn_ret;
	}

	// write back server
	DLOG("the server file fi->fh before copy_write: %ld", fi->fh);
	DLOG("path: %s, buf: %s, bufsize: %ld:", path, buf, fileSize);
	if (fileSize > 0)
	{
		fxn_ret = copy_write(filesStatus, path, buf, fileSize, 0, fi);
		if (fxn_ret < 0)
		{
			DLOG("server file write fail");
			delete statbuf;
			delete fi;
			free(full_path);
			free(buf);
			return fxn_ret;
		}
	}

	// update metadata
	struct timespec *ts = new struct timespec[2];
	ts[0] = statbuf->st_atim;
	ts[1] = statbuf->st_mtim;

	fxn_ret = copy_utimensat(filesStatus, path, ts);
	if (fxn_ret < 0)
	{
		DLOG("server file utimensat fail");
		delete statbuf;
		delete fi;
		free(full_path);
		free(buf);
		return fxn_ret;
	}

	// TODO: unlock the file
	int unlock_ret = watdfs_cli_unlock(path, RW_WRITE_LOCK);
	if (unlock_ret < 0)
	{
		DLOG("write unlock fail");
		fxn_ret = unlock_ret;
		delete statbuf;
		delete fi;
		free(full_path);
		free(buf);
		return fxn_ret;
	}
	DLOG("---------write unlock here---------");

	DLOG("write back succeed");
	delete statbuf;
	delete fi;
	free(full_path);
	free(buf);
	return fxn_ret;
}

int cli_close_file(const char *path, struct files_status *filesStatus)
{
	int closeRet = close(filesStatus->openFilesStatus[path].clientDesc);
	if (closeRet < 0)
	{
		DLOG("the local file close fail");
		closeRet = -errno;
		return closeRet;
	}
	filesStatus->openFilesStatus.erase(path);
	DLOG("the local file has been closed");
	return closeRet;
}

//----------------------watdfs_cli functions-----------------------------------
//
//
//
//
//
//
//
//
//
//
//
//

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
	// #ifdef PRINT_ERR
	// 	std::cerr << "Failed to initialize RPC Client" << std::endl;
	// #endif
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

	int callFlag = O_RDONLY;

	struct files_status *filesStatus = (struct files_status *)userdata;

	char *full_path = get_full_path(path, filesStatus);

	int fxn_ret = 0;
	int fileIsOpen = cli_file_is_open(path, filesStatus);

	struct stat serverStatbuf;
	int cpyStatRet = copy_getattr(filesStatus, path, &serverStatbuf);
	// the server file not exist
	if (cpyStatRet < 0)
	{
		if (cpyStatRet == -ENOENT)
		{
			DLOG("the server file not exist");
			free(full_path);
			return cpyStatRet;
		}
		else
		{
			DLOG("server getattr fail");
			free(full_path);
			return cpyStatRet;
		}
	}

	// server file and client file exists

	if (fileIsOpen == 0)
	{
		DLOG("client file is open");
		// client file is open
		int fileClientMode = filesStatus->openFilesStatus[path].clientMode & O_ACCMODE; // use O_ACCMODE to get the last two flag bits

		if (fileClientMode == O_RDONLY)
		{
			DLOG("client file open in read only");
			// file open in read only mode
			int fresh_check = cli_freshness_check(path, filesStatus);

			if (fresh_check == 0)
			{
				DLOG("client file is fresh, do local stat, update Tc");
				// file is fresh, do local stat, update Tc
				int statRet = stat(full_path, statbuf);
				if (statRet < 0)
				{
					DLOG("the local file stat fail");
					fxn_ret = -errno;
				}
				filesStatus->openFilesStatus[path].tc = time(0);
				// return fxn_ret;
			}
			else
			{
				DLOG("client file is not fresh, download file, update file_meta, do local stat");
				// download the file, open with O_RDONLY flag
				int download_ret = cli_downloadFile(path, filesStatus, callFlag);
				// if download failed
				if (download_ret < 0)
				{
					DLOG("download fail");
					fxn_ret = download_ret;
				}
				// if download succeeded
				else
				{
					DLOG("download succeed");
					// do the stat call
					int statRet = stat(full_path, statbuf);
					if (statRet < 0)
					{
						DLOG("the local file stat fail");
						fxn_ret = -errno;
					}
				}
				// return fxn_ret;
			}
		}
		else
		{
			DLOG("client file is open in read-write or write only mode, only do local stat");
			// file is open in read-write or write only mode, only writer, do local stat
			int statRet = stat(full_path, statbuf);
			if (statRet < 0)
			{
				DLOG("the local file stat fail");
				fxn_ret = -errno;
			}
			// return fxn_ret;
		}
	}
	else
	{
		DLOG("client file exist but is not open, open and copy file, do local stat, close file");
		// file exist but is not open, open and copy file, do local stat, close file

		// if freshness is expired...
		int check = cli_freshness_check(path, filesStatus);
		if (check < 0)
		{
			fxn_ret = cli_downloadFile(path, filesStatus, callFlag);
			if (fxn_ret < 0)
			{
				DLOG("fail to fetch server file");
				free(full_path);
				return fxn_ret;
			}

			// close the file
			int closeRet = cli_close_file(path, filesStatus);
			fxn_ret = closeRet;
		}
		// do the stat call
		int statRet = stat(full_path, statbuf);
		if (statRet < 0)
		{
			DLOG("the local file stat fail");
			fxn_ret = -errno;
		}
		// filesStatus->openFilesStatus[path].tc = time(NULL);
	}

	if (fxn_ret < 0)
	{
		// If the return code of watdfs_cli_getattr is negative (an error), then
		// we need to make sure that the stat structure is filled with 0s. Otherwise,
		// FUSE will be confused by the contradicting return values.
		memset(statbuf, 0, sizeof(struct stat));
	}

	free(full_path);

	int isOpen = cli_file_is_open(path, filesStatus);
	if (isOpen < 0)
	{
		DLOG("the file is not opened");
	}
	else
	{
		DLOG("the file is opened");
	}

	// Finally return the value we got from the server.
	return fxn_ret;
}

// CREATE, OPEN AND CLOSE
int watdfs_cli_mknod(void *userdata, const char *path, mode_t mode, dev_t dev)
{
	// Called to create a file.

	DLOG("watdfs_cli_mknod called for '%s'", path);

	int fxn_ret = 0;

	struct files_status *filesStatus = (struct files_status *)userdata;

	char *full_path = get_full_path(path, filesStatus);

	int fileIsOpen = cli_file_is_open(path, filesStatus);
	// the file has not yet opened
	if (fileIsOpen < 0)
	{
		struct stat clientStatbuf;
		int statRet = stat(full_path, &clientStatbuf);

		// if the file is not cached
		if (statRet < 0)
		{
			// the local file not exist
			if (-errno == -ENOENT)
			{
				DLOG("client file not exist");
				// check server file existence
				struct stat serverStatbuf;
				int cpyStatRet = copy_getattr(filesStatus, path, &serverStatbuf);
				// server file not exist
				if (cpyStatRet == -ENOENT)
				{
					DLOG("server file not exist, mknod on client and server, write back to server");

					int ret = mknod(full_path, mode, dev);
					if (ret < 0)
					{
						DLOG("local mknod fail");
						free(full_path);
						return -errno;
					}
					int cpyret = copy_mknod(filesStatus, path, mode, dev);
					if (cpyret < 0)
					{
						DLOG("server mknod fail");
						free(full_path);
						return cpyret;
					}
					// fxn_ret = cli_write_back(path, filesStatus);
					// update metadata
					int statRet = stat(full_path, &clientStatbuf);
					if (statRet < 0)
					{
						DLOG("local file stat fail");
						free(full_path);
						return fxn_ret;
					}
					struct timespec *ts = new struct timespec[2];
					ts[0] = clientStatbuf.st_atim;
					ts[1] = clientStatbuf.st_mtim;

					fxn_ret = copy_utimensat(filesStatus, path, ts);
					if (fxn_ret < 0)
					{
						DLOG("server file utimensat fail");
						free(full_path);
						return fxn_ret;
					}
				}
				// server file exist
				else if (cpyStatRet == 0)
				{
					DLOG("server file exist, download the file and call mknod");
					int dl_ret = cli_downloadFile(path, filesStatus, O_RDWR);
					if (dl_ret < 0)
					{
						DLOG("mknod: download file fail");
						fxn_ret = dl_ret;
						free(full_path);
						return fxn_ret;
					}
					else
					{
						int mkRet = mknod(full_path, mode, dev);
						fxn_ret = mkRet;
						// close the file after download
						cli_close_file(path, filesStatus);
					}
				}
				else
				{
					DLOG("server getattr fail");
					fxn_ret = cpyStatRet;
					free(full_path);
					return fxn_ret;
				}
			}
			// the stat fail
			else
			{
				DLOG("local stat fail");
				free(full_path);
				return -errno;
			}
		}
		// if the file is cached
		else
		{
			DLOG("client file exist");
			int mkRet = mknod(full_path, mode, dev);
			fxn_ret = mkRet;
		}
	}
	// the file has been opened
	else
	{
		if ((filesStatus->openFilesStatus[path].clientMode & O_ACCMODE) == O_RDONLY)
		{
			DLOG("the file is read only and cannot mknod");
			free(full_path);
			return -EPERM;
		}
		else
		{
			DLOG("the file is write only or read&write");
			int mkRet = mknod(full_path, mode, dev);
			fxn_ret = mkRet;
		}
	}

	// TODO: Check freshness.
	int check = cli_freshness_check(path, filesStatus);
	if (check < 0)
	{
		DLOG("mknod: write back to server");
		fxn_ret = cli_write_back(path, filesStatus);
	}

	if (fxn_ret < 0)
	{
		DLOG("watdfs_cli_mknod fail");
	}
	else
	{
		DLOG("watdfs_cli_mknod succeed");
	}

	free(full_path);
	// Finally return the value we got from the server.
	return fxn_ret;

	// return -ENOSYS;
}

int watdfs_cli_open(void *userdata, const char *path, struct fuse_file_info *fi)
{
	// Called during open.
	// You should fill in fi->fh.

	DLOG("watdfs_cli_open called for '%s'", path);

	int callFlag = fi->flags;
	int fileClientMode = callFlag & O_ACCMODE;
	if (fileClientMode == O_RDONLY)
	{
		DLOG("read only flag");
	}
	else if (fileClientMode == O_WRONLY)
	{
		DLOG("write only flag");
	}
	else if (fileClientMode == O_RDWR)
	{
		DLOG("read and write flag");
	}

	struct files_status *filesStatus = (struct files_status *)userdata;
	int fxn_ret = 0;

	// open the server file
	int serOpen = copy_open(filesStatus, path, fi);
	if (serOpen < 0)
	{
		DLOG("server file open error");
		return serOpen;
	}
	else
	{
		DLOG("server file open succeed");
	}

	char *full_path = get_full_path(path, filesStatus);

	int fileIsOpen = cli_file_is_open(path, filesStatus);

	// the file has not yet opened
	if (fileIsOpen < 0)
	{
		DLOG("the file has not yet been opened");
		struct stat *statbuf = new struct stat;
		int statRet = stat(full_path, statbuf);

		// if the file is not cached
		if (statRet < 0)
		{
			if (-errno == -ENOENT)
			{
				DLOG("the file is not cached, downloading...");
				// download the file, open with fi->flags flag
				int download_ret = cli_downloadFile(path, filesStatus, callFlag);
				// if download failed
				if (download_ret < 0)
				{
					DLOG("download fail");
					fxn_ret = download_ret;
				}
				// if download succeeded
				else
				{
					DLOG("download succeed");
					// do not need the open call, return the file descriptor in the map
					// fi->fh = filesStatus->openFilesStatus[path].clientDesc;
				}
			}
			else
			{
				DLOG("the stat call fail");
				fxn_ret = -errno;
			}
		}
		// the file has been cached
		else
		{
			DLOG("the file is cached, check the freshness...");
			// TODO: check freshness

			// if freshness is expired...
			int check = cli_freshness_check(path, filesStatus);
			if (check < 0)
			{
				fxn_ret = cli_downloadFile(path, filesStatus, callFlag);
				if (fxn_ret < 0)
				{
					DLOG("fail to fetch server file");
					free(full_path);
					return fxn_ret;
				}
				// fi->fh = filesStatus->openFilesStatus[path].clientDesc;
			}
			// if freshness is still valid
			else
			{
				u_int64_t openRet = open(full_path, callFlag);

				if (openRet < 0)
				{
					DLOG("the local file open fail");
					fxn_ret = -errno;
				}
				else
				{
					// fi->fh = openRet;
					struct file_meta newFile = {openRet, callFlag, time(0)};
					newFile.serverDesc = fi->fh;
					filesStatus->openFilesStatus[path] = newFile;
				}
			}
		}
		delete statbuf;
	}
	// the file has opened
	else
	{
		DLOG("the file has been opened");
		fxn_ret = -EMFILE;
	}

	DLOG("the server file descriptor: %ld", fi->fh);
	DLOG("the client file descriptor: %ld", filesStatus->openFilesStatus[path].clientDesc);

	if (fxn_ret < 0)
	{
		// If the return code of watdfs_cli_getattr is negative (an error), then
		// we need to make sure that the stat structure is filled with 0s. Otherwise,
		// FUSE will be confused by the contradicting return values.
		memset(fi, 0, sizeof(struct fuse_file_info));
	}

	free(full_path);
	int isOpen = cli_file_is_open(path, filesStatus);
	if (isOpen < 0)
	{
		DLOG("the file is not opened");
	}
	else
	{
		DLOG("the file is opened");
	}

	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

int watdfs_cli_release(void *userdata, const char *path,
					   struct fuse_file_info *fi)
{
	// Called during close, but possibly asynchronously.

	DLOG("watdfs_cli_release called for '%s'", path);

	int fxn_ret = 0;

	struct files_status *filesStatus = (struct files_status *)userdata;

	// If open in write mode, flush to server.
	if ((fi->flags & O_ACCMODE) != O_RDONLY)
	{
		fxn_ret = cli_write_back(path, filesStatus);
		if (fxn_ret < 0)
		{
			return fxn_ret;
		}
	}
	// close the server file
	int serClose = copy_release(filesStatus, path, fi);
	if (serClose < 0)
	{
		DLOG("server file close error");
		fxn_ret = serClose;
	}

	// close the local file
	int closeRet = cli_close_file(path, filesStatus);
	fxn_ret = closeRet;

	int isOpen = cli_file_is_open(path, filesStatus);
	if (isOpen < 0)
	{
		DLOG("the file is not opened");
	}
	else
	{
		DLOG("the file is opened");
	}

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

	struct files_status *filesStatus = (struct files_status *)userdata;
	char *full_path = get_full_path(path, filesStatus);
	int fileClientMode = filesStatus->openFilesStatus[path].clientMode & O_ACCMODE;

	int fxn_ret = 0;
	int fileIsOpen = cli_file_is_open(path, filesStatus);
	// the file has not yet opened
	if (fileIsOpen < 0)
	{
		DLOG("the file is not opened, error in read");
		free(full_path);
		return -EPERM;
	}
	// the file has opened
	else
	{
		DLOG("the file has been opened");
		// the file is write only or write&read
		if (fileClientMode != O_RDONLY)
		{
			DLOG("the file is write only or write&read");
			fxn_ret = pread(filesStatus->openFilesStatus[path].clientDesc, buf, size, offset);
			if (fxn_ret < 0)
			{
				DLOG("the pread fail");
				free(full_path);
				return -errno;
			}
		}
		// the file is read only
		else
		{
			DLOG("the file is read only");
			int check = cli_freshness_check(path, filesStatus);
			if (check < 0)
			{
				int downloadRet = cli_downloadFile(path, filesStatus, filesStatus->openFilesStatus[path].clientMode);
				if (downloadRet < 0)
				{
					DLOG("the download fail");
					free(full_path);
					return -EPERM;
				}
			}
			fxn_ret = pread(filesStatus->openFilesStatus[path].clientDesc, buf, size, offset);
		}
	}

	free(full_path);

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

	struct files_status *filesStatus = (struct files_status *)userdata;
	char *full_path = get_full_path(path, filesStatus);
	int fileClientMode = filesStatus->openFilesStatus[path].clientMode & O_ACCMODE;

	int fxn_ret = 0;
	int fileIsOpen = cli_file_is_open(path, filesStatus);
	// the file has not yet opened
	if (fileIsOpen < 0)
	{
		DLOG("the file is not opened, error in write");
		free(full_path);
		return -EPERM;
	}
	// the file has opened
	else
	{
		DLOG("the file has been opened");
		// the file is write only or write&read
		if (fileClientMode != O_RDONLY)
		{
			fxn_ret = pwrite(filesStatus->openFilesStatus[path].clientDesc, buf, size, offset);
			if (fxn_ret < 0)
			{
				DLOG("the local pwrite fail");
				free(full_path);
				return -errno;
			}
			filesStatus->openFilesStatus[path].tc = time(0);
			// check the freshness
			int check = cli_freshness_check(path, filesStatus);
			if (check == 0)
			{
				DLOG("don't need to write back");
				// return fxn_ret;
			}
			else
			{
				DLOG("need to write back");
				fxn_ret = cli_write_back(path, filesStatus);
			}
		}
		// the file is read only
		else
		{
			DLOG("the file is read only");
			DLOG("the file is not allowed to write, error in write");
			free(full_path);
			// return -EPERM;
			return -EMFILE;
		}
	}

	if (fxn_ret < 0)
	{
		DLOG("watdfs_cli_write fail");
	}
	free(full_path);

	// Finally return the value we got from the server.
	return fxn_ret;

	// return -ENOSYS;
}

int watdfs_cli_truncate(void *userdata, const char *path, off_t newsize)
{
	// Change the file size to newsize.

	DLOG("watdfs_cli_truncate called for '%s'", path);

	int fxn_ret = 0;

	struct files_status *filesStatus = (struct files_status *)userdata;

	char *full_path = get_full_path(path, filesStatus);

	int fileIsOpen = cli_file_is_open(path, filesStatus);
	// the file has not yet opened
	if (fileIsOpen < 0)
	{
		struct stat clientStatbuf;
		int statRet = stat(full_path, &clientStatbuf);

		// if the file is not cached
		if (statRet < 0)
		{
			// the local file not exist
			if (-errno == -ENOENT)
			{
				DLOG("client file not exist");
				int dl_ret = cli_downloadFile(path, filesStatus, O_RDWR);
				if (dl_ret < 0)
				{
					DLOG("truncate: download file fail");
					fxn_ret = dl_ret;
					free(full_path);
					return fxn_ret;
				}
				else
				{
					int mkRet = truncate(full_path, newsize);
					fxn_ret = mkRet;
					// close the file after download
					cli_close_file(path, filesStatus);
				}
			}
			// the stat fail
			else
			{
				DLOG("local stat fail");
				free(full_path);
				return -errno;
			}
		}
		// if the file is cached
		else
		{
			DLOG("client file exist");
			int mkRet = truncate(full_path, newsize);
			fxn_ret = mkRet;
		}
	}
	// the file has been opened
	else
	{
		if ((filesStatus->openFilesStatus[path].clientMode & O_ACCMODE) == O_RDONLY)
		{
			DLOG("the file is read only and cannot truncate");
			free(full_path);
			// return -EPERM;
			return -EMFILE;
		}
		else
		{
			DLOG("the file is write only or read&write, truncate the file on client");
			int mkRet = truncate(full_path, newsize);
			fxn_ret = mkRet;
		}
	}

	// TODO: Check freshness.
	int check = cli_freshness_check(path, filesStatus);
	if (check < 0)
	{
		DLOG("truncate: write back to server");
		fxn_ret = cli_write_back(path, filesStatus);
	}

	if (fxn_ret < 0)
	{
		DLOG("watdfs_cli_truncate fail");
	}
	else
	{
		DLOG("watdfs_cli_truncate succeed");
	}

	free(full_path);
	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}

int watdfs_cli_fsync(void *userdata, const char *path,
					 struct fuse_file_info *fi)
{
	// Force a flush of file data.
	DLOG("watdfs_cli_fsync called for '%s'", path);

	struct files_status *filesStatus = (struct files_status *)userdata;
	int fxn_ret = 0;

	int fileClientMode = filesStatus->openFilesStatus[path].clientMode & O_ACCMODE;
	// the file is opened with read only
	if (fileClientMode == O_RDONLY)
	{
		return -EPERM;
	}
	// the file is opened with write only or read&write
	else
	{
		fxn_ret = cli_write_back(path, filesStatus);

		if (fxn_ret < 0)
		{
			DLOG("watdfs_cli_fsync fail");
			return fxn_ret;
		}

		// Update tc.
		filesStatus->openFilesStatus[path].tc = time(0);
	}

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

	int fxn_ret = 0;

	struct files_status *filesStatus = (struct files_status *)userdata;

	char *full_path = get_full_path(path, filesStatus);
	int cli_fd = filesStatus->openFilesStatus[path].clientDesc;

	int fileIsOpen = cli_file_is_open(path, filesStatus);
	// the file has not yet opened
	if (fileIsOpen < 0)
	{
		struct stat clientStatbuf;
		int statRet = stat(full_path, &clientStatbuf);

		// if the file is not cached
		if (statRet < 0)
		{
			// the local file not exist
			if (-errno == -ENOENT)
			{
				DLOG("client file not exist");
				int dl_ret = cli_downloadFile(path, filesStatus, O_RDWR);
				if (dl_ret < 0)
				{
					DLOG("utimensat: download file fail");
					fxn_ret = dl_ret;
					free(full_path);
					return fxn_ret;
				}
				else
				{
					int mkRet = utimensat(cli_fd, full_path, ts, 0);
					fxn_ret = mkRet;
					// close the file after download
					cli_close_file(path, filesStatus);
				}
			}
			// the stat fail
			else
			{
				DLOG("local stat fail");
				free(full_path);
				return -errno;
			}
		}
		// if the file is cached
		else
		{
			DLOG("client file exist");
			int mkRet = utimensat(cli_fd, full_path, ts, 0);
			fxn_ret = mkRet;
		}
	}
	// the file has been opened
	else
	{
		if ((filesStatus->openFilesStatus[path].clientMode & O_ACCMODE) == O_RDONLY)
		{
			DLOG("the file is read only and cannot utimensat");
			free(full_path);
			// return -EPERM;
			return -EMFILE;
		}
		else
		{
			DLOG("the file is write only or read&write, utimensat the file on client");
			int cli_fd = filesStatus->openFilesStatus[path].clientDesc;
			int mkRet = utimensat(cli_fd, full_path, ts, 0);
			fxn_ret = mkRet;
		}
	}

	// TODO: Check freshness.
	int check = cli_freshness_check(path, filesStatus);
	if (check < 0)
	{
		DLOG("utimensat: write back to server");
		fxn_ret = cli_write_back(path, filesStatus);
	}

	if (fxn_ret < 0)
	{
		DLOG("watdfs_cli_utimensat fail");
	}
	else
	{
		DLOG("watdfs_cli_utimensat succeed");
	}
	free(full_path);
	// Finally return the value we got from the server.
	return fxn_ret;
	// return -ENOSYS;
}
