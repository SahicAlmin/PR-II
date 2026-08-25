#include <iostream>
#include<string>	
#include<regex>
#include<vector>
#include<sstream>
#include<thread>
#include<chrono>
#include<mutex>
#include<iomanip>
using namespace std;

string crt = "\n---------------------------------------------------------------------------------\n";

string PORUKA_TELEFON = crt +
"TELEFONE ISKLJUCITE I ODLOZITE U TORBU, DZEP ILI DRUGU LOKACIJU VAN DOHVATA.\n"
"CESTO SE NA TELEFONIMA (PRO)NALAZE PROGRAMSKI KODOVI KOJI MOGU BITI ISKORISTENI ZA\n"
"RJESAVANJE ISPITNOG ZADATKA, STO CE, U SLUCAJU PRONALASKA, BITI SANKCIONISANO.\n\n"
"SVI KOD KOJIH BUDE PRONADJEN TELEFON ILI NEKI DRUGI UREDJAJ KOJI MOZE SADRZAVATI\n"
"MATERIJAL ILI RJESENJA ISPITNIH ZADATAKA CE BITI UDALJENI SA ISPITA I BIT CE POKRENUT\n"
"POSTUPAK PROTIV NJIH" + crt;

string PORUKA_ISPIT = crt +
"0. PROVJERITE DA LI ZADACI PRIPADAJU VASOJ GRUPI (G1/G2)\n"
"1. SVE KLASE SA DINAMICKOM ALOKACIJOM MORAJU IMATI ISPRAVAN DESTRUKTOR\n"
"2. IZOSTAVLJANJE DESTRUKTORA ILI NJEGOVIH DIJELOVA BIT CE OZNACENO KAO TM\n"
"3. ATRIBUTI, METODE I PARAMETRI MORAJU BITI IDENTICNI ONIMA U TESTNOJ MAIN FUNKCIJI,\n"
" OSIM AKO POSTOJI JASNO OPISAN RAZLOG ZA MODIFIKACIJU\n"
"4. IZUZETKE BACAJTE SAMO TAMO GDJE JE IZRICITO NAGLASENO\n"
"5. SVE METODE KOJE SE POZIVAJU U MAIN-U MORAJU POSTOJATI.\n"
" AKO NEMATE ZELJENU IMPLEMENTACIJU, OSTAVITE PRAZNO TIJELO ILI VRATITE DEFAULT VRIJEDNOST\n"
"6. U MAIN FUNKCIJI MOZETE DODAVATI TESTNE PODATKE I POZIVE PO VLASTITOM IZBORU\n"
"7. TESTIRAJTE PROGRAM U OBA MODA (F5 i Ctrl+F5)" + crt;

char* AlocirajTekst(const char* tekst) {
	if (tekst == nullptr) return nullptr;
	size_t velicina = strlen(tekst) + 1;
	char* noviTekst = new char[velicina];
	strcpy_s(noviTekst, velicina, tekst);
	return noviTekst;
}
mutex mtx;
enum VrstaDostave { STANDARDNA, BRZA, MEDJUNARODNA };
const char* VrstaDostaveOznake[] = { "ST", "BR", "MD" };
const char* VrstaDostaveNazivi[] = { "STANDARDNA", "BRZA", "MEDJUNARODNA" };
enum StatusPosiljke { KREIRANA, PREUZETA, U_TRANZITU, ISPORUCENA, OTKAZANA };
const char* StatusPosiljkeNazivi[] = { "KREIRANA", "PREUZETA", "U TRANZITU", "ISPORUCENA", "OTKAZANA" };

string GenerisiOznaku(const char* imePrezime, int redniBroj, VrstaDostave vrstaDostave) {
	if (vrstaDostave < 0 || vrstaDostave>2 || redniBroj < 1 || redniBroj>9999) {
		return "BH-XX-0000-XX";
	}
	string inicijali = imePrezime;
	if (inicijali.find(" ") == string::npos) {
		return "BH-XX-0000-XX";
	}
	string oznaka;
	oznaka += "BH-";
	oznaka += VrstaDostaveOznake[vrstaDostave];
	oznaka += "-";
	if (redniBroj < 10) {
		oznaka += "000";
	}
	else if (redniBroj < 100) {
		oznaka += "00";
	}
	else if (redniBroj < 1000) {
		oznaka += "0";
	}
	oznaka += to_string(redniBroj);
	oznaka += "-";
	oznaka += std::toupper(inicijali[0]);
	oznaka += std::toupper(inicijali[inicijali.find_last_of(" ") + 1]);
	return oznaka;
}
/*
	Koristeci regex, funkcija ValidirajOznaku provjerava da li je oznaka
	zapisana u prethodno definisanom formatu. Dozvoljene su iskljucivo oznake
	vrsta ST, BR i MD, redni broj mora imati cetiri cifre i ne moze biti 0000,
	a inicijali moraju biti zapisani velikim slovima.
	Potpis funkcije treba biti:
	bool ValidirajOznaku(const string& oznaka)
	*/
