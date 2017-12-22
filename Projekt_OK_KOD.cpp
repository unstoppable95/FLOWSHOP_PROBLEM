/* PROBLEM 6
PIOTR PAWLACZYK  127245
KATARZYNA JOZWIAK 127237
GRUPA SRODA 9:45-11:15 , TYDZIEN NIEPARZYSTY
DATA REALIZACJI : STYCZEN 2017
*/


#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstdio>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

using namespace std;

#define liczba_maintenance 30//liczba przerw

#define liczba_zadan 100 // liczba zadan w instancjji

#define liczba_tworzonych_rozwiazan 35//ilosc iteracji petli korzystajacej z generatora losowego badz generatora macierzy w metaheurystyce

#define liczba_generowanych_losowo 10// liczba rozwiazan tworzonych losowo przed wejsciem do glownej petli metaheurystyki


#define wspolczynnik_odparowania 0.09 //wspolczynnik parowania macierzy feromnowej

#define wspolczynnik_wygladzenia 9 //wspolczynnik wygladzania macierzy feromnowej

int czas_trwania =25; //maksymalny czas dzialania petli metaheurystyki

int o_ile_sie_zmienia = 8; // przechowuje informacje o ile maleje prawdopodobienstwo skorzystania z generatora losowego

//DEKLARACJA TABLICY ZADAN ( trzyma informacje o operacji 1, operacji 2 i readytime op1 )
int **tablica_zadan = new int*[liczba_zadan];

//DEKLARACJA TABLICY MAINTENENOW MASZYNY M1 ( informacja o czasie trwanie , i momencie startu )
int **tablica_maintenance_m1 = new int*[liczba_maintenance];

//DEKLARACJA MACIERZY FEROMONOWEJ
double tablica_feromonowa[liczba_zadan][liczba_zadan];

// DEKLARACJA TABLIC KOLEJNOSCI ZADANA
int tablica_kolejnosci[ liczba_zadan] ;
int tablica_kolejnosci_m1[ liczba_zadan] ;
int tablica_kolejnosci_m2[ liczba_zadan] ;

vector <vector<int>> najlepszy_wektor_rozwiazan; //przechowuje najlepszego uszeregowanie ( M1 i M2 )

vector <vector <int >> wektor_rozw; //przechowuje wektory uszeregowan ( M1 i M2 )


//funkcja zwracajaca jak dluga jest przerwa na danej maszynie w wektorze uszeregowania (na M1 lub na M2)
int dlugosc_trwania_przerwy(int indeks,vector<int> rozw) {
    int indeks1=indeks;
    for(int i=indeks; i<rozw.size(); i++) {

        if(rozw[i]==rozw[i+1]) {
            indeks++;
            continue;
        }

        if(rozw[i]!=rozw[i+1]) {
            indeks++;
            break;
        }

    }
    int  a=indeks-indeks1;
    return a;
}

//funkcja wypelniajaca pomocnicza tablice do losowania maintenance dla M1
void wypelnij(int x, int y, bool tablica[]) {
    int p=x+y;
    for (int i= x ; i<p; i++) {
        tablica[i]= true;
    }
}

//funkcja sprawdzajaca czy wylosowany maintenance jest dozwolony dla M1 ( nie naklada sie na inny )
bool czywolne(int a,int b, bool tablica[]) {
    int p=a+b;
    bool check = true;
    for (int i= a ; i<p; i++) {
        if (tablica[i]) {
            check = false;
            break;
        }
    }
    if (!check)
        return true;
    else
        return false;
}

// funkcja zapisujaca zawartosc tablicy zadan do pliku wejsciowego
void zapiszdopliku(int rows, int columns, int **tablica,int parametr) {
    ofstream plik;
    string nazwa= "Testy/Instancja" + to_string(parametr) + ".txt";
    CreateDirectory("Testy", NULL);
    plik.open(nazwa);
    if(plik.good()) {
        plik<<"****"<<parametr<<"****"<<"\n";
        plik<<liczba_zadan<<endl;
        for(int i = 0; i < rows; i++) {
            for(int y = 0; y < columns; y++) {
                plik << tablica[i][y] << ";";
            }
            plik << "\n";
        }
        plik.close();
    }
}

//funckja dopisujaca do pliku wejsciowego zawartosc tablicy maintenance dla M1
void dopiszdopliku(int rows, int columns, int **tablica,int parametr) {
    ofstream plik;
    string nazwa= "Testy/Instancja" + to_string(parametr) + ".txt";
    plik.open(nazwa, ios::out|ios::app);
    if(plik.good()) {
        for(int i = 0; i < rows; i++) {
            plik<<i+1<<";";
            for(int y = 0; y < columns; y++) {
                if(y==1) {
                    plik<<tablica[i][y]-1<<";";
                } else
                    plik << tablica[i][y] << ";";
            }
            plik << "\n";
        }
        plik<<"****EOF****";
        plik.close();
    }
}

