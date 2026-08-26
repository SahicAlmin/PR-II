#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

const char* PORUKA = "\n-------------------------------------------------------------------------------\n"
"0. PROVJERITE DA LI PREUZETI ZADACI PRIPADAJU VASOJ GRUPI (G1/G2)\n"
"1. SVE KLASE TREBAJU POSJEDOVATI ADEKVATAN DESTRUKTOR\n"
"2. NAMJERNO IZOSTAVLJANJE KOMPLETNIH I/ILI POJEDINIH DIJELOVA DESTRUKTORA CE BITI OZNACENO KAO TM\n"
"3. SPASAVAJTE PROJEKAT KAKO BI SE SPRIJECILO GUBLJENJE URADJENOG ZADATKA\n"
"4. ATRIBUTI, NAZIVI FUNKCIJA... (skraceno zbog preglednosti)\n";

const char* crt = "\n-------------------------------------------\n";
enum Karakteristike { NARUDZBA, KVALITET, PAKOVANJE, ISPORUKA };
ostream& operator<<(ostream& os, Karakteristike karakteristika) {
    switch (karakteristika) {
    case NARUDZBA: os << "NARUDZBA"; break;
    case KVALITET: os << "KVALITET"; break;
    case PAKOVANJE: os << "PAKOVANJE"; break;
    case ISPORUKA: os << "ISPORUKA"; break;
    default: os << "Karakteristika ne postoji";
    }
    return os;
}

char* GetNizKaraktera(const char* sadrzaj, bool dealociraj = false) {
    if (sadrzaj == nullptr)return nullptr;
    int vel = strlen(sadrzaj) + 1;
    char* temp = new char[vel];
    strcpy_s(temp, vel, sadrzaj);
    if (dealociraj) delete[]sadrzaj;
    return temp;
}

template<class T1, class T2>
class Parovi {
    T1* _elementi1;
    T2* _elementi2;
    int* _trenutno;
    bool _omoguciDupliranje;
public:
    Parovi(bool omoguciDupliranje = true) {
        _omoguciDupliranje = omoguciDupliranje;
        _trenutno = new int(0);
        _elementi1 = nullptr;
        _elementi2 = nullptr;
    }
    Parovi(const Parovi& obj) {
        _trenutno = new int(*obj._trenutno);
        _omoguciDupliranje = obj._omoguciDupliranje;
        if (*_trenutno > 0) {
            _elementi1 = new T1[*_trenutno];
            _elementi2 = new T2[*_trenutno];
            for (int i = 0; i < *_trenutno; i++) {
                _elementi1[i] = obj._elementi1[i];
                _elementi2[i] = obj._elementi2[i];
            }
        }
        else {
            _elementi1 = nullptr;
            _elementi2 = nullptr;
        }
    }
    Parovi& operator=(const Parovi& obj) {
        if (this != &obj) {
            delete[] _elementi1; _elementi1 = nullptr;
            delete[] _elementi2; _elementi2 = nullptr;
            delete _trenutno; _trenutno = nullptr;
            _trenutno = new int(*obj._trenutno);
            _omoguciDupliranje = obj._omoguciDupliranje;
            if (*_trenutno > 0) {
                _elementi1 = new T1[*_trenutno];
                _elementi2 = new T2[*_trenutno];
                for (int i = 0; i < *_trenutno; i++) {
                    _elementi1[i] = obj._elementi1[i];
                    _elementi2[i] = obj._elementi2[i];
                }
            }
            else {
                _elementi1 = nullptr;
                _elementi2 = nullptr;
            }
        }return *this;
    }
    void AddElement(const T1& el1, const T2& el2) {
        if (!_omoguciDupliranje) {
            for (int i = 0; i < getTrenutno(); i++) {
                if (getElement1(i) == el1 && getElement2(i) == el2) {
                    throw exception("Dupliranje nije omoguceno\n");
                }
            }
        }
        T1* temp1 = new T1[*_trenutno + 1];
        T2* temp2 = new T2[*_trenutno + 1];
        for (int i = 0; i < *_trenutno; i++) {
            temp1[i] = _elementi1[i];
            temp2[i] = _elementi2[i];
        }
        temp1[*_trenutno] = el1;
        temp2[*_trenutno] = el2;
        delete[] _elementi1; _elementi1 = nullptr;
        delete[] _elementi2; _elementi2 = nullptr;
        _elementi1 = temp1;
        _elementi2 = temp2;
        (*_trenutno)++;
    }
    Parovi<T1, T2>operator()(int from, int to) {
        Parovi<T1, T2>parovi;
        for (int i = 0; i < *_trenutno; i++) {
            if (i >= from && i <= to) {
                parovi.AddElement(_elementi1[i], _elementi2[i]);
            }
        }
        return parovi;
    }
    Parovi<int, int>operator()(int from, int to, bool uslov) {
        Parovi<int, int>parovi;
        if (uslov == true) {
            for (int i = to; i >= from; i--) {
                parovi.AddElement(_elementi1[i], _elementi2[i]);
            }
        }
        return parovi;
    }
    ~Parovi() {
        delete[] _elementi1; _elementi1 = nullptr;
        delete[] _elementi2; _elementi2 = nullptr;
        delete _trenutno; _trenutno = nullptr;
    }
    T1& getElement1(int lokacija)const { return _elementi1[lokacija]; }
    T2& getElement2(int lokacija)const { return _elementi2[lokacija]; }
    int getTrenutno() const { return *_trenutno; }
    int getOmoguciDupliranje() { return _omoguciDupliranje; }
    friend ostream& operator<< (ostream& COUT, const Parovi& obj) {
        for (size_t i = 0; i < *obj._trenutno; i++)
            COUT << obj.getElement1(i) << " " << obj.getElement2(i) << endl;
        return COUT;
    }
};

