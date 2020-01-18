#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>

using namespace std;
 
//zpracuje parametry, vrati cislo portu
//pri chybe vrati -1
int processArguments(int argc, char* argv[])
{
    int port;

    if (argc != 3 || strcmp(argv[1], "-p") != 0)
    {
        //musi byt minimalne 2 parametry: -p port 
        fprintf(stderr, "Spravny tvar: server -p port\n");
        return -1;
    }

    if ((port = atoi(argv[2])) <= 0) //kontrola spravnosti prevodu nasledujiciho parametru na int
    {
        fprintf(stderr, "Spatne zadane cislo portu.\n");
        return -1;
    }
    return port;
}

//zpracuje pozadavek od klienta
//string request: request od klienta
//string requested: do promenne se da informace o tom, co klient pozaduje (NSLF)
//string firstName, lastName, faculty, login: vyhledavaci kriteria, od klienta, ktera se zjisti z pozadavku
void parse(string& requested, const string& request, string& firstName, string& lastName, string& faculty, string& login)
{
    enum State { NEXT_PART, FIRST_NAME, LAST_NAME, LOGIN, FACULTY, NSLF, END };

    firstName = "";
    lastName = "";
    faculty = "";
    login = "";
    requested = "";
    State state = NEXT_PART;
    int i = 1;

    while (state != END) //konecny stav
    {
        switch (state)
        {
        case NEXT_PART: //dalsi cast pozadavku (po pomlcce)
            if (request[i] == 'n')
            {
                state = FIRST_NAME; //nasleduje jmeno
                i++;
            }
            else if (request[i] == 's')
            {
                state = LAST_NAME;  //nasleuje prijmeni
                i++;
            }
            else if (request[i] == 'l')
            {
                state = LOGIN;      //nasleuje login
                i++;
            }
            else if (request[i] == 'f')
            {
                state = FACULTY;    //nasleduje fakulta
                i++;
            }
            else
            {
                state = NSLF;       //nasleduje NSLF
            }
            break;
        case FIRST_NAME: //jmeno
            while (request[i] != '-' && request[i] != '\0')
            {
                //naplneni promenne pro jmeno
                firstName += request[i];
                i++;
            }
            if (request[i] == '-')
            {
                i++;
                state = NEXT_PART; //pozadavek pokracuje
            }
            else
            {
                state = END; //konec
            }
            break;
        case LAST_NAME: //prijmeni
            while (request[i] != '-' && request[i] != '\0')
            {
                //naplneni promenne pro prijmeni
                lastName += request[i];
                i++;
            }
            if (request[i] == '-')
            {
                i++;
                state = NEXT_PART; //pozadavek pokracuje
            }
            else
            {
                state = END; //konec
            }
            break;
        case LOGIN: //login
            while (request[i] != '-' && request[i] != '\0')
            {
                //naplneni promenne pro login
                login += request[i];
                i++;
            }
            if (request[i] == '-')
            {
                i++;
                state = NEXT_PART; //pozadavek pokracuje
            }
            else
            {
                state = END; //konec
            }
            break;
        case FACULTY: //fakulta
            while (request[i] != '-' && request[i] != '\0')
            {
                //naplneni promenne pro fakultu
                faculty += request[i];
                i++;
            }
            if (request[i] == '-')
            {
                i++;
                state = NEXT_PART; //pozadavek pokracuje
            }
            else
            {
                state = END; //konec
            }
            break;
        case NSLF:
            while (request[i] != '-' && request[i] != '\0')
            {
                requested += request[i];
                i++;
            }
            if (request[i] == '-')
            {
                i++;
                state = NEXT_PART; //pozadavek pokracuje
            }
            else
            {
                state = END; //konec
            }
            break;
        }
    }
}

