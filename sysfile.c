//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int n, int *pfd, struct file **pf)
{
  int fd;
  struct file *f;

  if(argint(n, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
    return -1;
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
  int fd;
  struct proc *curproc = myproc();

  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd] == 0){
      curproc->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

int
sys_dup(void)
{
  struct file *f;
  int fd;
  struct proc *curproc = myproc();
  struct systemcall dup_syscall ;
  dup_syscall.syscall_number = 10;
  // dup_syscall.syscall_name = "dup";
  dup_syscall.syscall_name[3] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&dup_syscall.time);
  dup_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = dup_syscall;
  curproc->number_of_systemcalls++;

  if(argfd(0, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  filedup(f);
  return fd;
}

int
sys_read(void)
{
  struct file *f;
  int n;
  char *p;
  struct proc *curproc = myproc();
  struct systemcall read_syscall ;
  read_syscall.syscall_number = 5;
  // read_syscall.syscall_name = "read";
  read_syscall.syscall_name[4] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&read_syscall.time);
  read_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = read_syscall;
  curproc->number_of_systemcalls++;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return fileread(f, p, n);
}

int
sys_write(void)
{
  struct file *f;
  int n;
  char *p;

  struct proc *curproc = myproc();
  struct systemcall write_syscall ;
  write_syscall.syscall_number = 16;
  // write_syscall.syscall_name = "write";
  write_syscall.syscall_name[5] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&write_syscall.time);
  write_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = write_syscall;
  curproc->number_of_systemcalls++;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return filewrite(f, p, n);
}

int
sys_close(void)
{
  int fd;
  struct file *f;
  struct proc *curproc = myproc();
  struct systemcall close_syscall ;
  close_syscall.syscall_number = 21;
  // close_syscall.syscall_name = "close";
  close_syscall.syscall_name[5] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&close_syscall.time);
  close_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = close_syscall;
  curproc->number_of_systemcalls++;

  if(argfd(0, &fd, &f) < 0)
    return -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

int
sys_fstat(void)
{
  struct file *f;
  struct stat *st;

  struct proc *curproc = myproc();
  struct systemcall fstat_syscall ;
  fstat_syscall.syscall_number = 8;
  // fstat_syscall.syscall_name = "fstat";
  fstat_syscall.syscall_name[5] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&fstat_syscall.time);
  fstat_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = fstat_syscall;
  curproc->number_of_systemcalls++;

  if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
    return -1;
  return filestat(f, st);
}

// Create the path new as a link to the same inode as old.
int
sys_link(void)
{
  char name[DIRSIZ], *new, *old;
  struct inode *dp, *ip;

  struct proc *curproc = myproc();
  struct systemcall link_syscall ;
  link_syscall.syscall_number = 19;
  // link_syscall.syscall_name = "link";
  link_syscall.syscall_name[4] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&link_syscall.time);
  link_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = link_syscall;
  curproc->number_of_systemcalls++;


  if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
    return -1;

  begin_op();
  if((ip = namei(old)) == 0){
    end_op();
    return -1;
  }

  ilock(ip);
  if(ip->type == T_DIR){
    iunlockput(ip);
    end_op();
    return -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  if((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  return 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  return -1;
}

// Is the directory dp empty except for "." and ".." ?
static int
isdirempty(struct inode *dp)
{
  int off;
  struct dirent de;

  for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    if(de.inum != 0)
      return 0;
  }
  return 1;
}

//PAGEBREAK!
int
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], *path;
  uint off;

  struct proc *curproc = myproc();
  struct systemcall unlink_syscall ;
  unlink_syscall.syscall_number = 18;
  // unlink_syscall.syscall_name = "unlink";
  unlink_syscall.syscall_name[6] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&unlink_syscall.time);
  unlink_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = unlink_syscall;
  curproc->number_of_systemcalls++;

  if(argstr(0, &path) < 0)
    return -1;

  begin_op();
  if((dp = nameiparent(path, name)) == 0){
    end_op();
    return -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  if((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  if(ip->nlink < 1)
    panic("unlink: nlink < 1");
  if(ip->type == T_DIR && !isdirempty(ip)){
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  if(ip->type == T_DIR){
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  return 0;

bad:
  iunlockput(dp);
  end_op();
  return -1;
}

static struct inode*
create(char *path, short type, short major, short minor)
{
  uint off;
  struct inode *ip, *dp;
  char name[DIRSIZ];

  if((dp = nameiparent(path, name)) == 0)
    return 0;
  ilock(dp);

  if((ip = dirlookup(dp, name, &off)) != 0){
    iunlockput(dp);
    ilock(ip);
    if(type == T_FILE && ip->type == T_FILE)
      return ip;
    iunlockput(ip);
    return 0;
  }

  if((ip = ialloc(dp->dev, type)) == 0)
    panic("create: ialloc");

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  if(type == T_DIR){  // Create . and .. entries.
    dp->nlink++;  // for ".."
    iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      panic("create dots");
  }

  if(dirlink(dp, name, ip->inum) < 0)
    panic("create: dirlink");

  iunlockput(dp);

  return ip;
}

int
sys_open(void)
{
  char *path;
  int fd, omode;
  struct file *f;
  struct inode *ip;

  struct proc *curproc = myproc();
  struct systemcall open_syscall ;
  open_syscall.syscall_number = 15;
  // open_syscall.syscall_name = "open";
  open_syscall.syscall_name[4] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&open_syscall.time);
  open_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = open_syscall;
  curproc->number_of_systemcalls++;

  if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
    return -1;

  begin_op();

  if(omode & O_CREATE){
    ip = create(path, T_FILE, 0, 0);
    if(ip == 0){
      end_op();
      return -1;
    }
  } else {
    if((ip = namei(path)) == 0){
      end_op();
      return -1;
    }
    ilock(ip);
    if(ip->type == T_DIR && omode != O_RDONLY){
      iunlockput(ip);
      end_op();
      return -1;
    }
  }

  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    return -1;
  }
  iunlock(ip);
  end_op();

  f->type = FD_INODE;
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  return fd;
}

int
sys_mkdir(void)
{
  char *path;
  struct inode *ip;

  struct proc *curproc = myproc();
  struct systemcall mkdir_syscall ;
  mkdir_syscall.syscall_number = 20;
  // mkdir_syscall.syscall_name = "mkdir";
  mkdir_syscall.syscall_name[5] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&mkdir_syscall.time);
  mkdir_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = mkdir_syscall;
  curproc->number_of_systemcalls++;

  begin_op();
  if(argstr(0, &path) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0){
    end_op();
    return -1;
  }
  iunlockput(ip);
  end_op();
  return 0;
}

int
sys_mknod(void)
{
  struct inode *ip;
  char *path;
  int major, minor;

  struct proc *curproc = myproc();
  struct systemcall mknod_syscall ;
  mknod_syscall.syscall_number = 17;
  // mknod_syscall.syscall_name = "mknod";
  mknod_syscall.syscall_name[5] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&mknod_syscall.time);
  mknod_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = mknod_syscall;
  curproc->number_of_systemcalls++;

  begin_op();
  if((argstr(0, &path)) < 0 ||
     argint(1, &major) < 0 ||
     argint(2, &minor) < 0 ||
     (ip = create(path, T_DEV, major, minor)) == 0){
    end_op();
    return -1;
  }
  iunlockput(ip);
  end_op();
  return 0;
}