//funkcja tworzaca plik wynikowy metaheurystyki
void dopisz_plikwynikowy(vector<vector<int>> wektor_wektorow,int indeksM1, int indeksM2, int najlepszy_czas, int czas_po_losowym,int suma_maintenaceM1, int suma_maintenaceM2,int sumaIDLE_M1, int sumaIDLE_M2,int parametr) {
    ofstream plik;
    string nazwa1= "Testy/Plik_wynikowy" + to_string(parametr) + ".txt";
    plik.open(nazwa1);
    if(plik.good()) {
        plik<<"****"<<parametr<<"****"<<"\n";
        plik<<najlepszy_czas<< "," <<czas_po_losowym<<"\n";
        int zmienna_pom1;
        int y1;
        plik<<"M1: ";
        int indeksprzerwy1=0;
        for (int x=1; x<wektor_wektorow[indeksM1].size(); x+=y1) {
            zmienna_pom1=wektor_wektorow[indeksM1][x];
            if (zmienna_pom1>0) {
                y1 =tablica_zadan[zmienna_pom1-1][0];
                plik<<"op1_"<<zmienna_pom1<<";"<<x-1<<";"<<y1<<";";
            }
            if (zmienna_pom1<0) {
                y1=tablica_maintenance_m1[(-1*zmienna_pom1)-1][0];
                plik<<"maint"<<(-1*zmienna_pom1)<<"_M1"<<";"<<x-1<<";"<<y1<<";";

            }
            if (zmienna_pom1==0) {
                y1=dlugosc_trwania_przerwy(x,wektor_wektorow[indeksM1]);
                indeksprzerwy1++;
                plik<<"idle"<<indeksprzerwy1<<"_M1"<<";"<<x-1<<";"<<y1<<";";
            }
            if (x+y1>wektor_wektorow[indeksM1].size()) break;

        }

        int zmienna_pom;
        int y;
        int indeksprzerwy=0;

        plik<<"\nM2: ";
        for (int i=1; i<wektor_wektorow[indeksM2].size(); i+=y) {
            zmienna_pom=wektor_wektorow[indeksM2][i];
            if (zmienna_pom>0) {
                y= tablica_zadan[zmienna_pom-1][1];
                plik<<"op2_"<<zmienna_pom<<";"<<i-1<<";"<<y<<";";
            }
            if (zmienna_pom==0) {
                y=dlugosc_trwania_przerwy(i,wektor_wektorow[indeksM2]);

                indeksprzerwy++;
                plik<<"idle"<<indeksprzerwy<<"_M2"<<";"<<i-1<<";"<<y<<";";
            }
            if (i+y>wektor_wektorow[indeksM2].size()) break;
        }

        plik<<"\n";
        plik<<suma_maintenaceM1<<"\n";
        plik<<suma_maintenaceM2<<"\n";
        plik<<sumaIDLE_M1<<"\n";
        plik<<sumaIDLE_M2<<"\n";
        plik<<"****EOF****";

        plik.close();
    }
}

//funkcja zwracajaca miejsce ( indeks ) do wstawienia na M2 , korzystajac z uszeregowania M1
int miejsce_do_wstawienie_op2_M2(int indeks,vector<int> rozw) {
    int rozmiar_wektora=rozw.size();
    for(int i=indeks; i<rozmiar_wektora; i++) {
        if(rozw[i]<0) {
            indeks++;
            continue;
        }
        if(rozw[i]==rozw[i+1]) {
            indeks++;
            continue;
        }
        if(rozw[i]!=rozw[i+1]) {
            indeks++;
            break;
        }
    }
    return indeks;
}

//zwraca wartosc funkcji celu dla pojedynczego wektora uszeregowania ( M1 lub M2 )
int funkcja_celu(vector<int> wektorM2) {
    int rozmiarwektora=wektorM2.size()-1;
    int suma=0;

    for(int i=1; i<rozmiarwektora; i++) {
        if(wektorM2[i]<=liczba_zadan && wektorM2[i]>0) {
            if(wektorM2[i]!=wektorM2[i+1]) {
                suma+=i;
            }
        }
    }
    if(wektorM2[rozmiarwektora]>0) suma+=rozmiarwektora;
    return suma;
}

//funkcja zwracajaca wartosc funkcji celu dla danego uszeregowania , dla M1 i M2
int funkcja_celu_wektor_wektorow(vector<vector<int>> wektor_wektorow, int indeks_M1) {
    int suma=0;
    int sum1=0;
    int sum2=0;
    int indeks2=indeks_M1+1;
    sum1=funkcja_celu(wektor_wektorow[indeks_M1]);
    sum2=funkcja_celu(wektor_wektorow[indeks2]);
    suma =sum1 + sum2;
    return suma;
}

//funkcja zwracajaca wartosc funkcji celu dla najlepszego z dostepnych uszeregowan
int best_funkcja_celu_wektora_wektorow(vector<vector<int>> wektor_wektorow) {
    int najkrotsza_suma=funkcja_celu(wektor_wektorow[0])+funkcja_celu(wektor_wektorow[1]);
    int indeks_najkrotszej_sumy; //zwraca indeks maszyny M1, mszyna m2 to maszyna M1+1
    int x;
    for(int i=0; i<wektor_wektorow.size(); i++) {
        if(i%2==0) {
            x=funkcja_celu(wektor_wektorow[i])+funkcja_celu(wektor_wektorow[i+1]);
            if(x<najkrotsza_suma) {
                najkrotsza_suma=x;
                indeks_najkrotszej_sumy=i;
            }
        }
    }
    return najkrotsza_suma;
}
int best_funkcja_celu_wektora_wektorow_losowy(vector<vector<int>> wektor_wektorow) {

    int najgorsza_suma=funkcja_celu(wektor_wektorow[0])+funkcja_celu(wektor_wektorow[1]);
    int indeks_najgorszej_sumy;
    int x;
    for(int i=0; i<wektor_wektorow.size(); i++) {
        if(i%2==0) {
            x=funkcja_celu(wektor_wektorow[i])+funkcja_celu(wektor_wektorow[i+1]);
            if(x<najgorsza_suma) {
                najgorsza_suma=x;
                indeks_najgorszej_sumy=i;
            }
        }
    }
    return najgorsza_suma;
}


