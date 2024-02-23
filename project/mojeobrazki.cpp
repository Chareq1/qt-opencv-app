#include "mojeobrazki.h"
#include "ui_mojeobrazki.h"
#include <opencv4/opencv2/opencv.hpp>


MojeObrazki::MojeObrazki(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MojeObrazki)
{
    ui->setupUi(this);

    //Pole do wyświetlania informacji
    info.setMinimumWidth(500);
    info.setFrameStyle(QFrame::WinPanel);
    info.setText("Aplikacja na zajęcia z Przetwaraznia Obrazów");

    //Pasek postepu
    pasek_postepu.setMinimumWidth(150);
    pasek_postepu.setRange(0,100);
    pasek_postepu.setValue(pasek_postepu.minimum());

    //Pole do wyświetlania zegara
    czas_info.setMinimumWidth(150);
    czas_info.setFrameStyle(QFrame::Raised | QFrame::Box);
    czas_info.setAlignment(Qt::AlignCenter);
    czas_info.setText(czas.currentTime().toString("HH:mm:ss"));

    //Dodanie utworzonych elementów jako stałe widżety do statusBar
    ui->statusbar->addPermanentWidget(&czas_info, 1);
    ui->statusbar->addPermanentWidget(&pasek_postepu, 2);
    ui->statusbar->addPermanentWidget(&info, 3);

    //Aktywacja timera
    zegar = new QTimer(this);
    zegar->start(1000); //czas w milisekundach
    connect(zegar,SIGNAL(timeout()), this, SLOT(on_zegarTimeout()));
}

MojeObrazki::~MojeObrazki()
{
    //Dezaktywacja timera
    zegar->stop();
    delete zegar;

    delete ui;
}


void MojeObrazki::on_action_Koniec_triggered()
{
    QMessageBox MojeOkienkoWiadomosci(QMessageBox::Warning,"Wyjście z Programu","Czy chcesz wyjść z programu?",QMessageBox::Yes|QMessageBox::No);

    MojeOkienkoWiadomosci.setButtonText(QMessageBox::Yes,"Tak");
    MojeOkienkoWiadomosci.setButtonText(QMessageBox::No,"Nie");

    if (MojeOkienkoWiadomosci.exec()==QMessageBox::Yes)
    {
        QApplication::quit();
    }
}

void MojeObrazki::on_zegarTimeout()
{
    czas_info.setText(czas.currentTime().toString("HH:mm:ss"));
}

void MojeObrazki::on_action_Otw_rz_triggered()
{
    pasek_postepu.setValue(0);
    QString sciezka = QFileDialog::getOpenFileName(this,tr("Otwórz Plik"),"",tr("JPEG(*.jpg *.jpeg);;PNG (*.png)"));
    pasek_postepu.setValue(50);

    if (!sciezka.isEmpty())
    {
        wybranyObraz = sciezka;

        obraz = new QImage();
        obraz->load(sciezka);
        pixmapa = QPixmap::fromImage(*obraz);
        scena = new QGraphicsScene(this);
        obrazek_sceny = scena->addPixmap(pixmapa);

        scena->setSceneRect(pixmapa.rect());
        ui->graphicsView->setScene(scena);

        delete (obraz); // zabezpieczenie przed wyciekiem pamięci.

        info.setText("Wybrany Plik:"+sciezka+" rozmiar:"+QString::number(pixmapa.width())+"x"+QString::number(pixmapa.height()));
    }

    pasek_postepu.setValue(100);
}



void MojeObrazki::on_action_Zapisz_triggered()
{
    pasek_postepu.setValue(0);

    if (!pixmapa.isNull())
    {
        QString sciezka = QFileDialog::getSaveFileName(this,tr("Zapisz Plik"),"",tr("JPEG (*.jpg *.jpeg);;PNG (*.png)"));
        pasek_postepu.setValue(50);

        if (!sciezka.isEmpty())
        {
            QImage obrazek = obrazek_sceny->pixmap().toImage();
            if (obrazek.size().isValid()==true)
                obrazek.save(sciezka);
        }
    }
    else
    {
        QMessageBox(QMessageBox::Information,"Informacja","Nie załadowano pliku obrazu! Nie ma co zapisać!",QMessageBox::Ok).exec();
    }

    pasek_postepu.setValue(100);
}


void MojeObrazki::on_actionPokaz_obrazek_triggered()
{
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy = cv::imread(wybranyObraz.toStdString().c_str());
        cv::namedWindow("Przetwarzany Obraz",cv::WINDOW_NORMAL);
        cv::imshow("Przetwarzany Obraz",obraz_wejsciowy);
    }
}


void MojeObrazki::on_actionPodzia_na_RGB_triggered()
{
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy = cv::imread(wybranyObraz.toStdString().c_str());

        //Składowe B
        cv::Mat pokaz_blue;
        cv::Mat skladowa_blue[3];

        //Składowa G
        cv::Mat pokaz_green;
        cv::Mat skladowa_green[3];

        //Składowa R
        cv::Mat pokaz_red;
        cv::Mat skladowa_red[3];


        //Podział na poszczególne składowe
        //NIEBIESKI
        cv::split(obraz_wejsciowy, skladowa_blue);
        skladowa_blue[1] = cv::Mat::zeros(skladowa_blue[1].size(), CV_8UC1);
        skladowa_blue[2] = cv::Mat::zeros(skladowa_blue[2].size(), CV_8UC1);
        cv::merge(skladowa_blue,3,pokaz_blue);

        //ZIELONY
        cv::split(obraz_wejsciowy, skladowa_green);
        skladowa_green[0] = cv::Mat::zeros(skladowa_green[0].size(), CV_8UC1);
        skladowa_green[2] = cv::Mat::zeros(skladowa_green[2].size(), CV_8UC1);
        cv::merge(skladowa_green,3,pokaz_green);

        //CZERWONY
        cv::split(obraz_wejsciowy, skladowa_red);
        skladowa_red[0] = cv::Mat::zeros(skladowa_red[0].size(), CV_8UC1);
        skladowa_red[1] = cv::Mat::zeros(skladowa_red[1].size(), CV_8UC1);
        cv::merge(skladowa_red,3,pokaz_red);

        //Wyświetlenie okienek z poszczególnymi składowymi
        //NIEBIESKI
        cv::namedWindow("Skladowa B",cv::WINDOW_NORMAL);
        cv::imshow("Skladowa B",pokaz_blue);

        //ZIELONY
        cv::namedWindow("Skladowa G",cv::WINDOW_NORMAL);
        cv::imshow("Skladowa G",pokaz_green);

        //CZERWONY
        cv::namedWindow("Skladowa R",cv::WINDOW_NORMAL);
        cv::imshow("Skladowa R",pokaz_red);
    }
}


