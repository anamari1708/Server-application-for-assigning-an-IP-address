Server kao argument komandne linije prima IP subnet kao prvi argument (npr. 10.15.16.0/24) i
port kao drugi argument komandne linije programa.
Serverska aplikacija cijelo vrijeme sluša zahtjeve klijenata na zadanom portu te dodjeljuje slobodne
IP adrese iz zadanog subneta.
Primjer pokretanja serverske aplikacije:
./server 10.15.16.0/24 1234
Klijent se pokreæe s portom kao argumentom.
Primjer pokretanja klijentske aplikacije:
./klijent 1234
Nakon pokretanja klijentska aplikacija se mora javiti serveru za dodjelu IP adrese preko UDP
protokola. Nakon primitka zahtjeva za IP adresom, server alocira prvu slobodnu IP adresu (u
primjeru je to 10.15.16.1), te ju pošalje klijentu.
Klijent ispiše dobivenu IP adresu na standardni izlaz i pošalje potvrdu natrag serveru, te se nastavi
vrtiti u beskonaènoj petlji.
Kad se slijedeæi klijent pokrene, on æe dobiti iduæu slobodnu adresu iz tog subneta (10.15.16.2), treæi
æe dobiti treæu slobodnu, itd. Ako su npr. èetiri spojena klijenta (imaju adrese od 10.15.16.1 do
10.15.16.4) i drugi se odspoji, iduæi klijent koji se pokrene mora dobiti adresu 10.15.16.2 jer je to
tada prva slobodna adresa.
Klijent se odjavljuje slanjem poruke serveru nakon nasumièno odabrane kolièine sekundi od
trenutka kad je dobio IP adresu - iz raspona od 5 do 10.
Serverska i klijentska aplikacija na standarni izlaz ispisuju sve poruke koje izmijenjuju