//do wektora wektorow dodaje najlepsze obecne uszeregowanie, jezeli znalazlem lepsze niz tam sie znajduje to zamieniam
void best_funkcja_celu_wektora_wektorow_void() {
    int najkrotsza_suma=INT_MAX;//funkcja_celu(wektor_rozw[0])+funkcja_celu(wektor_rozw[1]);
    int indeks_najkrotszej_sumy=0; //zwraca indeks maszyny M1, mszyna m2 to maszyna M1+1
    int x;
    int wektor_rozwiazan_size=wektor_rozw.size();
    for(int i=0; i<wektor_rozwiazan_size; i++) {
        if(i%2==0) {
            x=funkcja_celu(wektor_rozw[i])+funkcja_celu(wektor_rozw[i+1]);

            if(x<najkrotsza_suma) {
                najkrotsza_suma=x;
                indeks_najkrotszej_sumy=i;
            }
        }
    }
    if(najlepszy_wektor_rozwiazan.size()==0) {
        najlepszy_wektor_rozwiazan.push_back(wektor_rozw[indeks_najkrotszej_sumy]);
        najlepszy_wektor_rozwiazan.push_back(wektor_rozw[indeks_najkrotszej_sumy+1]);
    } else if(funkcja_celu_wektor_wektorow ( najlepszy_wektor_rozwiazan, 0) > funkcja_celu_wektor_wektorow(wektor_rozw,indeks_najkrotszej_sumy)) { //////////
        najlepszy_wektor_rozwiazan.clear();
        najlepszy_wektor_rozwiazan.push_back(wektor_rozw[indeks_najkrotszej_sumy]);
        najlepszy_wektor_rozwiazan.push_back(wektor_rozw[indeks_najkrotszej_sumy+1]);
    }
}


//funkcja zwracajaca indeks najblizszego maintenencu dla M1
int najblizszy_m(int i) {
    int tablicapomocnicza[liczba_maintenance];
    for(int j=0; j<liczba_maintenance; j++) {
        tablicapomocnicza[j]=tablica_maintenance_m1[j][1]-i;
    }
    int min_odl;
    int min_indeks=-1;
    for(int k=0; k<liczba_maintenance; k++) //jezeli jestem na maintanencie zwracam jego numer
        if(tablicapomocnicza[k]==0)
            return k;

    for(int k=0; k<liczba_maintenance; k++) {
        if(tablicapomocnicza[k]>0) {
            min_odl=tablicapomocnicza[k];
            min_indeks=k;
            break;
        }
    }

    for(int j=min_indeks; j<liczba_maintenance; j++) {
        if(tablicapomocnicza[j]<=min_odl && tablicapomocnicza[j]>=0) {
            min_indeks=j;
            min_odl=tablicapomocnicza[j];
        }
    }
    if (min_indeks==-1) return -1;
    else return min_indeks;

}


//czy moge umiesc dane zadanie na M2 pod danym indeksem, korzystajac z uszeregowania M1
int czy_moge_na_M2(int nr_zadania,int indeks, vector<int> rozw) {
    bool byl=false;
    int pom=rozw.size();
    for(int x=indeks; x<pom; x++) { //petla sprawdzajaca czy wykonalo sie w M1 (w petli sprawdza czy nie ma tego zadania dalej)
        if(rozw[x]==nr_zadania) byl=true;
        if(byl) return -1;// -1 znaczy ze nie moge pod danym indeksem wlozyc tego zadania na m2
    }
    if(!byl) return 0;//byl przed danym indeksem, moge umiescic
}