void MojeObrazki::on_actionPodzia_na_YCrCb_triggered()
{
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy = cv::imread(wybranyObraz.toStdString().c_str());

        //Zmienne dla konwersji z RGB do YCrCb
        cv::Mat obraz_YCrCb;    //Obraz po konwersji na YCrCb
        cv::Mat skladowe_YCrCb[3];  //Tablica na składowe

        cv::cvtColor(obraz_wejsciowy, obraz_YCrCb, cv::COLOR_BGR2YCrCb);    //Konwersja na YCrCb
        cv::split(obraz_YCrCb,skladowe_YCrCb);  //Podział na składowe

        //Wyświetlenie okienek z poszczególnymi składowymi
        //LUMINACJA
        cv::namedWindow("Skladowa Y",cv::WINDOW_NORMAL);
        cv::imshow("Skladowa Y",skladowe_YCrCb[0]);

        //CHROMINACJA KOLORU CZERWONEGO
        cv::namedWindow("Skladowa Cr",cv::WINDOW_NORMAL);
        cv::imshow("Skladowa Cr",skladowe_YCrCb[1]);

        //CHROMINACJA KOLORU NIEBIESKIEGO
        cv::namedWindow("Skladowa Cb",cv::WINDOW_NORMAL);
        cv::imshow("Skladowa Cb",skladowe_YCrCb[2]);
    }
}


void MojeObrazki::on_actionPodzia_na_HSV_triggered()
{
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy = cv::imread(wybranyObraz.toStdString().c_str());

        //Zmienne dla konwersji z RGB do HSV
        cv::Mat obraz_HSV;    //Obraz po konwersji na HSV
        cv::Mat skladowe_HSV[3];  //Tablica na składowe

        cv::cvtColor(obraz_wejsciowy, obraz_HSV, cv::COLOR_BGR2HSV);    //Konwersja na HSV
        cv::split(obraz_HSV,skladowe_HSV);  //Podział na składowe

        //Wyświetlenie okienek z poszczególnymi składowymi
        //CZĘSTOTLIWOŚĆ FALI ŚWIATŁA
        cv::namedWindow("Skladowa H",cv::WINDOW_NORMAL);
        cv::imshow("Skladowa H",skladowe_HSV[0]);

        //NASYCENIE
        cv::namedWindow("Skladowa S",cv::WINDOW_NORMAL);
        cv::imshow("Skladowa S",skladowe_HSV[1]);

        //POZIOM ŚWIATŁA BIAŁEGO
        cv::namedWindow("Skladowa V",cv::WINDOW_NORMAL);
        cv::imshow("Skladowa V",skladowe_HSV[2]);
    }
}

//Funkcja służąca do wyświetlenia okna biblioteki OpenCV z suwakami
void MojeObrazki::on_actionJasno_i_kontrast_triggered()
{
    if(!wybranyObraz.isEmpty()) {
        obraz_in = cv::imread(wybranyObraz.toStdString().c_str());

        cv::namedWindow("Jasnosc/Kontrast",cv::WINDOW_NORMAL);

        //Utworzenie suwaka dla współczynnika korekcji jasności dla przestrzeni R
        cv::createTrackbar("Jasnosc R", "Jasnosc/Kontrast", &b_Red, 100, &MojeObrazki::JasKon_callback,this);
        cv::setTrackbarMin("Jasnosc R", "Jasnosc/Kontrast", -100);
        cv::setTrackbarPos("Jasnosc R", "Jasnosc/Kontrast", 0);

        //Utworzenie suwaka dla współczynnika korekcji jasności dla przestrzeni G
        cv::createTrackbar("Jasnosc G", "Jasnosc/Kontrast", &b_Green, 100, &MojeObrazki::JasKon_callback,this);
        cv::setTrackbarMin("Jasnosc G", "Jasnosc/Kontrast", -100);
        cv::setTrackbarPos("Jasnosc G", "Jasnosc/Kontrast", 0);

        //Utworzenie suwaka dla współczynnika korekcji jasności dla przestrzeni B
        cv::createTrackbar("Jasnosc B", "Jasnosc/Kontrast", &b_Blue, 100, &MojeObrazki::JasKon_callback,this);
        cv::setTrackbarMin("Jasnosc B", "Jasnosc/Kontrast", -100);
        cv::setTrackbarPos("Jasnosc B", "Jasnosc/Kontrast",  0);

        //Utworzenie suwaka dla współczynnika korekcji kontrastu
        cv::createTrackbar("Kontrast", "Jasnosc/Kontrast", &kontrast, 299, &MojeObrazki::JasKon_callback,this);
        cv::setTrackbarMin("Kontrast", "Jasnosc/Kontrast", 50);
        cv::setTrackbarPos("Kontrast", "Jasnosc/Kontrast", 100);

        //Wywołanie funkcji dla zmiany jasności i kontrastu
        on_jasnosc_kontrast_obrazu_RGB();
    }
}

