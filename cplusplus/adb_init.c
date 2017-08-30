#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
 
int CreateSocket(void)
{
    struct sockaddr_un addr;
    int fd;
    int errno = -1;
    char path[] = "/dev/socket/adbd";
    int ret;
    // char *string;

    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        printf("failed to open socket!\n");
        return -1;
    }


    printf("adbd socket fd is %d\n", fd);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    //snprintf(addr.sun_path, sizeof(addr.sun_path), "/dev/socket/%s", "adbd");
    // memcpy();
    memcpy(addr.sun_path, path, sizeof(addr.sun_path));
    printf("path is %s\n", addr.sun_path);
    // ret = unlink(addr.sun_path);
    // if (ret){
    //     printf("unlink failed!\n");
    //     return ret;
    // }

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret)
    {
        printf("Failed to bind socket adbd\n");
        return ret;
    }

    chmod(addr.sun_path, 0600);

    ret = listen(fd, 4);
    if (ret < 0) {
        printf("Failed to listen on '%d'\n", fd);
        return;
    }

    setenv("ANDROID_SOCKET_adbd", "3", 1);

    fcntl(fd, F_SETFD, 0);
    printf("Created socket '%s' with mode '%o'\n",addr.sun_path, 0660);

    return 0;
}

int main()
{
    int ret;

    ret = CreateSocket();

    return ret;
}

