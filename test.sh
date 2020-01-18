#!/bin/ksh
make
echo ""
echo "./server -p 1701 &"
echo ""
./server -p 1701 &
pid=`echo $!`

echo "./client -h localhost -p 1701 -n Radim -NS"
./client -h localhost -p 1701 -n Radim -NS 
echo ""

echo "./client -h localhost -p 1701 -l xvybir05 -NSLF"
./client -h localhost -p 1701 -l xvybir05 -NSLF 
echo ""

echo "Neni zadano, co se ma vypsat, vypise se vse:"
echo "./client -h localhost -p 1701 -l xvybir05"
./client -h localhost -p 1701 -l xvybir05
echo ""

echo "Nektera kriteria pro vypsani vicekrat, vypise se jen jednou:"
echo "./client -h localhost -p 1701 -l xvybir05 -NSSSLFF"
./client -h localhost -p 1701 -l xvybir05 -NSSSLFF
echo ""

echo "Jiny tvar kriterii pro vypsani:"
echo "./client -h localhost -p 1701 -l xvybir05 -N -S"
./client -h localhost -p 1701 -l xvybir05 -N -S
echo ""

echo "Jine poradi kriterii pro vypsani, vypisou se v tomto poradi:"
echo "./client -h localhost -p 1701 -l xvybir05 -LSN"
./client -h localhost -p 1701 -l xvybir05 -LSN
echo ""

echo "Pokus vyhledat neexistujici zaznam, nevypise se nic:"
echo "./client -h localhost -p 1701 -l xvybir05 -f FEKT -NSLF"
./client -h localhost -p 1701 -l xvybir05 -f FEKT -NSLF
echo ""

echo "Chybejici povinne parametry programu:"
echo "./client -h -p 1701 -l xvybir05 -NSLF"
./client -h -p 1701 -l xvybir05 -NSLF
echo "./client -h localhost -l xvybir05 -NSLF"
./client -h localhost -l xvybir05 -NSLF
echo ""

echo "Vynechany vsechny volitelne parametry, nevypise se nic:"
echo "./client -h localhost -p 1701"
./client -h localhost -p 1701
echo ""

echo "Opakujici se parametry:"
echo "./client -h localhost -p 1701 -n Jan -n Honza -NSLF"
./client -h localhost -p 1701 -n Jan -n Honza -NSLF
echo ""

echo "Nejsou zadana vyhledavaci kriteria, vypisou se vsechny osoby:"
echo "./client -h localhost -p 1701 -NSLF"
./client -h localhost -p 1701 -NSLF
echo ""

kill -15 $pid

echo "Server ukonèen."
echo ""
echo "ps -el"
echo ""
ps -el 