//Funkcja do zmiany jasności i kontrastu
void MojeObrazki::on_jasnosc_kontrast_obrazu_RGB()
{
    obraz_out=cv::Mat::zeros( obraz_in.size(), obraz_in.type() );   //Utworzenie dla obrazu wyjściowego macierzy o wymiarach i typie takim samym, jak oraz wejściowy
    alfa = (kontrast/100);    //Podzielenie otrzymanej wartości współczynnika korekcji przez 100

    //Pętla służąca do przejścia przez wszystkie piksele obrazu wejściowego
    for(int y=0; y<obraz_in.rows; y++) {
        for(int x=0; x<obraz_in.cols; x++)
        {
            //alfa - współczynnik korekcji kontrastu || beta - współczynnik korekcji jasności (dla danej barwy)
            //Zmiana przy użyciu wzoru wartości w przetrzeni B (w zakresie 0-255 -> saturate_cast) i dodanie do obrazka wynikowego
            obraz_out.at<cv::Vec3b>(y,x)[0] = cv::saturate_cast<uchar>(alfa*(obraz_in.at<cv::Vec3b>(y,x)[0] ) + b_Blue );

            //Zmiana przy użyciu wzoru wartości w przetrzeni G (w zakresie 0-255 -> saturate_cast) i dodanie do obrazka wynikowego
            obraz_out.at<cv::Vec3b>(y,x)[1] = cv::saturate_cast<uchar>(alfa*(obraz_in.at<cv::Vec3b>(y,x)[1] ) + b_Green );

            //Zmiana przy użyciu wzoru wartości w przetrzeni R (w zakresie 0-255 -> saturate_cast) i dodanie do obrazka wynikowego
            obraz_out.at<cv::Vec3b>(y,x)[2] = cv::saturate_cast<uchar>(alfa*(obraz_in.at<cv::Vec3b>(y,x)[2] ) + b_Red );
        }
    }

    //Wyświetlenie utworzonego obrazu
    cv::imshow("Jasnosc/Kontrast",obraz_out);
}


//PROGOWANIE OBRAZU RGB
//Funkcja do ustawiania progowania obrazu RGB
void MojeObrazki::Progowanie_RGB() {
    cv::Mat skladowe_obraz_in[3];   //Tablica pomocnicza do przechowywania składowych RGB obrazu wejściowego
    cv::Mat skladowe_obraz_out[3];  //Tablica pomocnicza do przechowywania składowych RGB obrazu wyjściowego

    cv::split(obraz_in, skladowe_obraz_in);     //Podzielenie obrazu wejściowego na składowe RGB

    //Ustawienie progowania dla poszczególnych składowych
    cv::threshold(skladowe_obraz_in[0], skladowe_obraz_out[0], prog_B, 255, typ_p);
    cv::threshold(skladowe_obraz_in[1], skladowe_obraz_out[1], prog_G, 255, typ_p);
    cv::threshold(skladowe_obraz_in[2], skladowe_obraz_out[2], prog_R, 255, typ_p);

    //Połączenie zmienionych składowych RGB do jednego obiektu (obrazu wyjściowego)
    cv::merge(skladowe_obraz_out, 3, obraz_out);

    //Wyświetlenie uzyskanego wyniku
    cv::imshow("Progowanie obrazu RGB", obraz_out);
}

void MojeObrazki::on_actionProgowanie_obrazu_RGB_triggered()
{
    char trackbar_type[] = "Rodzaj progowania: \n 0: Binary \n 1: Binary Inverted \n 2:Truncate \n 3: To Zero \n 4: To Zero Inverted\n";    //Zmienna przechowująca napis dla suwaka do zmiany typu progowania

    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        obraz_in=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR); //Przypisanie do obiektu obrazu wejściowego załadowanego obrazu do programu

        cv::namedWindow("Progowanie obrazu RGB",cv::WINDOW_AUTOSIZE);   //Utworzenie okna o podanej nazwie
         //Utworzenie suwaka do zmiany typu progowania
        cv::createTrackbar(trackbar_type, "Progowanie obrazu RGB", &typ_p, 4, &MojeObrazki::Prog_RGB_callback,this);

        //Utworzenie suwaków do zmiany wartości progu dla każdej ze składowych RGB
        cv::createTrackbar("Wartość progu R:","Progowanie obrazu RGB",&prog_R,255,&MojeObrazki::Prog_RGB_callback,this);
        cv::createTrackbar("Wartość progu G:","Progowanie obrazu RGB",&prog_G,255,&MojeObrazki::Prog_RGB_callback,this);
        cv::createTrackbar("Wartość progu B:","Progowanie obrazu RGB",&prog_B,255,&MojeObrazki::Prog_RGB_callback,this);

        //Wywołanie funkcji do progowania obrazu RGB
        Progowanie_RGB();
    }
}


//BINARYZACJA OBRAZU YCrCb
//Funckja służąca do binaryzacji obrazy YCrCb
void MojeObrazki::Binaryzacja_YCrCb() {
    cv::Mat skladowe_obraz_in[3];   //Utworzenie tablicy dla poszczególnych składowych YCrCb

    cv::split(obraz_in, skladowe_obraz_in); //Podzielenie obrazu wejściowego YCrCb na poszczególne składowe

    cv::threshold(skladowe_obraz_in[0], obraz_out, bin_Y, 255, typ_b); //Dokonanie operacji binaryzacji na składowej Y

    cv::imshow("Binaryzacja obrazu YCrCb", obraz_out); //Wyświetlenie uzyskanego rezultatu
}

