#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <time.h>
#include <cstring>

using namespace std;

//zpracuje parametry
//pri chybe vraci -1
int processArguments(int argc, char* argv[], string& hostname, int& port, string& request)
{
    hostname = "";
    port = 0;
    //true, pokud jiz dany parametr byl precten:
    bool n = false;
    bool s = false;
    bool l = false;
    bool f = false;
    bool N = false;
    bool S = false;
    bool L = false;
    bool F = false;

    if (argc < 5)
    {
        //musi byt minimalne 4 parametry: -h hostname -p port 
        fprintf(stderr, "Spravny tvar: client -h hostname -p port [-NSLF] [-n name] [-s surname] [-l login] [-f faculty]\n");
        return -1;
    }

    for (int i = 1; i < argc; ++i)
    {
        //prochazeni parametru  
        if (strcmp(argv[i], "-h") == 0 && argc >= i + 2)
        {
            //-h a neni posledni     
            if (hostname == "")
            {
                //hostname jeste nebylo  
                i++;
                hostname = argv[i]; //hostname z dalsiho parametru
            }
            else
            {
                fprintf(stderr, "Spravny tvar: client -h hostname -p port [-NSLF] [-n name] [-s surname] [-l login] [-f faculty]\n");
                return -1;
            }
        }
        else if (strcmp(argv[i], "-p") == 0 && argc >= i + 2)
        {
            //-p a neni posledni  
            if (port == 0)
            {
                //port jeste nebyl          
                i++;
                if ((port = atoi(argv[i])) <= 0) //kontrola spravnosti prevodu nasledujiciho parametru na int
                {
                    fprintf(stderr, "Spatne zadane cislo portu.\n");
                    return -1;
                }
            }
            else
            {
                fprintf(stderr, "Spravny tvar: client -h hostname -p port [-NSLF] [-n name] [-s surname] [-l login] [-f faculty]\n");
                return -1;
            }
        }
        else if (strcmp(argv[i], "-n") == 0 && argc >= i + 2 && n == false)
        {
            //-n, jeste nebylo a neni posledni
            n = true;
            request += argv[i];
            i++;
            request += argv[i];
        }
        else if (strcmp(argv[i], "-s") == 0 && argc >= i + 2 && s == false)
        {
            //-s, jeste nebylo a neni posledni
            s = true;
            request += argv[i];
            i++;
            request += argv[i];
        }
        else if (strcmp(argv[i], "-l") == 0 && argc >= i + 2 && l == false)
        {
            //-l, jeste nebylo a neni posledni
            l = true;
            request += argv[i];
            i++;
            request += argv[i];
        }
        else if (strcmp(argv[i], "-f") == 0 && argc >= i + 2 && f == false)
        {
            //-f, jeste nebylo a neni posledni
            f = true;
            request += argv[i];
            i++;
            request += argv[i];
        }
        else if (strlen(argv[i]) >= 2 && argv[i][0] == '-')
        {
            //jedna se o specifikaci udaju, ktere jsou pozadovany od serveru (-NSLF}
            request += argv[i][0];
            for (int p = 1; p < strlen(argv[i]); ++p)
            {
                if (argv[i][p] != 'N' && argv[i][p] != 'S' && argv[i][p] != 'L' && argv[i][p] != 'F')
                {
                    fprintf(stderr, "Spravny tvar: client -h hostname -p port [-NSLF] [-n name] [-s surname] [-l login] [-f faculty]\n");
                    return -1;
                }
                if (argv[i][p] == 'N' && N == false)
                {
                    //N, jeste nebylo
                    request += argv[i][p];
                    N = true;
                }
                else if (argv[i][p] == 'S' && S == false)
                {
                    //S, jeste nebylo
                    request += argv[i][p];
                    S = true;
                }
                else if (argv[i][p] == 'L' && L == false)
                {
                    //L, jeste nebylo
                    request += argv[i][p];
                    L = true;
                }
                else if (argv[i][p] == 'F' && F == false)
                {
                    //F, jeste nebylo
                    request += argv[i][p];
                    F = true;
                }
            }
        }
        else
        {
            fprintf(stderr, "Spravny tvar: client -h hostname -p port [-NSLF] [-n name] [-s surname] [-l login] [-f faculty]\n");
            return -1;
        }
    }
    if (hostname == "" || port == 0)
    {
        fprintf(stderr, "Spravny tvar: client -h hostname -p port [-NSLF] [-n name] [-s surname] [-l login] [-f faculty]\n");
        return -1;
    }

    return 0;
}

//komunikace se serverem
//pri chybe vrati -1, jinak 0
int communication(const string& hostname, int port, const string& request, string& response)
{
    int size, s;
    char buffer[1000];
    struct sockaddr_in sin;
    struct hostent* hptr;
    response = "";

    //vytvori socket
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    sin.sin_family = PF_INET;   //rodina protokolu
    sin.sin_port = htons(port); //cislo portu

    //zjistime info o vzdalenem pocitaci
    if ((hptr = gethostbyname(hostname.c_str())) == NULL)
    {
        fprintf(stderr, "gethostname error\n");
        return -1;
    }

    //nastaveni IP adresy, ke ktere se pripojime
    memcpy(&sin.sin_addr, hptr->h_addr, hptr->h_length);

    //navazani spojeni
    if (connect(s, (struct sockaddr*) & sin, sizeof(sin)) < 0)
    {
        perror("connect");
        return -1;
    }

    //poslani pozadavku
    if (write(s, request.c_str(), request.size() + 1) < 0)
    {
        perror("write");
        return -1;
    }

    //prijmuti odpovedi
    while ((size = read(s, buffer, sizeof(buffer) - 1)) > 0)
    {
        if (size == -1)
        {
            perror("read");
            return -1;
        }
        for (int i = 0; i < size; ++i)
        {
            response += buffer[i];
        }
    }

    //uzavreni spojeni
    if (close(s) < 0)
    {
        perror("close");
        return -1;
    }

    return 0;
}

//pri chybe vrati -1, jinak 0
int main(int argc, char* argv[])
{
    string hostname, request, response;
    int port;

    //zpracovani parametru
    if (processArguments(argc, argv, hostname, port, request) == -1)
    {
        return -1;
    }

    //komunikace se serverem
    if (communication(hostname, port, request, response) == -1)
    {
        return -1;
    }

    cout << response; //vypsani od serveru ziskanych dat

    return 0;
}
