#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
 
using namespace std;

int CreateSocket(void)
{
    struct sockaddr_un addr;
    int fd;
    int errno = -1;

    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        cout << "failed to open socket!" << endl;
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "/dev/socket/%s", "adbd");

    int ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret)
    {
        cout << "Failed to bind socket adbd" << endl;
        return ret;
    }

    cout << "Created socket adbd" << endl;

    return 0;
}

int main()
{
    int ret;

    ret = CreateSocket();

    return ret;
}