class Datum {
    int* _dan, * _mjesec, * _godina;
public:
    Datum(int dan = 1, int mjesec = 1, int godina = 2000) {
        _dan = new int(dan);
        _mjesec = new int(mjesec);
        _godina = new int(godina);
    }
    Datum(const Datum& obj) {
        _dan = new int(*obj._dan);
        _mjesec = new int(*obj._mjesec);
        _godina = new int(*obj._godina);
    }
    Datum& operator=(const Datum& obj) {
        if (this != &obj) {
            delete _dan; _dan = nullptr;
            delete _mjesec; _mjesec = nullptr;
            delete _godina; _godina = nullptr;
            _dan = new int(*obj._dan);
            _mjesec = new int(*obj._mjesec);
            _godina = new int(*obj._godina);
        }return *this;
    }
    ~Datum() {
        delete _dan; _dan = nullptr;
        delete _mjesec; _mjesec = nullptr;
        delete _godina; _godina = nullptr;
    }
    friend ostream& operator<< (ostream& COUT, const Datum& obj) {
        COUT << *obj._dan << "." << *obj._mjesec << "." << *obj._godina;
        return COUT;
    }
};

class ZadovoljstvoKupca {
    int _ocjena;
    Parovi<Karakteristike*, const char*>* _komentariKarakteristika;

    void Dealociraj() {
        if (_komentariKarakteristika != nullptr) {
            for (int i = 0; i < _komentariKarakteristika->getTrenutno(); i++) {
                delete _komentariKarakteristika->getElement1(i);
                delete[] _komentariKarakteristika->getElement2(i);
            }
            delete _komentariKarakteristika;
            _komentariKarakteristika = nullptr;
        }
    }
    void Kopiraj(const ZadovoljstvoKupca& obj) {
        _ocjena = obj._ocjena;
        _komentariKarakteristika = new Parovi<Karakteristike*, const char*>(false);
        for (int i = 0; i < obj._komentariKarakteristika->getTrenutno(); i++) {
            Karakteristike* kopijaKarak = new Karakteristike(*obj._komentariKarakteristika->getElement1(i));
            const char* kopijaKom = GetNizKaraktera(obj._komentariKarakteristika->getElement2(i));
            _komentariKarakteristika->AddElement(kopijaKarak, kopijaKom);
        }
    }

public:
    ZadovoljstvoKupca(int ocjena = 0) {
        _ocjena = ocjena;
        _komentariKarakteristika = new Parovi<Karakteristike*, const char*>(false);
    }
    ZadovoljstvoKupca(const ZadovoljstvoKupca& obj) {
        Kopiraj(obj);
    }
    ZadovoljstvoKupca& operator=(const ZadovoljstvoKupca& obj) {
        if (this != &obj) {
            Dealociraj();
            Kopiraj(obj);
        }return *this;
    }
    int GetOcjena() { return _ocjena; }
    Parovi<Karakteristike*, const char*>* GetKomentareKarakteristika() { return _komentariKarakteristika; }