bool ValidirajOznaku(const string& oznaka) {
	regex test("BH-(ST|BR|MD)-(?!0000)\\d{4}-[A-Z]{2}"); 
	return regex_match(oznaka, test); 
}
template<class T1, class T2, int max>
class Kolekcija {
	T1* _elementi1[max];
	T2* _elementi2[max];
	int _trenutno;
public:
	Kolekcija() : _trenutno(0) {
		for (int i = 0; i < max; i++) {
			_elementi1[i] = nullptr;
			_elementi2[i] = nullptr;
		}
	}
	Kolekcija(const Kolekcija& obj) {
		_trenutno = obj._trenutno; 
		for (int i = 0; i < _trenutno; i++)
		{
			_elementi1[i] = new T1(*obj._elementi1[i]); 
			_elementi2[i] = new T2(*obj._elementi2[i]); 
		}
	}
	Kolekcija& operator=(const Kolekcija& obj) {
		if (this != &obj) {
			for (int i = 0; i < _trenutno; i++) {
				delete _elementi1[i];
				delete _elementi2[i];
				_elementi1[i] = nullptr;
				_elementi2[i] = nullptr;
			}
			_trenutno = 0;
			_trenutno = obj._trenutno;
			for (int i = 0; i < _trenutno; i++)
			{
				_elementi1[i] = new T1(*obj._elementi1[i]);
				_elementi2[i] = new T2(*obj._elementi2[i]);
			}
		}return *this; 
	}
	void Dodaj(const T1& el1, const T2& el2) {
		_elementi1[_trenutno] = new T1(el1); 
		_elementi2[_trenutno] = new T2(el2);
		_trenutno++;
	}
	Kolekcija& DodajNaPoziciju(int lokacija, const T1& el1, const T2& el2) {
		if (_trenutno >= max) throw exception("Kolekcija je puna\n"); 
		if (lokacija<0 || lokacija>_trenutno) throw exception("Lokacija nije validna\n"); 
		for (int i = _trenutno; i > lokacija; i--)
		{
			_elementi1[i] = _elementi1[i - 1];
			_elementi2[i] = _elementi2[i - 1];
		}
		_elementi1[lokacija] = new T1(el1); 
		_elementi2[lokacija] = new T2(el2);
		_trenutno++; 
		return *this; 
	}
	pair<T1,T2>UkloniSaPozicije(int pozicija) {
		if (pozicija<0 || pozicija>_trenutno) throw exception("Range invalid\n"); 
		pair<T1, T2>uklonjeni; 
		uklonjeni.first = *_elementi1[pozicija]; 
		uklonjeni.second = *_elementi2[pozicija]; 
		delete _elementi1[pozicija]; _elementi1[pozicija] = nullptr; 
		delete _elementi2[pozicija]; _elementi2[pozicija] = nullptr;
		for (int i = pozicija; i < _trenutno-1; i++)
		{
			_elementi1[i] = _elementi1[i + 1]; 
			_elementi2[i] = _elementi2[i + 1]; 
		}
		_trenutno--; 
		return uklonjeni; 
 	}
	~Kolekcija() {
		for (int i = 0; i < _trenutno; i++) {
			delete _elementi1[i];
			delete _elementi2[i];
			_elementi1[i] = nullptr;
			_elementi2[i] = nullptr;
		}
		_trenutno = 0;
	}
	int GetTrenutno() const { return _trenutno; }
	T1& GetElement1(int indeks) { return *_elementi1[indeks]; }
	T2& GetElement2(int indeks) { return *_elementi2[indeks]; }
	const T1& GetElement1(int indeks) const { return *_elementi1[indeks]; }
	const T2& GetElement2(int indeks) const { return *_elementi2[indeks]; }
	T1& operator[](int indeks) { return *_elementi1[indeks]; }
	friend ostream& operator<<(ostream& COUT, const Kolekcija& obj) {
		for (int i = 0; i < obj.GetTrenutno(); i++)
			COUT << obj.GetElement1(i) << " " << obj.GetElement2(i) << endl;
		return COUT;
	}
};