void MojeObrazki::on_actionBinaryzacja_obrazu_YCrCb_triggered()
{
    char trackbar_type[] = "Rodzaj progowania: \n 0: Binary \n 1: Binary Inverted\n";   //Zmienna przechowująca napis dla suwaka do zmiany typu binaryzacji
    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);    //Przypisanie do obiektu pomocniczego obrazu załadowanego do programu
        cv::cvtColor(obraz_wejsciowy, obraz_in, cv::COLOR_BGR2YCrCb);   //Konwercja obrazu z RGB do YCrCb i przypisanie go do zmiennej globalnej dla obrazu wejściowego
        cv::namedWindow("Binaryzacja obrazu YCrCb",cv::WINDOW_AUTOSIZE);    //Utworzenie okna o podanej nazwie

        //Utworzenie suwaka do zmiany typu binaryzacji
        cv::createTrackbar(trackbar_type, "Binaryzacja obrazu YCrCb", &typ_b, 1, &MojeObrazki::Bin_YCrCb_callback,this);

        //Utworzenie suwaka do zmiany wartości progu dla składowej Y
        cv::createTrackbar("Wartość progu Y:","Binaryzacja obrazu YCrCb",&bin_Y,255,&MojeObrazki::Bin_YCrCb_callback,this);
        cv::setTrackbarPos("Wartość progu Y:", "Binaryzacja obrazu YCrCb", 0);

        //Wywołanie funkcji do binaryzacji obrazu YCrCb
        Binaryzacja_YCrCb();
    }
}


//ROTACJA OBRAZU RGB
//Funkcja służąca do obrotu obrazu RGB
void MojeObrazki::Rotacja()
{
    //ZMIENNE LOKALNE DO PRZECHOWYWANIA POTRZEBYNCH PARAMETRÓW
    double kat = kat_obrotu;                                                    //Zmienna do przechowywania kątu obrotu obrazu RGB
    cv::Size rozmiar;                                                           //Obiekt służący do przechowywania rozmiaru obrazu RGB
    rozmiar.width=obraz_in.cols;                                                //Przypisanie do obiektu rozmiar do pola width ilość kolumn w obrazie wejściowym
    rozmiar.height=obraz_in.rows;                                               //Przypisanie do obiektu rozmiar do pola height ilość wierzy w obrazie wejściowym
    cv::Point2f punkt_obrotu(rozmiar.width/2., rozmiar.height/2);               //Obliczenie współrzędncyh punktu w przestrzeni, względem którego następuje obrót obrazu
    cv::Mat macierz_obrotu = cv::getRotationMatrix2D(punkt_obrotu, kat, 1.0);   //Przypisanie do obiektu utworzonej macierzy obrotu dla obrazu względem wskazanego punktu i określeniem współczynnika kompensacji kształtu

    //Wykonanie finalnej postaci transformacji (przy użyciu macierzy obrotu) na obrazie wejściowym, przypisanie go do obrazu wyjściowego i wyświetlenie obrazu wyjściowego w oknie OpenCV
    cv::warpAffine(obraz_in, obraz_out, macierz_obrotu, rozmiar);
    cv::imshow("Rotacja", obraz_out);
}


void MojeObrazki::on_actionObr_t_obrazu_RGB_triggered()
{
    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu pomocniczego obrazu załadowanego do programu
        obraz_in = obraz_wejsciowy;                                                                     //Przypisanie do obiektu obrazu wejściowego obrazu z zmiennej pomocniczej

        cv::namedWindow("Rotacja", cv::WINDOW_AUTOSIZE);                                                //Utworzenie okna o podanej nazwie

        kat_obrotu=0;   //Ustawienie startowej wartości kąta obrotu na 0*

        //Utworzenie suwaka do wybrania kąta obrotu obrazu RGB
        cv::createTrackbar("Kąt obrotu", "Rotacja", &kat_obrotu, 360, &MojeObrazki::Rotacja_callback,this);

        //Wywołanie funkcji do obrotu obrazu RGB
        Rotacja();
    }
}


//SKALOWANIE OBRAZU RGB
//Funkcja służąca do skalowania obrazu RGB
void MojeObrazki::Skaluj()
{
    //ZMIENNE LOKALNE DO PRZECHOWYWANIA POTRZEBYNCH PARAMETRÓW
    double wspolczynnik=wsp_skali+1;    //Zmienna służaca do zmiany współczynnika skali (jest pewnym zabezpieczeniem przed wprowadzeniem wartości równej 0)
    wspolczynnik/=100;                  //Zmiana zakresu wspolczynnika z liczb całkowitych [0..200] na liczby zmiennoprzecinkowe [0.01 do 2.00]
    double Skala_x=wspolczynnik;        //Zmienna służąca do przechowywania współczynnika skalowania dla X
    double Skala_y=wspolczynnik;        //Zmienna służąca do przechowywania współczynnika skalowania dla Y
    cv::Size rozmiar;                   //Obiekt przechowujący informacje na temat rozmiaru obrazu wejściowego

    //Wywołanie funkcji służącej do skalowania z metodą INTER_LINEAR dla obrazu wejściowego, przypisanie do obrazu wyjściowego i wyświetlenie go w oknie biblioteki OpenCv
    cv::resize(obraz_in, obraz_out, rozmiar, Skala_x, Skala_y, cv::INTER_LINEAR);
    cv::imshow("Skalowanie", obraz_out);
}


void MojeObrazki::on_actionSkalowanie_obrazu_RGB_triggered()
{
    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu pomocniczego obrazu załadowanego do programu
        obraz_in = obraz_wejsciowy;                                                                     //Przypisanie do obiektu obrazu wejściowego obrazu z zmiennej pomocniczej

        cv::namedWindow("Skalowanie");                                             //Utworzenie okna o podanej nazwie
        cv::resizeWindow("Skalowanie", obraz_in.cols*2, obraz_in.rows*2);          //Zmiana rozmiaru okna dwukrotnie

        wsp_skali=100;   //Ustawienie startowej wartości kąta obrotu na 100%

        //Utworzenie suwaka do wybrania współczynnika skali obrazu RGB
        cv::createTrackbar("Współczynnik skali", "Skalowanie", &wsp_skali, 200, &MojeObrazki::Skaluj_callback,this);

        //Wywołanie funkcji do skalowania obrazu RGB
        Skaluj();
    }
}


