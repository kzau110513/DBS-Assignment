# System Manual
By Jiayao Su, Student ID: 21044672, Wat ID: j24su.
All functions with `copy_` prefix are the copy functions from A2.

1. Gloabal Data Structure
   - client:
        ```c++
        struct file_meta {
            u_int64_t clientDesc; // the file descriptor of client file
	        int clientMode;		  // the open flag of client file, set in watdfs_cli_open
	        time_t tc;			  // the Tc of the client file
	        u_int64_t serverDesc; // the file descriptor of server file
        };
        struct files_status
        {
	        time_t cacheInterval;									 // the cache interval
	        char *pathToCache;										 // the path of cache folder
	        std::map<std::string, struct file_meta> openFilesStatus; // maintain all opened file status on client
        };
        ```
   - server：
        ```c++
        struct file_status_s
        {
	        int serverMode;	 // 0 for read, 1 for write
	        int openNum;	 // the current numbers of clients opening the file
	        rw_lock_t *lock; // the file lock
        };
        std::map<std::string, struct file_status_s> openFilesStatus_s; // maintain all opened file status on server
        ```
2. Design Choices
   - Download: The method for download is `cli_downloadFile`.
     - Get the file size on the server. Invoke rpc getattr call `copy_getattr` to get *statbuf->st_size*.
     - Open and lock the file on server. Invoke rpc open call `copy_open` with *O_RDONLY* flag. Then invoke rpc lock call `watdfs_cli_lock` with *RW_READ_LOCK* mode.
     - Create/open the client file. Invoke `open` system call with *O_CREAT | O_RDWR* flag and *S_IRWXU* mode.
     - Truncate the client file.
     - Read the file from server to local buf through rpc read call `copy_read`.
     - Write the client file with the buf reading from server.
     - Insert/Update the new client file status in the *openFilesStatus* map.
     - Update the metadata *st_mtime* and *st_atime* of the client file.
     - Unlock the file on server. Invoke rpc unlock call `watdfs_cli_unlock` with *RW_READ_LOCK* mode.
     - Release the file on server. Invoke rpc release call `copy_release`.
     
   - Upload: The method for upload is `cli_write_back`.
     - Lock in write mode. Invoke rpc lock call `watdfs_cli_lock` with *RW_WRITE_LOCK* mode.
     - Get the client file size with `stat` system call.
     - Read from client file to local buf.
     - Truncate the file on server through rpc truncate call `copy_truncate`.
     - Write from buf to server through rpc write call `copy_write`.
     - Update *st_mtime* and *st_atime* on server file through rpc utimensat call `copy_utimensat`.
     - Unlock the file on server. Invoke rpc unlock call `watdfs_cli_unlock` with *RW_WRITE_LOCK* mode.

   - Mutual Exclusion on Client
     - Maintain a *map* structure of the opened files, so that we can know if a file has been opened on client. <br>`std::map<std::string, struct file_meta> openFilesStatus`
     - We can use `openFilesStatus.find(path)` to check if the file has been opened.

   - Mutual Exclusion on Server
     - Maintain a *map* structure of the opened files, so that we can know if a file has been opened on client. <br>`std::map<std::string, struct file_status_s> openFilesStatus_s`
     - In *file_status_s*, *serverMode* shows if the file is opened by writer or readers, *openNum* indicates how many clients are currently opening this file.
     - In the `watdfs_release`, If the *openNum* goes to 0, we will invoke `rw_lock_destroy` and remove the corresponding file.

   - Cache Timeout
     - Freshness interval *t* is stored on client as *cacheInterval* in userdata.
     - Each file opened on client will create a file record stored in *file_meta* struct (The record contains the *Tc* of the file). And each record will be stored in *openFilesStatus* map.
     - *T_client* and *T_server* are determined by the metadata *st_mtime* of the file on client and server.
     - For read calls, if the file is opened in read mode, check the freshness of file. First check if `T - Tc < t`. If so, we can return directly. Otherwise, get *T_client* and *T_server* through local stat call and rpc getattr call. If `T_client == T_server`, update Tc to current time and return. If two conditions are both not satisfied, download the file, and update Tc to the current time.
     - For write calls, check freshness at the end of functions. First we check if `T - Tc < t`. If so, return directly. Otherwise, get *T_client* and *T_server* through local stat call and rpc getattr call. If `T_client == T_server`, update Tc to current time and return. If two conditions are both not satisfied, upload the file, and update Tc to the current time.
    
   - Atomic Transfer
     - We use two rpc calls `watdfs_lock` and `watdfs_unlock` registered in server side to lock and unlock the file.
     - The lock pointer *lock* in *file_status_s* struct on server is used to lock the opened file. The *file_status_s* structure instance is created and `rw_lock_init` is called when the file is opened on server in `watdfs_open`.
     - In `watdfs_release`, lock will be destroyed when *openNum* equals to 0, which means no client is opening the file. The file record will be removed from the *openFilesStatus_s* map.
     - Before each read or write process to or from server, `watdfs_lock` is called. When the process finishes, `watdfs_unlock` is called.

3. Functionalities not Implemented
   - The system did not meet the expectations. The program did not pass the release test 7 and 9, although the code has been written and should run well as the features mentiond above.

4. Error Codes
   - If client initialization fail, return *NOT_INIT*.
   - If the file does not exist on both client and server, return -*ENOENT*.
   - If `watdfs_cli_read` or `watdfs_cli_write` is called when the file is not opened, return -*EPERM*.
   - If `watdfs_cli_fsync` is called when the file is opened in read only mode, return -*EPERM*.
   - For write calls, if the file is already opened in read only mode, return -*EMFILE*.
   - In `watdfs_cli_open`, if the file is already opened, return -*EMFILE*.
   - If two write open call conflicts on server, return -*EACCES*.

5. Test Methods
   - Test on linux command line: Run the commands on client.
     - Commands: <br>`touch file_path`<br> `echo "content" > file_path`<br> `cat file_path`<br> `stat file_path`
   - Test with Python script: Run the python script on client. Assert the return values of the os system calls to be equal to the expected results.
     - Python OS Functions: <br>`os.stat`<br>`os.open`<br>`os.write`<br>`os.read`<br>`os.fsync`<br>`os.truncate`<br>`os.close`