//----------- GENERATOR LOSOWY -----------------
//----------- USZEREGOWAN M1 i M2 --------------
void generator_losowy() {

//zmienne pomocnicze
    int najblizszy_maintenance; // indeks najblizszej przerwy
    int tablica_uzytych[liczba_zadan];//tablica ulozonych juz zadan
    for (int i=0; i<liczba_zadan; i++) tablica_uzytych[i]=0;
    int wszystkie_mniejsze[liczba_zadan];
    for (int i=0; i<liczba_zadan; i++) wszystkie_mniejsze[i]=-1;//tablica zadan ktore mozna wlozyc
    int i=1;
    int suma=0;
    int a;
// ********M1********
    vector <int> rozw;//wektor uszeregowania
    rozw.clear();
    rozw.push_back(23);//uzupelienie 0 indeksu wektora nieistotna wartoscia

powrot:
    while(suma<liczba_zadan) {
       // int piter;
        int losowanko=liczba_zadan;
        int numer_zadania=rand()%losowanko;
        if(tablica_uzytych[numer_zadania]==0) { // ---1---
            najblizszy_maintenance=najblizszy_m(i);//zwraca indeks najblizszego maintanance
            if (najblizszy_maintenance==-1) {
                goto skok2;    // ---2---
            } else {
                if(tablica_maintenance_m1[najblizszy_maintenance][1]>=(i+tablica_zadan[numer_zadania][0])) { // ---4---
skok2:
                    if(tablica_zadan[numer_zadania][2]<=i) { // ---3---
skok1:
                        a=tablica_zadan[numer_zadania][0];

                        for (int j=i; j<i+a; j++) { //FOR NR 3
                            rozw.push_back(numer_zadania+1);
                        }

                        tablica_uzytych[numer_zadania]=1;
                        suma++;
                        i+=tablica_zadan[numer_zadania][0];
                        goto powrot;
                    } else {
                        goto powrot;
                    }
                } else { //---- 5---
                    int k=0;
                    for (int j=0; j<liczba_zadan; j++) { //pkt 6
                        if( tablica_zadan[j][0]<=tablica_maintenance_m1[najblizszy_maintenance][1]-i && i>=tablica_zadan[j][2] &&tablica_uzytych[j]==0) { // pkt 6
                            wszystkie_mniejsze[k]=j;
                            k++;
                        }
                    }
                    if (k==0) { //liczba zadan ktore sie zmieszcz, wszystkie mniejsze
                        if(i==tablica_maintenance_m1[najblizszy_maintenance][1]) { //---7---
                            for (int z=i; z<(i+tablica_maintenance_m1[najblizszy_maintenance][0]); z++) {
                                rozw.push_back(-1*(najblizszy_maintenance+1));
                            }
                        } else {
                            for (int z =i; z<tablica_maintenance_m1[najblizszy_maintenance][1]; z++) { //---8---
                                rozw.push_back(0);

                            }
                            i=tablica_maintenance_m1[najblizszy_maintenance][1];
                            for (int y=i; y<(i+tablica_maintenance_m1[najblizszy_maintenance][0]); y++) { //10
                                rozw.push_back(-1*(najblizszy_maintenance+1));
                            }
                        }
                        i=tablica_maintenance_m1[najblizszy_maintenance][0]+tablica_maintenance_m1[najblizszy_maintenance][1];//---9---
                        najblizszy_maintenance=najblizszy_m(i); //---10---
                        goto powrot;

                    } else { // ---6---
                        int x;
                        x=rand()%k;
                        numer_zadania=wszystkie_mniejsze[x];
                        for (int y=0; y<k; y++) { //numer 9
                            wszystkie_mniejsze[y]=-1;
                        }
                        goto skok1;
                    }
                }
            }
        } else goto powrot;
    }

    wektor_rozw.push_back(rozw);

//------------------USZEREGOWANIE MASZYNY NR 2-------------
    int tablica_uzytychM2[liczba_zadan];//tablica ulozonych juz zadan

    for (int i=0; i<liczba_zadan; i++) tablica_uzytychM2[i]=0;

    int wszystkie_mniejszeM2[liczba_zadan];
    for (int i=0; i<liczba_zadan; i++) wszystkie_mniejszeM2[i]=-1;//tablica zadan ktore mozna wlozyc

    int i2=1;
    int suma2=0;
    vector <int> rozwM2;//wektor uszeregowania
    rozwM2.clear();
    rozwM2.push_back(23);//dorzucenie na 1 indeks wektora wartosci 23
powrot1:
    while(suma2<liczba_zadan) {
        int k=0;
        for (int j=0; j<liczba_zadan; j++) {
            if(czy_moge_na_M2(j+1,i2,rozw)==0 && tablica_uzytychM2[j]==0) { //j=0 zadanie 1 w tablicy zadan
                wszystkie_mniejszeM2[k]=j+1;
                k++;
            }
        }
        if(k==0) {
            int koniec_na_M1=miejsce_do_wstawienie_op2_M2(i2,rozw);
            for (int j=i2; j<koniec_na_M1; j++) {
                rozwM2.push_back(0);
                i2++;
            }
            goto powrot1;
        } else {
            int x=rand()%k;// indeks elementu
            int losowana=wszystkie_mniejszeM2[x];

            for (int x=i2; x<(i2+tablica_zadan[losowana-1][1]); x++) {
                rozwM2.push_back(losowana);
            }
            tablica_uzytychM2[losowana-1]=1;
            i2+=tablica_zadan[losowana-1][1];
            suma2++;
            goto powrot1;
        }
    }

    wektor_rozw.push_back(rozwM2);

}


// generator zadan i maintanencow , tworzacy plik wejsciowy
// generuje czasy trwania zadan, readytime dla zadan na m1 oraz maintenency dla m1
int generator(int n,int **tablica_zadan, int ** tablica_maintenance_m1, int parametr) { //n-liczba zadan ( globalna ) pozniej sie poprawi
    for(int i = 0; i < n; i++) {
        tablica_zadan[i] = new int[3];
    }

    for(int i = 0; i < liczba_maintenance; i++) {
        tablica_maintenance_m1[i] = new int[2];
    }
    //pierwsza polowa tablicy zadan
    for(int i=0; i<(n/2)+1; i++)
    {
        tablica_zadan[i][0] = (rand()%20)+1; //czas operacji 1
        tablica_zadan[i][1] = (rand()%100)+1; // czas operacji 2

    }
    //druga polowa
    for(int i=n/2; i<n; i++)
    {
        tablica_zadan[i][0] = (rand()%100)+1; //czas operacji 1
        tablica_zadan[i][1] = (rand()%20)+1; // czas operacji 2

    }
    int dlugosc_op1m1=0;
    for (int i=0; i<n; i++) {
        dlugosc_op1m1 += tablica_zadan[i][0];
    }

    int zakres=dlugosc_op1m1/2;
    //wypelnianie readytime dla op1 kazdego zadania
    for (int i=0;i<n;i++){
        tablica_zadan[i][2]=(rand()%zakres+1);
    }

    zapiszdopliku(n, 3, tablica_zadan,parametr);

    int x = dlugosc_op1m1+0.3*n;
    bool *tabpom= new bool [x];
    for(int i=0; i< x; i++) {
        tabpom[i]= false;
    }

    int licz = 0;
    for(int i=0; i<liczba_maintenance;)
    {
        tablica_maintenance_m1[i][0]= rand()%(0,3*n)+1;//dlugosc trwania 1
        tablica_maintenance_m1[i][1]= (rand()%dlugosc_op1m1)+1;//czas startu 1
        int a=tablica_maintenance_m1[i][1];
        int b=tablica_maintenance_m1[i][0];
        bool z = czywolne(a, b, tabpom);
        licz++;
        if (!z)
        {
            i++;
            wypelnij(a, b, tabpom);
        }
    }


    dopiszdopliku(liczba_maintenance,2,tablica_maintenance_m1,parametr);
    delete [] tabpom;
}


// ------------------FUNKCJE DO METAHERYSTYKI ------------