//FILTRACJA MEDIANOWA
void MojeObrazki::Mediana() {
    int wymiar_okna=(2*wielkosc_okna)+1;    //Zmienna wymiaru okna na podstawie wybranej wielkości z suwaka

    cv::medianBlur(obraz_in, obraz_out, wymiar_okna);   //Wywołanie funkcji filtracji medianowej dla danego obrazu wejściowego i dla danej wielkości okna, oraz przypisanie uzyskanego wyniku do obrazu wyjściowego
    cv::imshow("Filtr medianowy", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po filtracji)
}

void MojeObrazki::on_actionFiltracja_medianowa_obrazu_RGB_triggered()
{
    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu pomocniczego obrazu załadowanego do programu
        obraz_in = obraz_wejsciowy;                                                                     //Przypisanie do obiektu obrazu wejściowego obrazu z zmiennej pomocniczej

        cv::namedWindow("Filtr medianowy", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Utworzenie suwaka do wybrania wielkości okna i ustawienie minimalnej pozycji na 1
        cv::createTrackbar("Wielkość okna", "Filtr medianowy", &wielkosc_okna, 11, &MojeObrazki::Mediana_callback, this);
        cv::setTrackbarMin("Wielkość okna", "Filtr medianowy", 1);

        //Wywołanie funkcji do filtracji medianowej obrazu RGB
        Mediana();
    }
}


//FILTRACJA UŚREDNIAJĄCA
void MojeObrazki::Usrednianie() {
    int wymiar_okna=(2*wielkosc_okna)+1;    //Zmienna wymiaru okna na podstawie wybranej wielkości z suwaka

    cv::blur(obraz_in, obraz_out, cv::Size(wymiar_okna, wymiar_okna));   //Wywołanie funkcji filtracji uśredniającej dla danego obrazu wejściowego i dla danej wielkości okna, oraz przypisanie uzyskanego wyniku do obrazu wyjściowego
    cv::imshow("Filtr usredniajacy", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po filtracji)
}

void MojeObrazki::on_actionFiltracja_u_redniaj_ca_obrazu_RGB_triggered()
{
    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu pomocniczego obrazu załadowanego do programu
        obraz_in = obraz_wejsciowy;                                                                     //Przypisanie do obiektu obrazu wejściowego obrazu z zmiennej pomocniczej

        cv::namedWindow("Filtr usredniajacy", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Utworzenie suwaka do wybrania wielkości okna i ustawienie minimalnej pozycji na 1
        cv::createTrackbar("Wielkość okna", "Filtr usredniajacy", &wielkosc_okna, 11, &MojeObrazki::Usrednianie_callback, this);
        cv::setTrackbarMin("Wielkość okna", "Filtr usredniajacy", 1);

        //Wywołanie funkcji do filtracji uśredniajacej obrazu RGB
        Usrednianie();
    }
}


//FILTRACJA GAUSSA
void MojeObrazki::Gauss() {
    int wymiar_okna=(2*wielkosc_okna)+1;    //Zmienna wymiaru okna na podstawie wybranej wielkości z suwaka

    cv::GaussianBlur(obraz_in, obraz_out, cv::Size(wymiar_okna, wymiar_okna), 0, 0);   //Wywołanie funkcji filtracji Gaussa dla danego obrazu wejściowego, odchyleń standardowych równych 0 i dla danej wielkości okna, oraz przypisanie uzyskanego wyniku do obrazu wyjściowego
    cv::imshow("Filtr Gaussa", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po filtracji)
}


void MojeObrazki::on_actionFiltracja_Gaussa_obrazu_RGB_triggered()
{
    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu pomocniczego obrazu załadowanego do programu
        obraz_in = obraz_wejsciowy;                                                                     //Przypisanie do obiektu obrazu wejściowego obrazu z zmiennej pomocniczej

        cv::namedWindow("Filtr Gaussa", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Utworzenie suwaka do wybrania wielkości okna i ustawienie minimalnej pozycji na 1
        cv::createTrackbar("Wielkość okna", "Filtr Gaussa", &wielkosc_okna, 11, &MojeObrazki::Gauss_callback, this);
        cv::setTrackbarMin("Wielkość okna", "Filtr Gaussa", 1);

        //Wywołanie funkcji do filtracji Gaussa obrazu RGB
        Gauss();
    }
}


//FILTRACJA SOBELA
void MojeObrazki::Sobel() {
    cv::Mat skladowe_obraz_Sobel[3];   //Utworzenie tablicy dla poszczególnych składowych YCrCb
    cv::split(obraz_in, skladowe_obraz_Sobel); //Podzielenie obrazu wejściowego YCrCb na poszczególne składowe

    cv::Sobel(skladowe_obraz_Sobel[0], grad_x, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);  //Filtracja Sobela dla składowej Y obrazu wejściowego w orientacji poziomej
    cv::convertScaleAbs(grad_x, abs_grad_x);    //Normalizacja obrazu wejściowego dla składowej Y obrazu wejściowego w orientacji poziomej
    cv::Sobel(skladowe_obraz_Sobel[0], grad_y, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);  //Filtracja Sobela dla składowej Y obrazu wejściowego w orientacji pionowej
    cv::convertScaleAbs(grad_y, abs_grad_y);    //Normalizacja obrazu wejściowego dla składowej Y obrazu wejściowego w orientacji pionowej
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, obraz_out);    //Obliczenie ważonej postaci końcowej filtracji Sobela

    cv::imshow("Detekcja krawedzi - metoda Sobela", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po filtracji)
}