    bool daLiJeKarakteristikaKomentarisana(Karakteristike karak) {
        for (int i = 0; i < _komentariKarakteristika->getTrenutno(); i++) {
            if (*(_komentariKarakteristika->getElement1(i)) == karak) {
                return true;
            }
        }
        return false;
    }
    bool operator==(const ZadovoljstvoKupca& obj) const {
        return _ocjena == obj._ocjena;
    }
    void DodajKomentarKarakteristike(Karakteristike karak, const char* komentar) {
        if (daLiJeKarakteristikaKomentarisana(karak)) {
            throw exception("Zadana karakteristika je vec komentarisana!");
        }
        _komentariKarakteristika->AddElement(new Karakteristike(karak), GetNizKaraktera(komentar));
    }

    ~ZadovoljstvoKupca() {
        Dealociraj();
    }
};

class Osoba {
protected:
    char* _imePrezime;
    Datum _datumRodjenja;
public:
    Osoba(const char* imePrezime = "", Datum datumRodjenja = Datum()) : _datumRodjenja(datumRodjenja) {
        _imePrezime = GetNizKaraktera(imePrezime);
    }
    Osoba(const Osoba& obj) {
        _imePrezime = GetNizKaraktera(obj._imePrezime);
        _datumRodjenja = obj._datumRodjenja;
    }
    Osoba& operator=(const Osoba& obj) {
        if (this != &obj) {
            delete[] _imePrezime; _imePrezime = nullptr;
            _imePrezime = GetNizKaraktera(obj._imePrezime);
            _datumRodjenja = obj._datumRodjenja;
        }return *this;
    }
    virtual ~Osoba() {
        delete[] _imePrezime; _imePrezime = nullptr;
    }
    char* GetImePrezime() { return _imePrezime; }
    Datum& GetDatumRodjenja() { return _datumRodjenja; }
    const Datum& GetDatumRodjenja()const { return _datumRodjenja; }

    virtual void Info() = 0;
    friend ostream& operator<< (ostream& COUT, const Osoba& obj) {
        COUT << obj._imePrezime << " " << obj._datumRodjenja << endl;
        return COUT;
    }
};

mutex ispis_mutex;

class Kupac : public Osoba {
    char* _emailAdresa;
    Parovi<float, ZadovoljstvoKupca>* _kupovine;
    vector<int> _bodovi;
public:
    Kupac(const char* imePrezime = "", Datum datumRodjenja = Datum(), const char* emailAdresa = "") : Osoba(imePrezime, datumRodjenja) {
        _emailAdresa = GetNizKaraktera(emailAdresa);
        _kupovine = new Parovi<float, ZadovoljstvoKupca>(false);
    }
    Kupac(const Kupac& obj) :Osoba(obj) {
        _emailAdresa = GetNizKaraktera(obj._emailAdresa);
        _kupovine = new Parovi<float, ZadovoljstvoKupca>(*obj._kupovine);
        _bodovi = obj._bodovi;
    }
    Kupac& operator=(const Kupac& obj) {
        if (this != &obj) {
            delete[] _emailAdresa; _emailAdresa = nullptr;
            delete _kupovine; _kupovine = nullptr;
            _bodovi.clear();
            Osoba::operator=(obj);
            _emailAdresa = GetNizKaraktera(obj._emailAdresa);
            _kupovine = new Parovi<float, ZadovoljstvoKupca>(*obj._kupovine);
            _bodovi = obj._bodovi;
        }return *this;
    }
    ~Kupac() {
        delete[] _emailAdresa; _emailAdresa = nullptr;
        delete _kupovine; _kupovine = nullptr;
    }
    char* GetEmail() { return _emailAdresa; }
    Parovi<float, ZadovoljstvoKupca>& GetKupovine() { return *_kupovine; }
    vector<int> GetBodovi() { return _bodovi; }
    const vector<int> GetBodovi() const { return _bodovi; }
    int GetBodoviUkupno() {
        int ukupno = 0;
        for (size_t i = 0; i < _bodovi.size(); i++) ukupno += _bodovi[i];
        return ukupno;
    }