//zpracuje jeden radek ze souboru s daty a vrati retezec obsahujici pozadovane udaje z tohoto radku
//pokud tento radek neobsahuje zadne udaje, ktere me zajimaji, vrati prazdny retezec
//string requested: informace o tom, co klient pozaduje (NSLF)
//string firstName, lastName, faculty, login: vyhledavaci kriteria od klienta
//char row[]: radek ze souboru s udaji
string processRow(string& requested, const string& firstName, const string& lastName, const string& faculty, const string& login, char row[])
{
    string firstNameRow, lastNameRow, loginRow, facultyRow; //sem se nactou udaje z celeho radku

    if (requested == "") requested = "NSLF"; //pokud neni zadano, bude se vypisovat vsechno

    int i = 0, j = 0;
    while (row[i] != ';') i++; //preskoceni poradoveho cisla na zacatku

    i++;
    while (row[i] != ';')
    {
        loginRow += row[i]; //nacteni loginu z radku
        i++;
    }
    if (login != "") //pokud vyhledavame na zaklade loginu
    {
        if (login != loginRow) //pokud nejde o dany login, radek nas nezajima, konec
        {
            return "";
        }
    }
    loginRow += " "; //pridani mezery na konec

    i++;
    while (row[i] != ';')
    {
        lastNameRow += row[i]; //nacteni prijmeni z radku
        i++;
    }
    if (lastName != "") //pokud vyhledavame na zaklade prijmeni
    {
        if (lastName != lastNameRow) //pokud nejde o dane prijmeni, radek nas nezajima, konec
        {
            return "";
        }
    }
    lastNameRow += " "; //pridani mezery na konec

    i++;
    while (row[i] != ';')
    {
        firstNameRow += row[i]; //nacteni jmena z radku
        i++;
    }
    if (firstName != "") //pokud vyhledavame na zaklade jmena
    {
        if (firstName != firstNameRow) //pokud nejde o dane jmeno, radek nas nezajima, konec
        {
            return "";
        }
    }
    firstNameRow += " "; //pridani mezery na konec

    i++;
    while (isalpha(row[i]))
    {
        facultyRow += row[i]; //nacteni fakulty z radku
        i++;
    }
    if (faculty != "") //pokud vyhledavame na zaklade fakulty
    {
        if (faculty != facultyRow) //pokud nejde o danou fakultu, radek nas nezajima, konec
        {
            return "";
        }
    }
    facultyRow += " "; //pridani mezery na konec

    string output;

    //radek odpovida vyhledavacim kriteriim
    //do vysledneho vystupu pridame ty nactene hodnoty, ktere chce klient vypsat
    for (i = 0; i < requested.size(); ++i)
    {
        if (requested[i] == 'N')
        {
            output += firstNameRow;
            firstNameRow = "";
        }
        else if (requested[i] == 'S')
        {
            output += lastNameRow;
            lastNameRow = "";
        }
        else if (requested[i] == 'L')
        {
            output += loginRow;
            loginRow = "";
        }
        else if (requested[i] == 'F')
        {
            output += facultyRow;
            facultyRow = "";
        }
    }

    return output + '\n';
}

//ceka na deti
void handler(int sig)
{
    int state;
    if (wait(&state) <= 0)
    {
        fprintf(stderr, "Chyba pri wait.\n");
    }
}
 
//zpracovava pozadavky od klientu
//int s: listening socket, na kterem se akceptuji klienti
//pri chybe vraci -1, jinak 0
int handle_client(int s)
{
    signal(SIGCHLD, handler);

    string request, requested, firstName, lastName, faculty, login, response, output;
    char buffer[1000];
    int sock; //socket pro komunikaci s klienty

    //akceptuje klienta na listen socketu
    if ((sock = accept(s, NULL, 0)) < 0)
    {
        perror("accept");
        return -1;
    }

    int state;
    pid_t pid = fork();
    if (pid == 0) //jsme v child procesu
    {
        close(s); //zavreme listen socket

        //precteme pozadavek od klienta 
        if (read(sock, buffer, sizeof(buffer) - 1) < 0)
        {
            perror("read"); /* read error */
            return -1;
        }
        request = buffer;

        if (request == "") exit(0);

        //vytahneme z pozadavku info
        parse(requested, request, firstName, lastName, faculty, login);

        //otevreni souboru
        ifstream file;
        file.open("ipk_database.txt");

        //zpracovavame soubor po radcich, dokud nenarazime na konec
        while (1)
        {
            file.getline(buffer, 1000);
            if (buffer[0] == '\0') break;
            output += processRow(requested, firstName, lastName, faculty, login, buffer);
        }

        //posleme klientovi odpoved       
        if (write(sock, output.c_str(), output.size()) < 0) // echo message back 
        {
            perror("write");
            return -1; // write error 
        }

        file.close(); //zavreme soubor
        close(sock); //zavreme socket pro komunikaci s klientem
        exit(0);
    }
    else if (pid == -1) //chyba forku
    {
        fprintf(stderr, "Chyba forku.\n");
    }

    close(sock); //zavreme socket pro komunikaci s klientam

    return 0;
}
 
int main(int argc, char* argv[])
{
    int s, port;
    struct sockaddr_in sin;

    //vytvori socket	 
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    //zjisti port, na kterem ma poslouchat
    if ((port = processArguments(argc, argv)) == -1)
    {
        return -1;
    }

    sin.sin_family = PF_INET; //rodina protokolu
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port); //cislo portu

    if (bind(s, (struct sockaddr*) & sin, sizeof(sin)) < 0)
    {
        perror("bind");
        return -1;
    }

    //poslouchame
    if (listen(s, 5) < 0)
    {
        perror("listen");
        return -1;
    }

    //obstaravame klienty
    while (1)
    {
        if (handle_client(s) == -1)
        {
            return -1;
        }
    }

    //zavreme listen socket
    close(s);

    return 0;
}