void MojeObrazki::on_actionDetekcja_krawedzi_obrazu_RGB_metoda_Sobela_triggered()
{
    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu pomocniczego obrazu załadowanego do programu
        cv::cvtColor(obraz_wejsciowy, obraz_in, cv::COLOR_BGR2YCrCb);   //Konwercja obrazu z RGB do YCrCb i przypisanie go do zmiennej globalnej dla obrazu wejściowego

        cv::namedWindow("Detekcja krawedzi - metoda Sobela", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Wywołanie funkcji do filtracji obrazu RGB metodą Sobela
        Sobel();
    }
}

//FILTRACJA PREWITTA
void MojeObrazki::Prewitt() {
    cv::Mat skladowe_obraz_Prewitt[3];   //Utworzenie tablicy dla poszczególnych składowych YCrCb
    cv::split(obraz_in, skladowe_obraz_Prewitt); //Podzielenie obrazu wejściowego YCrCb na poszczególne składowe

    cv::Mat kernel_x = cv::Mat(3, 3, CV_32F, matrix_x);
    cv::Mat kernel_y = cv::Mat(3, 3, CV_32F, matrix_y);

    cv::filter2D(skladowe_obraz_Prewitt[0], prewitt_x, CV_16S, kernel_x, cv::Point(-1,-1), 0, cv::BORDER_DEFAULT);  //Filtracja Prewitta dla składowej Y obrazu wejściowego w orientacji poziomej
    cv::convertScaleAbs(prewitt_x, abs_prewitt_x);  //Normalizacja obrazu wejściowego dla składowej Y obrazu wejściowego w orientacji poziomej
    cv::filter2D(skladowe_obraz_Prewitt[0], prewitt_y, CV_16S, kernel_y, cv::Point(-1,-1), 0, cv::BORDER_DEFAULT);  //Filtracja Sobela dla składowej Y obrazu wejściowego w orientacji pionowej
    cv::convertScaleAbs(prewitt_y, abs_prewitt_y);  //Normalizacja obrazu wejściowego dla składowej Y obrazu wejściowego w orientacji pionowej
    cv::addWeighted(abs_prewitt_x, 0.5, abs_prewitt_y, 0.5, 0, obraz_out);  //Obliczenie ważonej postaci końcowej filtracji Prewitta

    cv::imshow("Detekcja krawedzi - metoda Prewitta", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po filtracji)
}

void MojeObrazki::on_actionDetekcja_krawedzi_obrazu_RGB_metoda_Prewitta_triggered()
{
    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu pomocniczego obrazu załadowanego do programu
        cv::cvtColor(obraz_wejsciowy, obraz_in, cv::COLOR_BGR2YCrCb);   //Konwercja obrazu z RGB do YCrCb i przypisanie go do zmiennej globalnej dla obrazu wejściowego

        cv::namedWindow("Detekcja krawedzi - metoda Prewitta", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Wywołanie funkcji do filtracji Prewitta obrazu RGB
        Prewitt();
    }
}


//FILTRACJA CANNYEGO
void MojeObrazki::Canny() {
    cv::Mat skladowe_obraz_Canny[3];   //Utworzenie tablicy dla poszczególnych składowych YCrCb
    cv::split(obraz_in, skladowe_obraz_Canny); //Podzielenie obrazu wejściowego YCrCb na poszczególne składowe

    T_max = T_min*3;    //Obliczenie wartości maksymalnego progu T_max

    cv::Canny(skladowe_obraz_Canny[0], obraz_out, T_min, T_max, 3); //Filtracja Canny;ego

    cv::imshow("Detekcja krawedzi - metoda Cannyego", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po filtracji)
}

void MojeObrazki::on_actionDetekcja_kraw_dzi_obrazu_RGB_metod_Canny_ego_triggered()
{
    //Sprawdzenie, czy do programu załadoway jest jakiś obraz
    if(!wybranyObraz.isEmpty())
    {
        cv::Mat obraz_wejsciowy=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu pomocniczego obrazu załadowanego do programu
        cv::cvtColor(obraz_wejsciowy, obraz_in, cv::COLOR_BGR2YCrCb);   //Konwercja obrazu z RGB do YCrCb i przypisanie go do zmiennej globalnej dla obrazu wejściowego

        cv::namedWindow("Detekcja krawedzi - metoda Cannyego", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Utworzenie suwaka do wybrania wielkości T_min i ustawienie minimalnej pozycji na 1
        cv::createTrackbar("Próg T_min", "Detekcja krawedzi - metoda Cannyego", &T_min, 100, &MojeObrazki::Canny_callback, this);
        cv::setTrackbarMin("Próg T_min", "Detekcja krawedzi - metoda Cannyego", 1);

        //Wywołanie funkcji do filtracji Canny'ego obrazu RGB
        Canny();
    }
}


//EROZJA
void MojeObrazki::Erozja() {
    cv::Mat kernel; //Utworzenie obiektu dla kernela

    //Sprawdzenie, jaki rodzaj elementu struktulralnego został wybrany i przypisanie go do obiektu kernel
    if(typ_elementu_e==0) {
        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size (2*rozmiar_bazowy_e + 1, 2*rozmiar_bazowy_e+1), cv::Point(rozmiar_bazowy_e, rozmiar_bazowy_e));
    }
    else if(typ_elementu_e==1) {
        kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size (2*rozmiar_bazowy_e + 1, 2*rozmiar_bazowy_e+1), cv::Point(rozmiar_bazowy_e, rozmiar_bazowy_e));
    }
    if(typ_elementu_e==2) {
        kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size (2*rozmiar_bazowy_e + 1, 2*rozmiar_bazowy_e+1), cv::Point(rozmiar_bazowy_e, rozmiar_bazowy_e));
    }

    //Funkcja do wykonania erozji dla obrazu wejściowego
    cv::erode(obraz_in, obraz_out, kernel, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT);

    cv::imshow("Erozja", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po erozji)
}

