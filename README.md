# IPK - Počítačové komunikace a sítě - Jednoduchá adresářová služba

### Zadání:

Vytvoření klientské a serverové síťové aplikace v prostředí UNIXu s využitím komunikačního rozhraní BSD sockets.

Vytvořte program pro klienta a server, s využitím rozhraní schránek (BSD sockets), který implementuje požadovanou funkci.

- navrhněte aplikační protokol pro komunikaci mezi klientem a serverem
- vytvořte oba programy v jazyce C/C++ přeložitelné na studentském unixovém serveru eva.fit.vutbr.cz včetně funkčního Makefile souboru (oba programy přeložitelné po zadání příkazu make)
- vytvořte dokumentaci popisující aplikační protokol (max. 1 strana A4, formát pdf, ps, doc, html)

Programy využívají spojovanou službu (protokol TCP). Jméno programu pro server po přeložení bude server. Pro klienta client. 

**Server:**

    ./server -p cislo_portu

Pokud nebude správně zadáno číslo portu, server zahlásí chybu a ukončí se.
Server běží donekonečna a klienti od něj mohou požadovat data.
Server musí být konkuretní, tzn. bude požadována současná obsluha více klientů. 
Server předpokládá jeden povinný parametr -p následovaný číslem portu, na kterém bude očekávat spojení od klienta. 

**Klient:**

    ./client -h hostname -p port [-NSLF] [-n name] [-s surname] [-l login] [-f faculty]

Klient získává od serveru požadované informace o osobách uvedených v databázi. 
Databáze je prezentována textovým souborem, který naleznete zde. 
Server umožňuje vyhledávání v seznamu osob podle kritérií. 
Kritéria jsou jméno, příjmení, login, fakulta. Kritéria lze kombinovat. Získané informace vypíše na standardní výstup.
Klient předpokládá alespoň povinné parametry -p nasledovaný číslem portu, na kterém provede spojení se serverem a -h následovaný hostitelským jménem serveru. 
Oznámení o chybách, které mohou nastat, bude vytištěno na standardní chybový výstup (stderr). 
Data přenášená mezi klientem a serverem jsou pouze ta, která jsou požadována danou funkcí.

**Příklad:**

    client -h eva.fit.vutbr.cz -p 10000 -NSF -l xnovak99

vytiskne jméno, příjmení, fakultu osoby, která má login xnovak99.

    client -h eva.fit.vutbr.cz -p 10000 -L -f FIT -n Jan
    
vytiskne loginy všech studentů, kteří mají jméno Jan a jsou z FITu.

### Hodnocení: 

**8/8**