class DatumVrijeme {
	int* _dan, * _mjesec, * _godina, * _sati, * _minute;
public:
	DatumVrijeme(int dan = 1, int mjesec = 1, int godina = 2000,
		int sati = 0, int minute = 0) {
		_dan = new int(dan);
		_mjesec = new int(mjesec);
		_godina = new int(godina);
		_sati = new int(sati);
		_minute = new int(minute);
	}
	DatumVrijeme(const DatumVrijeme& obj) {
		_dan = new int(*obj._dan);
		_mjesec = new int(*obj._mjesec);
		_godina = new int(*obj._godina);
		_sati = new int(*obj._sati);
		_minute = new int(*obj._minute);
	}
	DatumVrijeme& operator=(const DatumVrijeme& obj) {
		if (this != &obj) {
			delete _dan;
			delete _mjesec;
			delete _godina;
			delete _sati;
			delete _minute;
			_dan = _mjesec = _godina = _sati = _minute = nullptr;
			_dan = new int(*obj._dan);
			_mjesec = new int(*obj._mjesec);
			_godina = new int(*obj._godina);
			_sati = new int(*obj._sati);
			_minute = new int(*obj._minute);
		}return *this;
	}
	string ToString()const {
		stringstream ss{};
		ss << setfill('0') << setw(2) << *_dan << "." << setw(2) << *_mjesec << "." << setw(4) << *_godina << " " << setw(2) << *_sati << ":" << setw(2) << *_minute << " ";
		return ss.str(); 
	}
	bool operator==(const DatumVrijeme& obj)const {
		return *_godina == *obj._godina
			&& *_mjesec == *obj._mjesec
			&& *_dan == *obj._dan
			&& *_sati == *obj._sati
			&& *_minute == *obj._minute; 
   }
	bool operator>(const DatumVrijeme& obj)const {
		if (*_godina != *obj._godina) return *_godina > *obj._godina; 
		if (*_mjesec != *obj._mjesec) return *_mjesec > *obj._mjesec; 
		if (*_dan != *obj._dan) return *_dan > *obj._dan; 
		if (*_sati != *obj._sati) return *_sati > *obj._sati; 
		return *_minute > *obj._minute; 
	}
	friend ostream& operator<<(ostream& COUT, const DatumVrijeme& obj)
	{
		//ToString vraca datum i vrijeme u formatu DD.MM.GGGG HH:MM,
		COUT << obj.ToString();
		return COUT;
	}
	~DatumVrijeme() {
		delete _dan;
		delete _mjesec;
		delete _godina;
		delete _sati;
		delete _minute;
		_dan = _mjesec = _godina = _sati = _minute = nullptr;
	}
};

class Posiljka {
	char* imePrezimePrimaoca;
	char* _oznaka;
	char* sadrzaj;
	char* emailPrimaoca;
	VrstaDostave vrstaDostave;
	int redniBroj;
	Kolekcija<StatusPosiljke, DatumVrijeme, 10> _statusi;