void MojeObrazki::on_actionErozja_triggered()
{
    //Zmienna do przechowywania nazwy suwaka dla rodzaju elementu strukturalnego
    char trackbar_type[] = "Rodzaj elementu strukturalnego: \n 0: Kwadratowy \n 1: Krzyżowy \n 2: Eliptyczny \n";

    //Sprawdzenie, czy do programu został załadowany jakikolwiek obraz
    if(!wybranyObraz.isEmpty())
    {
        obraz_in=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu obrazu wejściowego obrazu załadowanego do programu

        cv::namedWindow("Erozja", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Utworzenie suwaka do wybrania rodzaju elementu strukturalnego
        cv::createTrackbar(trackbar_type, "Erozja", &typ_elementu_e, 2, &MojeObrazki::Erozja_callback, this);

        //Utworzenie suwaka do wybrania wielkości bazowej elementu
        cv::createTrackbar("Wielkość bazowa N elementu", "Erozja", &rozmiar_bazowy_e, 11, &MojeObrazki::Erozja_callback, this);

        //Wywołanie funkcji do erozji obrazu RGB
        Erozja();
    }
}


//DYLATACJA
void MojeObrazki::Dylatacja() {
    cv::Mat kernel; //Utworzenie obiektu dla kernela

    //Sprawdzenie, jaki rodzaj elementu struktulralnego został wybrany i przypisanie go do obiektu kernel
    if(typ_elementu_d==0) {
        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size (2*rozmiar_bazowy_d + 1, 2*rozmiar_bazowy_d+1), cv::Point(rozmiar_bazowy_d, rozmiar_bazowy_d));
    }
    else if(typ_elementu_d==1) {
        kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size (2*rozmiar_bazowy_d + 1, 2*rozmiar_bazowy_d+1), cv::Point(rozmiar_bazowy_d, rozmiar_bazowy_d));
    }
    if(typ_elementu_d==2) {
         kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size (2*rozmiar_bazowy_d + 1, 2*rozmiar_bazowy_d+1), cv::Point(rozmiar_bazowy_d, rozmiar_bazowy_d));
    }

    //Funkcja do wykonania dylatacji dla obrazu wejściowego
    cv::dilate(obraz_in, obraz_out, kernel, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT);

    cv::imshow("Dylatacja", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po dylatacji)
}

void MojeObrazki::on_actionDylatacja_triggered()
{
    //Zmienna do przechowywania nazwy suwaka dla rodzaju elementu strukturalnego
    char trackbar_type[] = "Rodzaj elementu strukturalnego: \n 0: Kwadratowy \n 1: Krzyżowy \n 2: Eliptyczny \n";

    //Sprawdzenie, czy do programu został załadowany jakikolwiek obraz
    if(!wybranyObraz.isEmpty())
    {
        obraz_in=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu obrazu wejściowego obrazu załadowanego do programu

        cv::namedWindow("Dylatacja", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Utworzenie suwaka do wybrania rodzaju elementu strukturalnego
        cv::createTrackbar(trackbar_type, "Dylatacja", &typ_elementu_d, 2, &MojeObrazki::Dylatacja_callback, this);

        //Utworzenie suwaka do wybrania wielkościbazowej elementu
        cv::createTrackbar("Wielkość bazowa N elementu", "Dylatacja", &rozmiar_bazowy_d, 11, &MojeObrazki::Dylatacja_callback, this);

        //Wywołanie funkcji do dylatacji obrazu RGB
        Dylatacja();
    }
}


//OTWARCIE
void MojeObrazki::Otwarcie() {
    cv::Mat kernel;     //Utworzenie obiektu dla kernela

    //Sprawdzenie, jaki rodzaj elementu struktulralnego został wybrany i przypisanie go do obiektu kernel
    if(typ_elementu_o==0) {
        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size (2*rozmiar_bazowy_o + 1, 2*rozmiar_bazowy_o+1), cv::Point(rozmiar_bazowy_o, rozmiar_bazowy_o));
    }
    else if(typ_elementu_o==1) {
        kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size (2*rozmiar_bazowy_o + 1, 2*rozmiar_bazowy_o+1), cv::Point(rozmiar_bazowy_o, rozmiar_bazowy_o));
    }
    if(typ_elementu_o==2) {
        kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size (2*rozmiar_bazowy_o + 1, 2*rozmiar_bazowy_o+1), cv::Point(rozmiar_bazowy_o, rozmiar_bazowy_o));
    }

    //Dokonanie erozji na obrazie wejściowym
    cv::erode(obraz_in, Erozja_o, kernel, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT);

    //Dokonanie dylatacji na obrazie po erozji
    cv::dilate(Erozja_o, obraz_out, kernel, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT);

    cv::imshow("Otwarcie", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po otwarciu)
}

