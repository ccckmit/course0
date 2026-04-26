#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sched.h>
#include <string.h>
#include <signal.h>

// 栈大小（用于 clone 系统调用）
#define STACK_SIZE (1024 * 1024)
static char container_stack[STACK_SIZE];

// 容器内要执行的命令（默认 /bin/sh）
char *const container_args[] = {"/bin/sh", NULL};

/**
 * @brief 容器初始化函数（子进程执行）
 * @param arg 外部传入的根目录路径
 * @return 执行结果
 */
int container_main(void *arg) {
    char *root_dir = (char *)arg;
    printf("容器内 PID: %d\n", getpid()); // 容器内 PID 会从 1 开始

    // 1. 设置主机名（UTS Namespace 隔离）
    sethostname("docker0-container", 16);

    // 2. 挂载 proc 文件系统（让容器能看到自己的进程）
    mkdir("/tmp/proc", 0755);
    if (mount("proc", "/tmp/proc", "proc", 0, NULL) != 0) {
        perror("mount proc failed");
        exit(1);
    }
    // 切换 proc 挂载点到 /proc（容器内的 /proc）
    if (mount("/tmp/proc", "/proc", NULL, MS_MOVE, NULL) != 0) {
        perror("move proc failed");
        exit(1);
    }
    rmdir("/tmp/proc");

    // 3. 切换根目录（pivot_root 比 chroot 更安全）
    if (root_dir != NULL && chdir(root_dir) == 0) {
        // 创建 oldroot 目录用于 pivot_root
        mkdir(".oldroot", 0755);
        if (pivot_root(".", ".oldroot") == 0) {
            // 卸载旧根目录
            umount2(".oldroot", MNT_DETACH);
            rmdir(".oldroot");
        } else {
            perror("pivot_root failed, fallback to chroot");
            chroot("."); // 降级使用 chroot
            chdir("/");
        }
    }

    // 4. 执行容器内的命令（默认 shell）
    execv(container_args[0], container_args);
    perror("execv failed"); // 如果 execv 失败才会执行到这里
    exit(1);
}

/**
 * @brief 打印使用帮助
 * @param prog_name 程序名
 */
void usage(const char *prog_name) {
    fprintf(stderr, "用法: %s [容器根目录]\n", prog_name);
    fprintf(stderr, "示例: %s /root/ubuntu-fs\n", prog_name);
    fprintf(stderr, "说明: 若不指定根目录，仅做 PID/UTS 隔离，不切换文件系统\n");
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        usage(argv[0]);
        exit(1);
    }

    // 检查是否为 root 用户（Namespace 需要 root 权限）
    if (getuid() != 0) {
        fprintf(stderr, "错误：必须以 root 权限运行（sudo）\n");
        exit(1);
    }

    char *root_dir = (argc == 2) ? argv[1] : NULL;

    // 5. 创建隔离进程（Clone Namespace）
    // CLONE_NEWPID: PID 隔离（容器内 PID 从 1 开始）
    // CLONE_NEWUTS: UTS 隔离（主机名隔离）
    // CLONE_NEWNS: Mount 隔离（挂载点隔离）
    pid_t pid = clone(container_main, 
                      container_stack + STACK_SIZE, // 栈顶（栈向下生长）
                      CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | SIGCHLD, 
                      root_dir);

    if (pid == -1) {
        perror("clone failed");
        exit(1);
    }

    printf("容器进程 PID (宿主机视角): %d\n", pid);

    // 6. 等待容器进程退出
    waitpid(pid, NULL, 0);
    printf("容器已退出\n");

    return 0;
}