int
sys_chdir(void)
{
  char *path;
  struct inode *ip;
  struct proc *curproc = myproc();
  struct systemcall chdir_syscall ;
  chdir_syscall.syscall_number = 9;
  // chdir_syscall.syscall_name = "chdir";
  chdir_syscall.syscall_name[5] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&chdir_syscall.time);
  chdir_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = chdir_syscall;
  curproc->number_of_systemcalls++;
  
  begin_op();
  if(argstr(0, &path) < 0 || (ip = namei(path)) == 0){
    end_op();
    return -1;
  }
  ilock(ip);
  if(ip->type != T_DIR){
    iunlockput(ip);
    end_op();
    return -1;
  }
  iunlock(ip);
  iput(curproc->cwd);
  end_op();
  curproc->cwd = ip;
  return 0;
}

int
sys_exec(void)
{
  char *path, *argv[MAXARG];
  int i;
  uint uargv, uarg;
  struct proc *curproc = myproc();
  struct systemcall exec_syscall ;
  exec_syscall.syscall_number = 7;
  // exec_syscall.syscall_name = "exec";
  exec_syscall.syscall_name[4] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&exec_syscall.time);
  exec_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = exec_syscall;
  curproc->number_of_systemcalls++;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv))
      return -1;
    if(fetchint(uargv+4*i, (int*)&uarg) < 0)
      return -1;
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    if(fetchstr(uarg, &argv[i]) < 0)
      return -1;
  }
  return exec(path, argv);
}

int
sys_pipe(void)
{
  int *fd;
  struct file *rf, *wf;
  int fd0, fd1;

  struct proc *curproc = myproc();
  struct systemcall pipe_syscall ;
  pipe_syscall.syscall_number = 4;
  // pipe_syscall.syscall_name = "pipe";
  pipe_syscall.syscall_name[4] = '\0';
  //wait_syscall->time = malloc(sizeof(struct rtcdate));
  cmostime(&pipe_syscall.time);
  pipe_syscall.caller_pid = curproc->pid;
  //wait_syscall->argumants = malloc(sizeof(char * 4));
  //wait_syscall->argumants = "void";
  //wait_syscall->argumants[4] = '\0';
  curproc->syscalls[curproc->number_of_systemcalls] = pipe_syscall;
  curproc->number_of_systemcalls++;

  if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
    return -1;
  if(pipealloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      myproc()->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  fd[0] = fd0;
  fd[1] = fd1;
  return 0;
}