//--------------------PAROWANIE--------------------------
//obnize wartosci w calej tablicy feromonowej o okreslony wspolczynnik
void parowanie_tablica_feromonowej(double tablica_feromonowa[liczba_zadan][liczba_zadan]) {
    for(int i=0; i<liczba_zadan; i++) {
        for(int j=0; j<liczba_zadan; j++) {
            tablica_feromonowa[i][j]=tablica_feromonowa[i][j]*(1-wspolczynnik_odparowania);
        }
    }
}

//funkcja tworzaca wektor uszeregowania M1, na podstawie tablicy kolejnosci zadan ( po przejsciu macierzy feromonowej )
void dodawanie_pokolei_M1_void(int tab_kolejnosci[]) {
    vector<int> dodane_w_kolejnosci;
    dodane_w_kolejnosci.clear();
    dodane_w_kolejnosci.push_back(23);
    int ktore=0;
    int i=1;
    int suma=0;
    int a;
powrot3:
    while(suma<liczba_zadan) {
        int numer_zadania=tab_kolejnosci[ktore];

        int najblizszy_maintenance=najblizszy_m(i); //znajduje indeks najmniejszego maintenensu
        if(tablica_zadan[numer_zadania][2]<=i) { //sprawdzam czy ready time zadania na m1 jest mniejsze badz rowne miejscu w ktorym chce go wstawic
skok4:
            if (najblizszy_maintenance==-1) { //czyli nie ma juz dalej maintenensow
skok3:
                a=tablica_zadan[numer_zadania][0]; //a=czas trwania zadania

                for (int j=i; j<i+a; j++) { //FOR NR 3  ->  do indeksu w ktorym jestem+cz. trw. zadania dodaje do wektora
                    dodane_w_kolejnosci.push_back(numer_zadania+1);  //+1 bo na wektorze sa o 1 wieksze niz w tablicy zadan

                }
                ktore++;
                suma++;
                i+=tablica_zadan[numer_zadania][0]; //przesuwam sie indeksem na koniec zadania
                goto powrot3;
            } else { //---4---
                if(tablica_maintenance_m1[najblizszy_maintenance][1]>=(i+tablica_zadan[numer_zadania][0])) { //cz. rozp. najblizszego maint. >=obecny indeks+cz. trwania zadania
                    goto skok3;
                } else { //---5---
                    if(i==tablica_maintenance_m1[najblizszy_maintenance][1]) { //---5---
                        for (int z=i; z<(i+tablica_maintenance_m1[najblizszy_maintenance][0]); z++) {
                            dodane_w_kolejnosci.push_back(-1*(najblizszy_maintenance+1));
                        }
                    } else {
                        for (int z =i; z<tablica_maintenance_m1[najblizszy_maintenance][1]; z++) { //---8---
                            dodane_w_kolejnosci.push_back(0);
                        }
                        i=tablica_maintenance_m1[najblizszy_maintenance][1];
                        for (int y=i; y<(i+tablica_maintenance_m1[najblizszy_maintenance][0]); y++) { //10
                            dodane_w_kolejnosci.push_back(-1*(najblizszy_maintenance+1));
                        }
                    }
                    i=tablica_maintenance_m1[najblizszy_maintenance][0]+tablica_maintenance_m1[najblizszy_maintenance][1];//---9---
                    najblizszy_maintenance=najblizszy_m(i); //---10---
                    goto skok4;
                }
            }
        } else { //readytime zadania >i (dodaje przerwe do readytime)
            for(int t=i; t<tablica_zadan[numer_zadania][2]; t++) {

                dodane_w_kolejnosci.push_back(0);
            }
            i=tablica_zadan[numer_zadania][2];
            goto skok4;
        }

    }
    wektor_rozw.push_back(dodane_w_kolejnosci);
}

//funkcja zwracajaca miejsce ( indeks ) do wstawienia danego zadania na M2, korzystajaca z wektora M1
int miejsce_do_wstawienia(int nr_zadania,vector<int>wektorM) {
    int pomoc=nr_zadania;
    int indeks=0;

    for ( int i =1 ; i<wektorM.size(); i++) {
        if(wektorM[i]==pomoc) {
            indeks = i;
            break;
        }
    }
    int pomoc1= tablica_zadan[nr_zadania-1][0];
    int a = indeks+ pomoc1;

    return a;
}

// funkcja tworzaca wektor uszeregowania M2 na podstawie tablicy kolejnosci
void dodawanie_pokolei_M2_void(vector<int> wektorM1,int tab_kolejnosci[]) {
    vector<int> dodane_w_kolejnosciM2;
    dodane_w_kolejnosciM2.clear();
    int ktore=0;
    int i3=1;
    int suma=0;
    dodane_w_kolejnosciM2.push_back(23);

powrot44:
    while(suma<liczba_zadan) {
        int numer_zadania=tab_kolejnosci[ktore];
        if(czy_moge_na_M2(numer_zadania+1,i3,wektorM1)==0) { //sprawdzam czy wykonalo sie na m1 ////////////////////////////
skok44:
            for(int j=i3; j<i3+tablica_zadan[numer_zadania][1]; j++) {
                dodane_w_kolejnosciM2.push_back(numer_zadania+1);
            }
            suma++;
            i3+=tablica_zadan[numer_zadania][1];
            ktore++;
            goto powrot44;
        } else { //sprawdzanie kiedy wykona sie na M1 i wypelnianie dziury
            int piciu;
            int gdzie_wstawic;

            gdzie_wstawic=miejsce_do_wstawienia(numer_zadania+1,wektorM1);
            for(int k=i3; k<gdzie_wstawic; k++) {
                dodane_w_kolejnosciM2.push_back(0);
                i3++;
            }
            goto skok44;
        }
    }

    wektor_rozw.push_back(dodane_w_kolejnosciM2);
}


