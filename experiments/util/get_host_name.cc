#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    struct hostent *he;
    char hostname[40] = {0};
    char ipaddr[40] = {0};

    gethostname(hostname, sizeof(hostname));
    cout << "hostname: " << hostname << endl;

    he = gethostbyname(hostname);
    cout << "gethostbyname return: " << he << endl;

    char destIp[128];
    char **phe = NULL;

    for (phe = he->h_addr_list; NULL != *phe; ++phe) {
        inet_ntop(he->h_addrtype, *phe, destIp, sizeof(destIp));
        cout << "addr: " << destIp << endl;
    }

    return 0;
}