void MojeObrazki::on_actionOtwarcie_triggered()
{
    //Zmienna do przechowywania nazwy suwaka dla rodzaju elementu strukturalnego
    char trackbar_type[] = "Rodzaj elementu strukturalnego: \n 0: Kwadratowy \n 1: Krzyżowy \n 2: Eliptyczny \n";

    //Sprawdzenie, czy do programu został załadowany jakikolwiek obraz
    if(!wybranyObraz.isEmpty())
    {
        obraz_in=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu obrazu wejściowego obrazu załadowanego do programu

        cv::namedWindow("Otwarcie", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Utworzenie suwaka do wybrania rodzaju elementu struktrualnego
        cv::createTrackbar(trackbar_type, "Otwarcie", &typ_elementu_o, 2, &MojeObrazki::Otwarcie_callback, this);

        //Utworzenie suwaka do wybrania wielkości bazowej elementu
        cv::createTrackbar("Wielkość bazowa N elementu", "Otwarcie", &rozmiar_bazowy_o, 11, &MojeObrazki::Otwarcie_callback, this);

        //Wywołanie funkcji do otwarcia obrazu RGB
        Otwarcie();
    }
}


//ZAMKNIECIE
void MojeObrazki::Zamkniecie() {
    cv::Mat kernel;     //Utworzenie obiektu dla kernela

    //Sprawdzenie, jaki rodzaj elementu struktulralnego został wybrany i przypisanie go do obiektu kernel
    if(typ_elementu_z==0) {
        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size (2*rozmiar_bazowy_z + 1, 2*rozmiar_bazowy_z+1), cv::Point(rozmiar_bazowy_z, rozmiar_bazowy_z));
    }
    else if(typ_elementu_z==1) {
        kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size (2*rozmiar_bazowy_z + 1, 2*rozmiar_bazowy_z+1), cv::Point(rozmiar_bazowy_z, rozmiar_bazowy_z));
    }
    if(typ_elementu_z==2) {
        kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size (2*rozmiar_bazowy_z + 1, 2*rozmiar_bazowy_z+1), cv::Point(rozmiar_bazowy_z, rozmiar_bazowy_z));
    }

    //Dokonanie dylatacji na obrazie wejściowym
    cv::dilate(obraz_in, Dylatacja_z, kernel, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT);

    //Dokonanie erozji na obrazie po dylatacji
    cv::erode(Dylatacja_z, obraz_out, kernel, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT);

    cv::imshow("Zamkniecie", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po zamknięciu)
}

void MojeObrazki::on_actionZamkniecie_triggered()
{
    //Zmienna do przechowywania nazwy suwaka dla rodzaju elementu strukturalnego
    char trackbar_type[] = "Rodzaj elementu strukturalnego: \n 0: Kwadratowy \n 1: Krzyżowy \n 2: Eliptyczny \n";

    //Sprawdzenie, czy do programu został załadowany jakikolwiek obraz
    if(!wybranyObraz.isEmpty())
    {
        obraz_in=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu obrazu wejściowego obrazu załadowanego do programu

        cv::namedWindow("Zamkniecie", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Utworzenie suwaka do wybrania rodzaju elementu strukturalnego
        cv::createTrackbar(trackbar_type, "Zamkniecie", &typ_elementu_z, 2, &MojeObrazki::Zamkniecie_callback, this);

        //Utworzenie suwaka do wybrania wielkości bazowej elementu
        cv::createTrackbar("Wielkość bazowa N elementu", "Zamkniecie", &rozmiar_bazowy_z, 11, &MojeObrazki::Zamkniecie_callback, this);

        //Wywołanie funkcji do zamknięcia obrazu RGB
        Zamkniecie();
    }
}


//GRADIENT MORFOLOGICZNY
void MojeObrazki::MorphGrad() {
    cv::Mat kernel;     //Utworzenie obiektu dla kernela

    //Sprawdzenie, jaki rodzaj elementu struktulralnego został wybrany i przypisanie go do obiektu kernel
    if(typ_elementu_g==0) {
        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size (2*rozmiar_bazowy_g + 1, 2*rozmiar_bazowy_g+1), cv::Point(rozmiar_bazowy_g, rozmiar_bazowy_g));
    }
    else if(typ_elementu_g==1) {
        kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size (2*rozmiar_bazowy_g + 1, 2*rozmiar_bazowy_g+1), cv::Point(rozmiar_bazowy_g, rozmiar_bazowy_g));
    }
    if(typ_elementu_g==2) {
        kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size (2*rozmiar_bazowy_g + 1, 2*rozmiar_bazowy_g+1), cv::Point(rozmiar_bazowy_g, rozmiar_bazowy_g));
    }

    //Dokonanie dylatacji i erozji na obrazie wejściowym
    cv::dilate(obraz_in, Dylatacja_g, kernel, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT);
    cv::erode(obraz_in, Erozja_g, kernel, cv::Point(-1,-1), 1, cv::BORDER_CONSTANT);

    //Odjęcie obrazu erozji z obrazu dylatacji
    obraz_out=Dylatacja_g-Erozja_g;

    cv::imshow("Gradient morfologiczny", obraz_out);   //Wyświetlenie obrazu wyjściowego (czyli po gradiencie)
}


void MojeObrazki::on_actionGradient_morfologiczny_triggered()
{
    //Zmienna do przechowywania nazwy suwaka dla rodzaju elementu strukturalnego
    char trackbar_type[] = "Rodzaj elementu strukturalnego: \n 0: Kwadratowy \n 1: Krzyżowy \n 2: Eliptyczny \n";

    //Sprawdzenie, czy do programu został załadowany jakikolwiek obraz
    if(!wybranyObraz.isEmpty())
    {
        obraz_in=cv::imread(wybranyObraz.toStdString().c_str(),cv::IMREAD_COLOR);        //Przypisanie do obiektu obrazu wejściowego obrazu załadowanego do programu

        cv::namedWindow("Gradient morfologiczny", cv::WINDOW_AUTOSIZE);    //Utworzenie nowego okna z rozmiarem typu AUTOSIZE

        //Utworzenie suwaka do wybrania rodzaju elementu strukturalnego
        cv::createTrackbar(trackbar_type, "Gradient morfologiczny", &typ_elementu_g, 2, &MojeObrazki::MorphGrad_callback, this);

        //Utworzenie suwaka do wybrania wielkości bazowej elementu
        cv::createTrackbar("Wielkość bazowa N elementu", "Gradient morfologiczny", &rozmiar_bazowy_g, 11, &MojeObrazki::MorphGrad_callback, this);

        //Wywołanie funkcji do gradientu morfologicznego obrazu RGB
        MorphGrad();
    }
}