	void Dealociraj() {
		delete[] _oznaka;
		delete[] sadrzaj;
		delete[] imePrezimePrimaoca;
		delete[] emailPrimaoca;
		_oznaka = sadrzaj = imePrezimePrimaoca = emailPrimaoca = nullptr;
	}
public:
	Posiljka(const char* _sadrzaj, const char* imePrezime, const char* email, VrstaDostave vrsta, int broj, DatumVrijeme vrijeme) {
		_oznaka = AlocirajTekst(GenerisiOznaku(imePrezime, broj, vrsta).c_str()); 
		sadrzaj = AlocirajTekst(_sadrzaj); 
		imePrezimePrimaoca = AlocirajTekst(imePrezime); 
		emailPrimaoca = AlocirajTekst(email); 
		vrstaDostave = vrsta; 
		redniBroj = broj; 
		_statusi.Dodaj(KREIRANA, vrijeme);
	}
	Posiljka(const Posiljka& obj) {
		_oznaka = AlocirajTekst(obj._oznaka); 
		imePrezimePrimaoca = AlocirajTekst(obj.imePrezimePrimaoca); 
		sadrzaj = AlocirajTekst(obj.sadrzaj); 
		emailPrimaoca = AlocirajTekst(obj.emailPrimaoca); 
		vrstaDostave = obj.vrstaDostave; 
		redniBroj = obj.redniBroj; 
		_statusi = obj._statusi; 
	}
	Posiljka& operator=(const Posiljka& obj) {
		if (this != &obj) {
			delete[] _oznaka;
			delete[] sadrzaj;
			delete[] imePrezimePrimaoca;
			delete[] emailPrimaoca;
			_oznaka = sadrzaj = imePrezimePrimaoca = emailPrimaoca = nullptr;
			_oznaka = AlocirajTekst(obj._oznaka);
			imePrezimePrimaoca = AlocirajTekst(obj.imePrezimePrimaoca);
			sadrzaj = AlocirajTekst(obj.sadrzaj);
			emailPrimaoca = AlocirajTekst(obj.emailPrimaoca);
			vrstaDostave = obj.vrstaDostave;
			redniBroj = obj.redniBroj;
			_statusi = obj._statusi;
		}return *this;
	}
	string ToString()const {
		stringstream ss{};
		ss << GetOznaka() << " | " << GetImePrezimePrimaoca() << " | " << VrstaDostaveNazivi[GetVrstaDostave()] << " | " << GetSadrzaj() << " | " <<StatusPosiljkeNazivi[GetTrenutniStatus()]; 
		return ss.str(); 
	}
	/*
	DodajStatus dodaje status samo ako je vrijeme vece od vremena
	posljednjeg statusa i ako je promjena statusa dozvoljena.
	Dozvoljen slijed statusa je: KREIRANA -> PREUZETA -> U_TRANZITU -> ISPORUCENA
	OTKAZANA se moze evidentirati iz bilo kojeg statusa koji nije zavrsni.
	Nakon statusa ISPORUCENA ili OTKAZANA nisu dozvoljene nove promjene.
	Preskakanje, ponavljanje statusa i hronoloski neispravno vrijeme vracaju
	false, bez izmjene kolekcije statusa.
	*/
	bool DodajStatus(StatusPosiljke status, DatumVrijeme vrijeme) {
		DatumVrijeme zadnjeVrijeme= _statusi.GetElement2(_statusi.GetTrenutno() - 1);
		StatusPosiljke zadnjiStatus = GetTrenutniStatus(); 
		StatusPosiljke novi = status; 
		if (vrijeme > zadnjeVrijeme) {
			if (zadnjiStatus == OTKAZANA || zadnjiStatus == ISPORUCENA) {
				return false;
			}
			if (novi == OTKAZANA) {
				_statusi.Dodaj(novi, vrijeme); 
				return true; 
			}
			if (zadnjiStatus == KREIRANA && novi == PREUZETA) {
				_statusi.Dodaj(novi, vrijeme); 
				return true; 
			}
			if (zadnjiStatus == PREUZETA && novi == U_TRANZITU) {
				_statusi.Dodaj(novi, vrijeme);
				return true;
			}
			if (zadnjiStatus == U_TRANZITU && novi == ISPORUCENA) {
				_statusi.Dodaj(novi, vrijeme);
				return true;
			}
		}
	
		return false; 
	}
	const char* GetOznaka() const { return _oznaka; }
	const char* GetSadrzaj() const { return sadrzaj; }
	const char* GetImePrezimePrimaoca() const { return imePrezimePrimaoca; }
	const char* GetEmailPrimaoca() const { return emailPrimaoca; }
	VrstaDostave GetVrstaDostave() const { return vrstaDostave; }
	int GetRedniBroj() const { return redniBroj; }
	Kolekcija<StatusPosiljke, DatumVrijeme, 10>& GetStatusi() { return _statusi; }
	StatusPosiljke GetTrenutniStatus() const {
		return _statusi.GetElement1(_statusi.GetTrenutno() - 1);
	}
	friend ostream& operator<<(ostream& COUT, const Posiljka& obj) {
		// ToString vraca podatke u formatu:
		// oznaka | ime i prezime | vrsta dostave | sadrzaj | trenutni status
		//// BH-BR-0042-AB | Amina Buric | BRZA | Dokumenti | KREIRANA
		COUT << obj.ToString();
		return COUT;
	}
	~Posiljka() {
		Dealociraj();
	}
};
void PosaljiObavjest(string imePrezime, string email, string oznaka, StatusPosiljke status, DatumVrijeme vrijeme) {
	lock_guard<mutex>lock(mtx); 
	cout << crt; 
	cout << "To: " << email<<"\n";
	cout << "From:info@brzaposta.ba\n"; 
	cout << "Subject: Promjena statusa posiljke\n"; 
	cout << "Postovani " << imePrezime << "\n"; 
	cout << "Posiljka " << oznaka << " je promijenila status u " << StatusPosiljkeNazivi[status]<< "\n"; 
	cout << "Vrijeme evidentiranja " << vrijeme << "\n"; 
	cout << "Hvala sto koristite nase usluge.\n"; 
	cout << "BrzaPosta Tim" << crt; 
}
class DostavnaSluzba {
	char* _naziv;
	vector<Posiljka> _posiljke;
public:
	DostavnaSluzba(const char* naziv = "") {
		_naziv = AlocirajTekst(naziv);
	}
	DostavnaSluzba(const DostavnaSluzba& obj) {
		_naziv = AlocirajTekst(obj._naziv); 
		_posiljke = obj._posiljke; 
	}
	DostavnaSluzba& operator=(const DostavnaSluzba& obj) {
		if (this != &obj) {
			delete[] _naziv;
			_naziv = nullptr;
			_naziv = AlocirajTekst(obj._naziv);
			_posiljke = obj._posiljke;
		}return *this; 
	}
	void DodajPosiljku(const Posiljka& posiljka) {
		for (int i = 0; i < _posiljke.size(); i++)
		{
			if (strcmp(_posiljke[i].GetOznaka(), posiljka.GetOznaka()) == 0 || _posiljke[i].GetRedniBroj() == posiljka.GetRedniBroj()) {
				throw exception("Posiljka vec dodana\n"); 
			}
		}
		_posiljke.push_back(posiljka); 
	}
	Posiljka* PronadjiPosiljku(string oznaka) {
		for (int i = 0; i < _posiljke.size(); i++)
		{
			if (strcmp(_posiljke[i].GetOznaka(), oznaka.c_str()) == 0) {
				return &_posiljke[i]; 
			}
		}
		return nullptr; 
	}
	/*
	EvidentirajStatus pronalazi posiljku na osnovu oznake i pokusava dodati
	novi status koristeci pravila metode DodajStatus.
	Nakon svakog uspjesno evidentiranog statusa potrebno je u zasebnom threadu
	poslati obavijest primaocu. Obavijest se ne salje ako posiljka ne
	postoji ili status nije dodan, te u tim slucajevima metoda vraca false.
	Primjer sadrzaja obavijesti:*/
	bool EvidentirajStatus(string oznaka, StatusPosiljke status, DatumVrijeme vrijeme) {
		for (int i = 0; i < _posiljke.size(); i++)
		{
			if (strcmp(_posiljke[i].GetOznaka(), oznaka.c_str()) == 0) {
				if (_posiljke[i].DodajStatus(status, vrijeme)) {
					string imePrezime = _posiljke[i].GetImePrezimePrimaoca(); 
					string email = _posiljke[i].GetEmailPrimaoca();  
					thread t(PosaljiObavjest, imePrezime, email, oznaka, status , vrijeme); 
					t.detach(); 
					this_thread::sleep_for(chrono::milliseconds(100)); 
					return true; 
				}
			}
		}
		return false; 
	}
	vector<Posiljka*> PosiljkePoStatusu(StatusPosiljke status) {
		vector<Posiljka*>posiljke; 
		for (int i = 0; i < _posiljke.size(); i++)
		{
			if (_posiljke[i].GetTrenutniStatus() == status) {
				posiljke.push_back(&_posiljke[i]); 
			}
		}
		return posiljke; 
	}
	/*
	StatistikaPoStatusu vraca kolekciju koja za svaki status, redoslijedom
	definisanim enumom StatusPosiljke, sadrzi status i broj posiljki koje se
	trenutno nalaze u tom statusu. U rezultat se dodaje svih pet statusa,
	ukljucujuci i one za koje je broj posiljki jednak nuli.
	*/
	Kolekcija<StatusPosiljke, int, 5> StatistikaPoStatusu() {
		Kolekcija<StatusPosiljke, int, 5> rezultat; 
		for (int i = 0; i < 5; i++)
		{
			StatusPosiljke st = (StatusPosiljke)i;
			int brojac = 0;
			for (int j = 0; j < _posiljke.size(); j++) {
				if (_posiljke[j].GetTrenutniStatus() == st) {
					brojac++;
				}
			}
			rezultat.Dodaj(st, brojac);
		}
		return rezultat; 
	}
	const char* GetNaziv() const { return _naziv; }
	vector<Posiljka>& GetPosiljke() { return _posiljke; }
	const vector<Posiljka>& GetPosiljke()const { return _posiljke; }
	~DostavnaSluzba() {
		delete[] _naziv;
		_naziv = nullptr;
	}
};

