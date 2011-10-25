struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  u32 off;
};


// in-core file system types

struct inode {
  u32 dev;           // Device number
  u32 inum;          // Inode number
  u32 gen;           // Generation number
  int ref;           // Reference count
  int flags;         // I_BUSY, I_VALID
  int readbusy;
  struct condvar cv;
  struct spinlock lock;
  char lockname[16];
  struct ns *dir;

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  u32 size;
  u32 addrs[NDIRECT+1];
};

#define I_BUSYR 0x1
#define I_BUSYW 0x2
#define I_VALID 0x4
#define I_FREE 0x8


// device implementations

struct devsw {
  int (*read)(struct inode*, char*, int);
  int (*write)(struct inode*, char*, int);
};

extern struct devsw devsw[];

#define CONSOLE 1