//funkcja ktora wchodzi z wektorem uszeregowania, tablica feromonowa oraz wartoscia ktora dodaje do danego pola
void wypelnianie_macierzy_feromonowej__uszeregowania( vector<int> dodane_w_kolejnosci,double wartosc) {
    // cout<<"wartosc uzupekniajaca " <<wartosc<<endl;
    int tablicapomocnicza[liczba_zadan];
    int indeks=0;
    int y=0;
    int zmienna_pom=0;
    int rozmiar_wektora=dodane_w_kolejnosci.size();
    int last = liczba_zadan + 1;
    for (int i=1; i<rozmiar_wektora; i++) {
        zmienna_pom = dodane_w_kolejnosci[i];

        if(zmienna_pom > 0) {
            if(last != zmienna_pom) {
                tablicapomocnicza[indeks++] = zmienna_pom-1;
                last = zmienna_pom;
                zmienna_pom = -1;
            }
        }
    }

    for (int y=0; y<liczba_zadan; y++) {
        int a =tablicapomocnicza[y];
        int b=tablicapomocnicza[y+1];
        if (y==liczba_zadan-2) {
            tablica_feromonowa[a][b]+=wartosc;
            break;
        } else {
            tablica_feromonowa[a][b]+=wartosc;
        }
    }

}

void turniej (int do_zostawienia)
{
    int indeks_zad1;
    int indeks_zad2;
    int wart_zad1;
    int wart_zad2;

    int ilosc_wektorow=wektor_rozw.size(); //zmienna przechowujaca dlugosc wektora uszeregowan maszyn M1 i M2
// 0-1 U1 ( M1, M2 ) ; 2-3 U2 ( M1 , M2 )
    int ilosc_wek_po_turnieju=(ilosc_wektorow-(do_zostawienia*2))/2;

    for (int i=0; i<ilosc_wek_po_turnieju; i++) {
skok:
        indeks_zad1=(rand()%(ilosc_wektorow/2))*2;
        indeks_zad2=(rand()%(ilosc_wektorow/2))*2;
        if ( indeks_zad1!=indeks_zad2) { // warunek zabezpieczajacy wylosowanie tej samej liczby
            if(indeks_zad1 % 2 ==0) {
                wart_zad1=funkcja_celu_wektor_wektorow(wektor_rozw,indeks_zad1);
            }
            if(indeks_zad2 % 2 ==0) {
                wart_zad2=funkcja_celu_wektor_wektorow(wektor_rozw,indeks_zad2);
            }
        } else goto skok;

        if (wart_zad1>=wart_zad2) {
            wektor_rozw.erase(wektor_rozw.begin()+indeks_zad1);
            wektor_rozw.erase(wektor_rozw.begin()+indeks_zad1);
        } else {
            wektor_rozw.erase(wektor_rozw.begin()+indeks_zad2);
            wektor_rozw.erase(wektor_rozw.begin()+indeks_zad2);
        }
        ilosc_wektorow-=2;
    }
}

//funkcja mutujaca wektor M1 i M2 , wedlug wylosowanych zadan
void mutacja(int indeks_wektora_M1,int indeks_wektora_M2) {
//uzupelnianie tablicy kolejnosci dla maszyny 1;
    //----------------------------  M1 ---------------------------------
    int indeks=0;
    int y=0;
    int zmienna_pom=0;
    int rozmiar_wektora=wektor_rozw[indeks_wektora_M1].size();
    int last = liczba_zadan + 1;
    for (int i=1; i<rozmiar_wektora; i++) {
        zmienna_pom = wektor_rozw[indeks_wektora_M1][i];

        if(zmienna_pom > 0) {
            if(last != zmienna_pom) {
                tablica_kolejnosci_m1[indeks++] = zmienna_pom-1;
                last = zmienna_pom;
                zmienna_pom = -1;
            }
        }
    }
// -------- M2 --------------------
    int indeks1=0;
    int y1=0;
    int zmienna_pom1=0;
    int rozmiar_wektora1=wektor_rozw[indeks_wektora_M2].size();
    int last1 = liczba_zadan + 1;
    for (int i=1; i<rozmiar_wektora1; i++) {
        zmienna_pom1 = wektor_rozw[indeks_wektora_M2][i];

        if(zmienna_pom1 >0) {
            if(last1 != zmienna_pom1) {
                tablica_kolejnosci_m2[indeks1++] = zmienna_pom1-1;
                last1 = zmienna_pom1;
                zmienna_pom1 = -1;
            }
        }
    }
//---------------LOSOWANIE
    //losowanie zadan do zamiany
    int ktore1=rand()%(liczba_zadan);
    int ktore2=rand()%(liczba_zadan);

    while(ktore2==ktore1) {
        ktore2=rand()%(liczba_zadan);
    }

    //zamiana tych zadan w tablicach kolejnosci
    int pom=ktore1;//zamiana elementow o indeksach ktore1 i ktore2
    int pom1=ktore2;
    int indekspom1;
    int indekspom2;
    for (int i=0; i<liczba_zadan ; i++) {
        if(tablica_kolejnosci_m1[i]==pom) {
            indekspom1=i;
            break;
        }
    }
    for (int i=0; i<liczba_zadan ; i++) {
        if(tablica_kolejnosci_m1[i]==pom1) {
            indekspom2=i;
            break;
        }
    }
    tablica_kolejnosci_m1[indekspom1]=ktore2;
    tablica_kolejnosci_m1[indekspom2]=ktore1;
    int indekspom1_2;
    int indekspom2_2;
    for (int i=0; i<liczba_zadan ; i++) {
        if(tablica_kolejnosci_m2[i]==pom) {
            indekspom1_2=i;
            break;
        }
    }
    for (int i=0; i<liczba_zadan ; i++) {
        if(tablica_kolejnosci_m2[i]==pom1) {
            indekspom2_2=i;
            break;
        }
    }
    tablica_kolejnosci_m2[indekspom1_2]=ktore2;
    tablica_kolejnosci_m2[indekspom2_2]=ktore1;


    //dodawanie wynikow do wektorow - wywolanie funkcji
    dodawanie_pokolei_M1_void(tablica_kolejnosci_m1);
    dodawanie_pokolei_M2_void(wektor_rozw[wektor_rozw.size()-1],tablica_kolejnosci_m2);

}