//const char* GetOdgovorNaPrvoPitanje() {
//	cout << "Pitanje -> Pojasnite da li se svako nasljedjivanje moze posmatrati kao polimorfizam i obrnuto? Navedite konkretne primjere.\n";
//	return "Odgovor -> OVDJE UNESITE VAS ODGOVOR";
//}
//
//const char* GetOdgovorNaDrugoPitanje() {
//	cout << "Pitanje -> Pojasnite razloge kojima bi se implementator klase mogao voditi pri donosenju odluke da sve metode u klasi (ne)proglasi virtualnim?\n";
//	return "Odgovor -> OVDJE UNESITE VAS ODGOVOR";
//}

int main() {
	//cout << PORUKA_TELEFON; cin.get();
	//cout << PORUKA_TELEFON; cin.get();
	//cout << PORUKA_ISPIT;
	//cin.get();
	//system("cls");
	//cout << GetOdgovorNaPrvoPitanje() << crt;
	//cin.get();
	//cout << GetOdgovorNaDrugoPitanje() << crt;
	//cin.get();

	/*
	Funkcija GenerisiOznaku generise oznaku posiljke u formatu: BH-VD-BBBB-IN
	Potpis funkcije treba biti:
	string GenerisiOznaku(const char* imePrezime, int redniBroj, VrstaDostave vrstaDostave)
	BH -> fiksna oznaka drzave,
	VD -> vrsta dostave: ST (standardna), BR (brza), MD (medjunarodna),
	BBBB -> redni broj posiljke popunjen nulama na slobodnim mjestima,
	IN -> inicijali imena i prezimena primaoca.
	Kod imena koje sadrzi vise rijeci koriste se inicijal prve i posljednje rijeci. Redni broj mora biti u rasponu 1-9999.
	Za neispravne podatke funkcija vraca "BH-XX-0000-XX".
	*/
	if (GenerisiOznaku("Amina Buric", 42, BRZA) == "BH-BR-0042-AB")
		cout << "Oznaka OK" << crt;
	if (GenerisiOznaku("Goran Skondric", 7, STANDARDNA) == "BH-ST-0007-GS")
		cout << "Oznaka OK" << crt;
	if (GenerisiOznaku("Ana Marija Kovac", 156, MEDJUNARODNA) == "BH-MD-0156-AK")
		cout << "Oznaka OK" << crt;
	if (GenerisiOznaku("Amina", 42, BRZA) == "BH-XX-0000-XX" &&
		GenerisiOznaku("Amina Buric", 0, BRZA) == "BH-XX-0000-XX" &&
		GenerisiOznaku("Amina Buric", 10000, BRZA) == "BH-XX-0000-XX" &&
		GenerisiOznaku("Amina Buric", 42, (VrstaDostave)99) == "BH-XX-0000-XX")
		cout << "Neispravni podaci za oznaku OK" << crt;

	/*
	Koristeci regex, funkcija ValidirajOznaku provjerava da li je oznaka
	zapisana u prethodno definisanom formatu. Dozvoljene su iskljucivo oznake
	vrsta ST, BR i MD, redni broj mora imati cetiri cifre i ne moze biti 0000,
	a inicijali moraju biti zapisani velikim slovima.
	Potpis funkcije treba biti:
	bool ValidirajOznaku(const string& oznaka)
	*/
	if (ValidirajOznaku("BH-BR-0042-AB"))
		cout << "OZNAKA VALIDNA" << crt;
	if (!ValidirajOznaku("BH-EX-0042-AB") &&
		!ValidirajOznaku("BH-BR-042-AB") &&
		!ValidirajOznaku("BH-BR-0042-Ab") &&
		!ValidirajOznaku("BH-BR-0000-AB") &&
		!ValidirajOznaku("bh-BR-0042-AB"))
		cout << "OZNAKA NIJE VALIDNA" << crt;
	if (ValidirajOznaku(GenerisiOznaku("Maid Ramic", 284, BRZA)))
		cout << "GENERISANA OZNAKA VALIDNA" << crt;

	Kolekcija<int, string, 5> brojevi;
	brojevi.Dodaj(10, "Deset");
	brojevi.Dodaj(20, "Dvadeset");
	brojevi.Dodaj(30, "Trideset");
	cout << brojevi << crt;

	/*
	DodajNaPoziciju dodaje novi par na lokaciju/indeks definisanu prvim parametrom,
	pomjera postojece pokazivace udesno i vraca trenutno stanje kolekcije (izmijenjeni tj. prosireni objekat).
	U slucaju popunjene kolekcije ili neispravne lokacije potrebno je baciti izuzetak.
	*/
	Kolekcija<int, string, 5> prosireniBrojevi = brojevi.DodajNaPoziciju(1, 15, "Petnaest");
	cout << prosireniBrojevi << crt;

	/*
	UkloniSaPozicije uklanja par sa proslijedjene pozicije/indeksa, preostale elemente
	pomjera ulijevo i vraca uklonjeni par u obliku pair<T1, T2>.
	Za poziciju izvan opsega baca se izuzetak.
	*/
	pair<int, string> uklonjeni = prosireniBrojevi.UkloniSaPozicije(2);
	cout << "Uklonjeno: " << uklonjeni.first << " " << uklonjeni.second << crt;
	cout << "Preostali elementi:" << crt << prosireniBrojevi;

	try {
		prosireniBrojevi.UkloniSaPozicije(10);
	}
	catch (exception& e) {
		cout << "Exception: " << e.what() << crt;
	}

	try {
		Kolekcija<int, string, 3> punaKolekcija;
		punaKolekcija.Dodaj(1, "Jedan");
		punaKolekcija.Dodaj(2, "Dva");
		punaKolekcija.Dodaj(3, "Tri");
		punaKolekcija.DodajNaPoziciju(1, 4, "Cetiri");
	}
	catch (exception& e) {
		cout << "Exception: " << e.what() << crt;
	}

	Kolekcija<int, string, 5> kopijaBrojeva = brojevi;
	kopijaBrojeva[0] = 100;
	Kolekcija<int, string, 5> dodijeljeniBrojevi;
	dodijeljeniBrojevi = brojevi;
	dodijeljeniBrojevi[1] = 200;
	cout << "Original:" << crt << brojevi;
	cout << "Kopija:" << crt << kopijaBrojeva;
	cout << "Dodijeljeni objekat:" << crt << dodijeljeniBrojevi;

	DatumVrijeme kreirana(1, 7, 2026, 8, 0);
	DatumVrijeme preuzeta(1, 7, 2026, 9, 0);
	DatumVrijeme tranzit(1, 7, 2026, 10, 0);
	DatumVrijeme isporucena(1, 7, 2026, 11, 0);

	/*
	ToString vraca datum i vrijeme u formatu DD.MM.GGGG HH:MM, ukljucujuci pocetne nule.
	*/
	cout << kreirana.ToString() << crt; //01.07.2026 08:00
	if (preuzeta > kreirana)
		cout << "Vrijeme preuzimanja je nakon vremena kreiranja" << crt;
	DatumVrijeme kopijaPreuzeta(preuzeta);
	if (kopijaPreuzeta == preuzeta && !(kreirana == preuzeta))
		cout << "Provjera vremena, OK." << crt;

	/*
	Konstruktor Posiljke generise oznaku i evidentira pocetni status KREIRANA
	sa proslijedjenim vremenom kreiranja.
	*/
	Posiljka dokumenti("Dokumenti", "Amina Buric", "amina@fit.ba", BRZA, 42, kreirana);
	Posiljka knjige("Knjige", "Goran Skondric", "goran@fit.ba", STANDARDNA, 7, kreirana);
	Posiljka oprema("Oprema", "Ana Marija Kovac", "ana@fit.ba", MEDJUNARODNA, 156, kreirana);

	/*
	ToString vraca podatke u formatu:
	oznaka | ime i prezime | vrsta dostave | sadrzaj | trenutni status
	BH-BR-0042-AB | Amina Buric | BRZA | Dokumenti | KREIRANA
	*/
	cout << dokumenti.ToString() << crt;
	if (dokumenti.ToString() == "BH-BR-0042-AB | Amina Buric | BRZA | Dokumenti | KREIRANA")
		cout << "Posiljka ToString OK" << crt;

	/*
	DodajStatus dodaje status samo ako je vrijeme vece od vremena
	posljednjeg statusa i ako je promjena statusa dozvoljena.
	Dozvoljen slijed statusa je: KREIRANA -> PREUZETA -> U_TRANZITU -> ISPORUCENA
	OTKAZANA se moze evidentirati iz bilo kojeg statusa koji nije zavrsni.
	Nakon statusa ISPORUCENA ili OTKAZANA nisu dozvoljene nove promjene.
	Preskakanje, ponavljanje statusa i hronoloski neispravno vrijeme vracaju
	false, bez izmjene kolekcije statusa.
	*/
	if (dokumenti.DodajStatus(PREUZETA, preuzeta))
		cout << "Status PREUZETA dodan" << crt;
	if (dokumenti.DodajStatus(U_TRANZITU, tranzit))
		cout << "Status U TRANZITU dodan" << crt;
	if (dokumenti.DodajStatus(ISPORUCENA, isporucena))
		cout << "Status ISPORUCENA dodan" << crt;
	if (!dokumenti.DodajStatus(OTKAZANA, isporucena))
		cout << "Promjena zavrsnog statusa nije dozvoljena" << crt;
	if (!knjige.DodajStatus(U_TRANZITU, preuzeta))
		cout << "Preskakanje statusa nije dozvoljeno" << crt;
	if (oprema.DodajStatus(OTKAZANA, preuzeta))
		cout << "Posiljka otkazana" << crt;

	Posiljka kopijaDokumenata = dokumenti;
	cout << kopijaDokumenata << crt;

	DostavnaSluzba brzaPosta("Brza posta");

	/*
	DodajPosiljku dodaje posiljku u dostavnu sluzbu. Nije dozvoljeno dodati
	dvije posiljke sa istim rednim brojem ili istom oznakom. U slucaju
	duplikata metoda baca izuzetak.
	*/
	brzaPosta.DodajPosiljku(knjige);
	brzaPosta.DodajPosiljku(oprema);
	try {
		brzaPosta.DodajPosiljku(knjige);
	}
	catch (exception& e) {
		cout << "Exception: " << e.what() << crt;
	}

	/*
	PronadjiPosiljku vraca pokazivac na posiljku sa proslijedjenom oznakom.
	Ako posiljka nije pronadjena, metoda vraca nullptr.
	*/
	string oznakaKnjige = knjige.GetOznaka();
	Posiljka* pronadjena = brzaPosta.PronadjiPosiljku(oznakaKnjige);
	if (pronadjena != nullptr)
		cout << "Pronadjena posiljka: " << pronadjena->GetOznaka() << crt;
	if (brzaPosta.PronadjiPosiljku("BH-ST-9999-XX") == nullptr)
		cout << "Nepostojeca posiljka nije pronadjena" << crt;

	/*
	EvidentirajStatus pronalazi posiljku na osnovu oznake i pokusava dodati
	novi status koristeci pravila metode DodajStatus.
	Nakon svakog uspjesno evidentiranog statusa potrebno je u zasebnom threadu
	poslati obavijest primaocu. Obavijest se ne salje ako posiljka ne
	postoji ili status nije dodan, te u tim slucajevima metoda vraca false.
	Primjer sadrzaja obavijesti:
	---------------------------------------------------------------------------------
	To: goran@fit.ba
	From: info@brzaposta.ba
	Subject: Promjena statusa posiljke
	Postovani Goran Skondric,
	Posiljka BH-ST-0007-GS je promijenila status u PREUZETA.
	Vrijeme evidentiranja: 01.07.2026 09:00
	Hvala sto koristite nase usluge.
	BrzaPosta Tim
	---------------------------------------------------------------------------------
	*/
	if (brzaPosta.EvidentirajStatus(oznakaKnjige, PREUZETA, preuzeta))
		cout << "Status evidentiran i obavijest poslana" << crt;
	if (!brzaPosta.EvidentirajStatus(oznakaKnjige, ISPORUCENA, isporucena))
		cout << "Obavijest nije poslana za neispravnu promjenu statusa" << crt;
	if (!brzaPosta.EvidentirajStatus("NEPOSTOJECA", PREUZETA, preuzeta))
		cout << "Obavijest nije poslana za nepostojecu posiljku" << crt;

	/*
	PosiljkePoStatusu vraca vector pokazivaca na sve posiljke ciji trenutni
	status odgovara proslijedjenom statusu.
	*/
	vector<Posiljka*> preuzetePosiljke = brzaPosta.PosiljkePoStatusu(PREUZETA);
	for (auto posiljka : preuzetePosiljke)
		cout << posiljka->GetOznaka() << " -> " << StatusPosiljkeNazivi[(int)posiljka->GetTrenutniStatus()] << crt;

	/*
	StatistikaPoStatusu vraca kolekciju koja za svaki status, redoslijedom
	definisanim enumom StatusPosiljke, sadrzi status i broj posiljki koje se
	trenutno nalaze u tom statusu. U rezultat se dodaje svih pet statusa,
	ukljucujuci i one za koje je broj posiljki jednak nuli.
	*/
	Kolekcija<StatusPosiljke, int, 5> statistika = brzaPosta.StatistikaPoStatusu();
	for (int i = 0; i < statistika.GetTrenutno(); i++)
		cout << StatusPosiljkeNazivi[(int)statistika.GetElement1(i)] << " -> " << statistika.GetElement2(i) << crt;

	DostavnaSluzba kopijaSluzbe = brzaPosta;
	cout << kopijaSluzbe.GetNaziv() << " ima " << kopijaSluzbe.GetPosiljke().size() << " posiljki" << crt;

	cin.get();
	return 0;
}