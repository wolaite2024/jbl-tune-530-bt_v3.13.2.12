//#ifdef OTG_IP_EN
#ifndef _GENERIC_ERRNO_H
#define _GENERIC_ERRNO_H
#define ESUCCESS     0  /* Operation Success */

#define EPERM        1  /* Operation not permitted */
#define ENOENT       2  /* No such file or directory */
#define ESRCH        3  /* No such process */
#define EINTR        4  /* Interrupted system call */
#define EIO          5  /* I/O error */
#define ENXIO        6  /* No such device or address */
#define E2BIG        7  /* Argument list too long */
#define ENOEXEC      8  /* Exec format error */
#define EBADF        9  /* Bad file number */
#define ECHILD      10  /* No child processes */
#define EAGAIN      11  /* Try again */
#define ENOMEM      12  /* Out of memory */
#define EACCES      13  /* Permission denied */
#define EFAULT      14  /* Bad address */
#define ENOTBLK     15  /* Block device required */
#define EBUSY       16  /* Device or resource busy */
#define EEXIST      17  /* File exists */
#define EXDEV       18  /* Cross-device link */
#define ENODEV      19  /* No such device */
#define ENOTDIR     20  /* Not a directory */
#define EISDIR      21  /* Is a directory */
#define EINVAL      22  /* Invalid argument */
#define ENFILE      23  /* File table overflow */
#define EMFILE      24  /* Too many open files */
#define ENOTTY      25  /* Not a typewriter */
#define ETXTBSY     26  /* Text file busy */
#define EFBIG       27  /* File too large */
#define ENOSPC      28  /* No space left on device */
#define ESPIPE      29  /* Illegal seek */
#define EROFS       30  /* Read-only file system */
#define EMLINK      31  /* Too many links */
#define EPIPE       32  /* Broken pipe */
#define EDOM        33  /* Math argument out of domain of func */
#define ERANGE      34  /* Math result not representable */
#define EVRF        35  /* Verification failed */
#define ENOF        36  /* Not found */

#define ENODATA     61  /* No data available */
#define EOVERFLOW   75  /* Value too large for defined data type */
#define ERESTART    85  /* Interrupted system call should be restarted */
#define ECONNABORTED    103 /* Software caused connection abort */
#define EOPNOTSUPP  95  /* Operation not supported on transport endpoint */
#define ECONNRESET  104 /* Connection reset by peer */
#define ENOBUFS     105 /* No buffer space available */
#define ESHUTDOWN   108 /* Cannot send after transport endpoint shutdown */
#define ETIMEDOUT   110 /* Connection timed out */
#define EINPROGRESS 115 /* Operation now in progress */
#define ENOTSUPP        524     /* Operation is not supported */
#endif

//#endif