//wygladza tablicy feromonowej  wg wzoru ze strony + parametr WSPOLCZYNNIK WYGLADZENIA
void wygladzanie_tablicy_feromonowej() {
    for(int i=0; i<liczba_zadan; i++) {
        double minn=tablica_feromonowa[0][i];
        for(int j=0; j<liczba_zadan; j++) {
            if(tablica_feromonowa[i][j]<minn)
                minn=tablica_feromonowa[i][j];
        }
        if(minn<=0) minn=0.00001;
        for(int j=0; j<liczba_zadan; j++) {
            if(tablica_feromonowa[i][j]>0) {
                tablica_feromonowa[i][j]=minn*(1*(log10(tablica_feromonowa[i][j]/minn)/log10(wspolczynnik_wygladzenia)));
            } else {
                tablica_feromonowa[i][j]=minn*(1*(log10(0.00001/minn)/log10(wspolczynnik_wygladzenia)));
            }
        }
    }

}




//----------------- ZWRACANIE KOLEJNOSCI ZADAN WG MACIERZY FEROMONOWEJ ---------------------
// rozpoczynam od losowo wybranego zadania
void zwroc_kolejnosc_macierz_feromonowa(int zadanie_poczatkowe) {
    int indeks=0;//poczatkowy indeks tablicy kolejnosc
    int start=zadanie_poczatkowe; //zmienne przechowujaca wartosc zadania startowego w petli while
    vector <int > zad_uzyte ; //wektor zadan uzytych
    zad_uzyte.clear();
    zad_uzyte.push_back(start);
    tablica_kolejnosci[indeks]=start; // tablica kolejnosc dostaje wartosc wylosowanego zadania z kotrym wchodze
    indeks++;
    bool zadanie_uzyte[liczba_zadan];
    zadanie_uzyte[start]=true;
    double suma=0.0;
    double pom_fermonowa[liczba_zadan][liczba_zadan];
    int ostatnie_zadanie=start;
    int liczba_wybranych=1;


    for (int i=0; i<liczba_zadan; i++) {
        suma=0.0;
        for (int j=0; j<liczba_zadan; j++) {
            suma+=tablica_feromonowa[i][j]+1;
            pom_fermonowa[i][j]=suma;
        }
    }
    int licznik_iteracji=0;
    int losowana;
    int wartosc_wylosowana;
    while ( liczba_wybranych<=liczba_zadan) {
        losowana= (pom_fermonowa[ostatnie_zadanie][liczba_zadan-1]); //zmienna przechowujaca wartosc randa do ktorego losuje w danym wierszu
        licznik_iteracji++;
        //--------------
        if (licznik_iteracji>50) {
            licznik_iteracji=0;
            liczba_wybranych++;
            for (int i=0 ; i<liczba_zadan ; i++) {
                if (!zadanie_uzyte[i]) {
                    zad_uzyte.push_back(i);
                    zadanie_uzyte[i]=true;
                    ostatnie_zadanie=i;
                    tablica_kolejnosci[indeks]=i;
                    indeks++;
                    break;
                }
            }
        } else {
//--------------------------

            wartosc_wylosowana= (rand()%losowana)  ;                  //(double)(rand()%losowana);
            //  cout<<"wylosowalem " << wartosc_wylosowana <<endl;
            for (int i=0; i<liczba_zadan ; i++) {
                if (!zadanie_uzyte[i] && pom_fermonowa[ostatnie_zadanie][i]>(double)wartosc_wylosowana ) {
                    zad_uzyte.push_back(i);
                    liczba_wybranych++;
                    zadanie_uzyte[i]=true;
                    ostatnie_zadanie=i;
                    tablica_kolejnosci[indeks]=i;
                    indeks++;
                    break;
                }

            }
        }
    }

}
//----------------------- KONIEC ZWRACANIA KOLEJNOSCI -------------------------


