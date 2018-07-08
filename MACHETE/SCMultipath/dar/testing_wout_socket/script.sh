echo "-------------- clean compilation --------------"
make clean

echo "-------------------- compile ------------------"
make

echo "--------------- run parse_hijack --------------"
./parse_hijack Hijack,101.0.2.2,10.0.2.45,lat Almost-Hijack,190.23.2.34,140.0.333.46,prop Timer-alert,155.55.5.1,10.0.2.30,hop Timer-alert,155.55.5.1,10.0.2.30,hop

echo "-------------- clean compilation --------------"
make clean