    void DodajKupovinu(float iznos, ZadovoljstvoKupca& zad) {
        _kupovine->AddElement(iznos, zad);

        int ostvareniBodovi = iznos / 10;
        if (ostvareniBodovi > 0) {
            _bodovi.push_back(ostvareniBodovi);
        }

        if (ostvareniBodovi > 5) {
            int ukupniBodovi = GetBodoviUkupno();
            string email = _emailAdresa;

            thread emailThread([email, ostvareniBodovi, ukupniBodovi]() {
                this_thread::sleep_for(chrono::seconds(3)); 
                lock_guard<mutex> lck(ispis_mutex);         
                cout << "\nTo: " << email << ";\n"
                    << "Subject: Osvareni bodovi\n\n"
                    << "Postovani,\n\n"
                    << "Prilikom posljednje kupovine ste ostvarili " << ostvareniBodovi
                    << " bodova, tako da trenutno vas ukupan broj bodova iznosi " << ukupniBodovi << ".\n\n"
                    << "Zahvaljujemo vam na kupovini.\n"
                    << "Puno pozdrava\n" << crt;
                });
            emailThread.detach(); 
        }
    }

    Parovi<Karakteristike, const char*> GetKupovineByKomentar(const char* rijec) {
        Parovi<Karakteristike, const char*> rezultat(true);
        for (int i = 0; i < _kupovine->getTrenutno(); i++) {
            auto komentari = _kupovine->getElement2(i).GetKomentareKarakteristika();
            for (int j = 0; j < komentari->getTrenutno(); j++) {
                string komentarStr = komentari->getElement2(j);
                if (komentarStr.find(rijec) != string::npos) {
                    rezultat.AddElement(*komentari->getElement1(j), GetNizKaraktera(komentari->getElement2(j)));
                }
            }
        }
        return rezultat;
    }

    virtual void Info() override {
        cout << crt << _imePrezime << " " << _datumRodjenja << " " << _emailAdresa << endl;
        cout << "KUPOVINE -> " << crt;
        for (int i = 0; i < _kupovine->getTrenutno(); i++) {
            cout << "Iznos racuna: " << _kupovine->getElement1(i) << "KM, zadovoljstvo kupca: "
                << _kupovine->getElement2(i).GetOcjena() << endl;

            auto komKarak = _kupovine->getElement2(i).GetKomentareKarakteristika();
            for (int j = 0; j < komKarak->getTrenutno(); j++) {
                cout << "\t\t" << *komKarak->getElement1(j) << " - " << komKarak->getElement2(j) << endl;
            }
            cout << crt;
        }
        cout << "BODOVI -> ";
        for (size_t i = 0; i < _bodovi.size(); i++) cout << _bodovi[i] << ", ";
        cout << crt;
    }
};