//------------------------ METAHEURYSTYKA  ----------------------
void metaheurystyka(int parametr) {
    clock_t zegar_start=clock();

    int najlepsza_wartosc = INT_MAX; //zmienna przechowujaca najlepsza wartosc funkcji celu

    int skorzystanie_z_generatora_losowego = 100; // poczatkowe prawdopodobienstwo korzystania z generatora losowego

    int iteracja=0; // zmienna liczaca iteracje petli while

    double uzupelniajaca; // wartosc o jaka uzupelniana jest macierz feromonowa

     int najlepsza_wartosc_losowego; // zmienna ktorej wartosc jest zapisywana do pliku wynikowego

      int value; // zmienna o ktora zwiekszam wartosci pol w tablicy feromonowej

    //zmienne pomocnicze do obslugi metaheurystyki
    int b;
    int rozmiarwejsciowy;
    int pomocniczyrozmiar;
    int wylosowanynumer;
    int j;


    //tworzenie uszeregowan przez generator losowy
    for(int i=0; i<liczba_generowanych_losowo; i++) {
        generator_losowy();
        }

    //wypelnianie macierzy feromonowej uszeregowaniami
    for(int i=0; i<liczba_generowanych_losowo; i++) {
        if(i%2==0) {
            value=funkcja_celu(wektor_rozw[i])+funkcja_celu(wektor_rozw[i+1]); //parametr do wartosci wypelniania macierzy zalezy od funkcji celu maszyny 1 danego uszeregowania
        }
        uzupelniajaca=((double)najlepsza_wartosc_losowego / (double)value);
        wypelnianie_macierzy_feromonowej__uszeregowania(wektor_rozw[i], uzupelniajaca );
    }

    //zapisanie najlepszego wektora rozwiazan to zmiennej
    najlepsza_wartosc_losowego=best_funkcja_celu_wektora_wektorow_losowy(wektor_rozw);

   //do wektora uszeregowan najlepszych zapisuje  najlepszego uszeregowanie ( M1 i M2 )
    best_funkcja_celu_wektora_wektorow_void();



    //glowna petla metaheurystyki
    while ((clock()-zegar_start)<czas_trwania*CLOCKS_PER_SEC) {

        iteracja++;
        if(iteracja%2==0) skorzystanie_z_generatora_losowego-=o_ile_sie_zmienia;
        if(skorzystanie_z_generatora_losowego<0) skorzystanie_z_generatora_losowego=0;

        //tworzenie nowych rozwiazan na podstawie prawdopodobienstwa
        for(int i=0; i<liczba_tworzonych_rozwiazan; i++) {

            if((rand()+1.0) < RAND_MAX * skorzystanie_z_generatora_losowego/100) {

                generator_losowy();

            } else {
                wylosowanynumer = rand()%liczba_zadan;
                zwroc_kolejnosc_macierz_feromonowa(wylosowanynumer);
                dodawanie_pokolei_M1_void(tablica_kolejnosci);
                dodawanie_pokolei_M2_void(wektor_rozw[wektor_rozw.size()-1],tablica_kolejnosci);

            }
        }

        //mutacja rozwiazan
        rozmiarwejsciowy=(wektor_rozw.size()/2);
        j= 1;
        for (int z=0; z<3; z++) {
                for(int i=0; i<rozmiarwejsciowy; i++) {
                mutacja(j-1,j);
                j+=2;
            }
            j=1;
        }

        // zostawienie 30 najlepszych uszeregowan
        turniej(30);

        //nadpisywanie wartosci
        if(najlepsza_wartosc >best_funkcja_celu_wektora_wektorow(wektor_rozw) ) {
            najlepsza_wartosc=best_funkcja_celu_wektora_wektorow(wektor_rozw);
        }

        best_funkcja_celu_wektora_wektorow_void();
        pomocniczyrozmiar= wektor_rozw.size();

        //uzupelnianie macierzy feromonowej najlepszymi rozwiazaniami po mutacji, i  turnieju
        for(int i=0; i<pomocniczyrozmiar; i++) {
            if(i%2==0) {
                value=funkcja_celu(wektor_rozw[i])+funkcja_celu(wektor_rozw[i+1]); //parametr do wartosci wypelniania macierzy zalezy od funkcji celu maszyny 1 danego uszeregowania
            }

            uzupelniajaca=((double)najlepsza_wartosc_losowego / (double)value);
            wypelnianie_macierzy_feromonowej__uszeregowania(wektor_rozw[i], uzupelniajaca);
        }

        //funkcje optymalizujace dzialanie macierzy feromonowej
        wygladzanie_tablicy_feromonowej();
        parowanie_tablica_feromonowej(tablica_feromonowa);

    }


    //tworzenie wartosci wynikowych
    int m2=najlepszy_wektor_rozwiazan.size()-1;
    int m1=najlepszy_wektor_rozwiazan.size()-2;

    // petla liczaca sume maintenensow
    int suma_maint=0;
    for (int i=1; i<najlepszy_wektor_rozwiazan[0].size(); i++) {
        if(najlepszy_wektor_rozwiazan[0][i]<0) suma_maint++;
    }

    //petla liczaca sume przerw
    int suma_przerw_M1=0;
    for (int i=1; i<najlepszy_wektor_rozwiazan[0].size(); i++) {
        if(najlepszy_wektor_rozwiazan[0][i]==0) suma_przerw_M1++;
    }
    int suma_przerw_M2=0;
    for (int i=1; i<najlepszy_wektor_rozwiazan[1].size(); i++) {
        if(najlepszy_wektor_rozwiazan[1][i]==0) suma_przerw_M2++;
    }


    dopisz_plikwynikowy(najlepszy_wektor_rozwiazan,m1,m2,najlepsza_wartosc,najlepsza_wartosc_losowego,suma_maint,0,suma_przerw_M1,suma_przerw_M2,parametr);
    cout<<" --- Dopisalem do pliku --- "<<endl;

    //czyszczenie wektorow uszeregowan
    najlepszy_wektor_rozwiazan.clear();
    wektor_rozw.clear();

}



int main() {

    srand( time( NULL ) );
    for(int i=1; i<=10; i++) {
        cout<<"Iteracja " << i<<endl;
        generator(liczba_zadan,tablica_zadan,tablica_maintenance_m1,i);
        metaheurystyka(i);
        }
//--------------- USUWANIE TABLIC DYNAMICZNYCH WIELOWYMIAROWYCH - ZWALNIANIE PAMIECI ------------
    for (int i=0; i<liczba_maintenance; i++) {
        delete [] tablica_maintenance_m1[i];
    }
    delete [] tablica_maintenance_m1;

    for (int i=0; i<liczba_zadan; i++) {
        delete [] tablica_zadan[i];
    }
    delete [] tablica_zadan;

}