void main() {

    const int ParoviTestSize = 9;

    Parovi<int, int> Parovi1(false);
    for (int i = 0; i < ParoviTestSize - 1; i++)
        Parovi1.AddElement(i, i);//dodaje vrijednosti u Parovi

    try {
        Parovi1.AddElement(3, 3);
    }
    catch (exception& err) {
        cout << err.what() << crt;
    }
    Parovi1.AddElement(9, 9);
    cout << Parovi1 << crt;

    Parovi<int, int> Parovi2 = Parovi1(2, 5);
    cout << Parovi2 << crt;
    Parovi<int, int> Parovi3 = Parovi1(2, 5, true);
    cout << Parovi3 << crt;

    Parovi3 = Parovi2;
    cout << Parovi3 << crt;


    const int maxKupaca = 3;
    Osoba* kupci[maxKupaca];
    kupci[0] = new Kupac("Denis Music", Datum(12, 1, 1980), "denis@fit.ba");
    kupci[1] = new Kupac("Jasmin Azemovic", Datum(12, 2, 1980), "jasmin@fit.ba");
    kupci[2] = new Kupac("Adel Handzic", Datum(12, 3, 1980), "adel@edu.fit.ba");

    ZadovoljstvoKupca zadovoljstvoKupca(7);
    zadovoljstvoKupca.DodajKomentarKarakteristike(NARUDZBA, "Nismo mogli odabrati sve potrebne opcije");
    zadovoljstvoKupca.DodajKomentarKarakteristike(KVALITET, "Kvalitet je ocekivan");

    try {
        //karakteristika kvaliteta je vec komentarisana, pa je potrebno baciti izuzetak
        zadovoljstvoKupca.DodajKomentarKarakteristike(KVALITET, "Kvalitet je ocekivan");
    }
    catch (exception& err) {
        cout << err.what() << crt;
    }

    zadovoljstvoKupca.DodajKomentarKarakteristike(PAKOVANJE, "Pakovanje je bilo osteceno");
    zadovoljstvoKupca.DodajKomentarKarakteristike(ISPORUKA, "Mada su najavili da ce proizvod biti isporucen u roku od 2 dana, cekali smo 5 dana");

    Kupac* denis = dynamic_cast<Kupac*>(kupci[0]);
    /*za svakih 10KM kupcu se dodaje 1 bod, pa ce tako kupovina od 128KM kupcu donijeti 12 bodova*/
    denis->DodajKupovinu(128, zadovoljstvoKupca);
    cout << "Ukupno bodova -> " << denis->GetBodoviUkupno() << endl;//12 bodova


    ZadovoljstvoKupca zadovoljstvoKupca2(4);
    zadovoljstvoKupca2.DodajKomentarKarakteristike(KVALITET, "Jako lose, proizvod ostecen");
    denis->DodajKupovinu(81, zadovoljstvoKupca2);

    cout << "Ukupno bodova -> " << denis->GetBodoviUkupno() << endl;//20 bodova

    /*prilikom svake kupovine, ukoliko je kupac ostvario vise od 5 bodova, potrebno je, u zasebnom thread-u (nakon 3 sekunde), poslati email sa sljedecim sadrzajem:

      To: denis@fit.ba;
      Subject: Osvareni bodovi

      Postovani,

      Prilikom posljednje kupovine ste ostvarili 8 bodova, tako da trenutno vas ukupan broj bodova iznosi 20.

      Zahvaljujemo vam na kupovini.
      Puno pozdrava
 */
    // ispisuje sve podatke o kupcu i njegovim kupovinama.
    denis->Info();
    /* Primjer ispisa:
        -------------------------------------------
        Denis Music 12.1.1980 denis@fit.ba
        KUPOVINE ->
        -------------------------------------------
        Iznos racuna: 128KM, zadovoljstvo kupca: 7
                  NARUDZBA - Nismo mogli odabrati sve potrebne opcije
                  KVALITET - Kvalitet je ocekivan
                  PAKOVANJE - Pakovanje je bilo osteceno
                  ISPORUKA - Mada su najavili da ce proizvod biti isporucen u roku od 2 dana, cekali smo 5 dana
        -------------------------------------------
        Iznos racuna: 81KM, zadovoljstvo kupca: 4
                  KVALITET - Jako lose, proizvod ostecen
        -------------------------------------------
        BODOVI -> 12, 8,
        -------------------------------------------
 */
    // metoda GetKupovineByKomentar pronadje i vrati sve karakteristike i komentare koji sadrze rijec
    Parovi<Karakteristike, const char*> osteceniProizvodi = denis->GetKupovineByKomentar("ostecen");
    cout << crt << "Rezultat pretrage -> " << crt << osteceniProizvodi << crt;
    /*Ocekivani ispis:
       -------------------------------------------
       Rezultat pretrage ->
       -------------------------------------------
       PAKOVANJE Pakovanje je bio osteceno
       KVALITET Jako lose, proizvod ostecen
       -------------------------------------------
  */

    for (size_t i = 0; i < maxKupaca; i++)
        delete kupci[i], kupci[i] = nullptr;

    this_thread::sleep_for(chrono::seconds(4));
    cout << "Kraj programa!" << endl;

    cin.get();
    system("pause>0");
}