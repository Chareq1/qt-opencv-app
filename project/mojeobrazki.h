#ifndef MOJEOBRAZKI_H
#define MOJEOBRAZKI_H

#include <QMainWindow>

//Biblioteki dołączone do projektu
#include <QFileDialog>          //Biblioteka do obsługi okna dialogowego do wyboru plików
#include <QImage>               //Biblioteka do obsługi obrazów
#include <QLabel>               //Biblioteka do obsługi pól tekstowych
#include <QTime>                //Biblioteka do obsługi czasu
#include <QMessageBox>          //Biblioteka do obsługi pola z wiadomością (okna dialogowego)
#include <QString>              //Biblioteka do obsługi łańcuchów znakowych
#include <QGraphicsPixmapItem>  //Biblioteka do obsługi pixmapy dla danej sceny
#include <QGraphicsScene>       //Biblioteka do obsługi scen graficznych
#include <QProgressBar>         //Biblioteka do obsługi paska postępu
#include <QPixmap>              //Biblioteka do obsługi mapy pikseli
#include <QDebug>               //Biblioteka do obsługi debugowania
#include <QTimer>               //Biblioteka do obsługi timerów
#include <opencv4/opencv2/core.hpp> //Biblioteka OpenCV

QT_BEGIN_NAMESPACE
namespace Ui { class MojeObrazki; }
QT_END_NAMESPACE

class MojeObrazki : public QMainWindow
{
    Q_OBJECT

public:
    MojeObrazki(QWidget *parent = nullptr);
    ~MojeObrazki();

    //ZMIENNE OGÓLNE
    cv::Mat obraz_in;   //Obraz wejściowy
    cv::Mat obraz_out;  //Obraz wyjściowy

    //ZMIENNE I METODY DLA JASNOŚCI I KONTRASTU
    int b_Red;          //Współczynnik korekcji jansości dla przestrzeni R
    int b_Green;        //Współczynnik korekcji jansości dla przestrzeni G
    int b_Blue;         //Współczynnik korekcji jansości dla przestrzeni B
    int kontrast;       //Zmienna pomocnicza do przechowywania wartości int dla kontrastu
    double alfa;        //Współczynnik korekcji kontrastu
    void on_jasnosc_kontrast_obrazu_RGB();  //Metoda do obsługi zmiany jasności i kontrastu

    //ZMIENNE I METODY DLA PROGROWANIA OBRAZU RGB I BINARYZACJI OBRAZU YCbCr
    int typ_p;                  //Zmienna wskazująca na typ progowania
    int prog_R;                 //Zmienna dla progu składowej R
    int prog_G;                 //Zmienna dla progu składowej G
    int prog_B;                 //Zmienna dla progu składowej B
    void Progowanie_RGB();      //Metoda do obsługi zmiany progowania obrazu RGB

    int typ_b;                  //Zmienna wskazująca na typ binaryzacji
    int bin_Y;                  //Zmienna dla progu składowej Y
    void Binaryzacja_YCrCb();   //Metoda do obsługi zmiany binaryzacji obrazu YCbCr

    //ZMIENNE I METODY DLA OBROTU I SKALOWANIA
    int kat_obrotu=0;           //Zmienna przechowująca kątu obrotu obrazu (0-360*)
    int wsp_skali=100;          //Zmienna przechowująca wartość skali obrazu
    void Skaluj();              //Metoda do obsługi skalowania obrazu RGB
    void Rotacja();             //Metoda do obsługi obrotu obrazu RGB

    //ZMIENNE I METODY DLA FILTRACJI DOLNOPRZEPUSTOWYCH
    int wielkosc_okna=1;        //Zmienna do przechowywania wielkości okna filtru
    void Mediana();             //Metoda do obsługi filtracji medianowej
    void Usrednianie();         //Metoda do obsługi filtracji uśredniającej
    void Gauss();               //Metoda do obsługi filtracji Gaussa

    //ZMEINE I METODY DLA FILTRACJI GÓRNOPRZEPUSTOWYCH I DETEKCJI KRAWĘDZI
    /*SOBEL*/
    cv::Mat grad_x;             //Zmienna do przechowywania obrazu wyjściowego w orientacji poziomej
    cv::Mat grad_y;             //Zmiennd do przechowywania obrazy wyjściowego w orientacji pionowej
    cv::Mat abs_grad_x;         //Zmienna do przechowywania obrazu wyjściowego w orientacji poziomej po nomralizacji wartości otrzymanej filtracji
    cv::Mat abs_grad_y;         //Zmienna do przechowywania obrazu wyjściowego w orientacji pionowej po nomralizacji wartości otrzymanej filtracji
    void Sobel();               //Metoda do obsługi filtracji Sobela

    /*PREWITT*/
    cv::Mat prewitt_x;           //Zmienna do przechowywania obrazu wyjściowego w orientacji poziomej
    cv::Mat prewitt_y;           //Zmienna do przechowywania obrazy wyjściowego w orientacji pionowej
    cv::Mat abs_prewitt_x;       //Zmienna do przechowywania obrazu wyjściowego w orientacji poziomej po nomralizacji wartości otrzymanej filtracji
    cv::Mat abs_prewitt_y;       //Zmienna do przechowywania obrazu wyjściowego w orientacji pionowej po nomralizacji wartości otrzymanej filtracji
    float matrix_x[3][3] = {{-1,0,1},{-1,0,1},{-1,0,1}};    //Macierz dla orientacji poziomej filtracji Prewitta
    float matrix_y[3][3] = {{1,1,1},{0,0,0},{-1,-1,-1}};    //Macierz dla orientacji pionowej filtracji Prewitta
    void Prewitt();             //Metoda do obsługi filtracji Prewitta

    /*CANNY*/
    int T_min;                  //Zmienna do przechowywania minimalnego progu T_min
    int T_max;                  //Zmienna do przechowywania maksymalnego progu T_max
    void Canny();               //Metoda do obsługi metody Canny'ego

    /*EROZJA*/
    int rozmiar_bazowy_e=0;     //Zmienna do przechowywania rozmiaru bazowego elementu dla erozji
    int typ_elementu_e=0;       //Zmienna do przechowywania rodzaju elementu strukturalnego dla erozji
    void Erozja();              //Metoda do obsługi erozji

    /*DYLATACJA*/
    int rozmiar_bazowy_d=0;     //Zmienna do przechowywania rozmiaru bazowego elementu dla dylatacji
    int typ_elementu_d=0;       //Zmienna do przechowywania rodzaju elementu strukturalnego dla dylatacji
    void Dylatacja();           //Metoda do obsługi dylatacji

    /*OTWARCIE*/
    int rozmiar_bazowy_o=0;     //Zmienna do przechowywania rozmiaru bazowego elementu dla otwarcia
    int typ_elementu_o=0;       //Zmienna do przechowywania rodzaju elementu strukturalnego dla otwarcia
    cv::Mat Erozja_o;           //Obiekt do przechowywania obrazu po erozji
    void Otwarcie();            //Metoda do obsługi otwarcia

    /*ZAMKNIĘCIE*/
    int rozmiar_bazowy_z=0;     //Zmienna do przechowywania rozmiaru bazowego elementu dla zamknięcia
    int typ_elementu_z=0;       //Zmienna do przechowywania rodzaju elementu strukturalnego dla zamknięcia
    cv::Mat Dylatacja_z;        //Obiekt do przechowywania obrazu po dylatacji
    void Zamkniecie();          //Metoda do obsługi zamknięcia

    /*GRADIENT MORFOLOGICZNY*/
    int rozmiar_bazowy_g=0;     //Zmienna do przechowywania rozmiaru bazowego elementu dla gradientu
    int typ_elementu_g=0;       //Zmienna do przechowywania rodzaju elementu strukturalnego dla gradientu
    cv::Mat Erozja_g;           //Obiekt do przechowywania obrazu po erozji
    cv::Mat Dylatacja_g;        //Obiekt do przechowywania obrazu po dylatacji
    void MorphGrad();           //Metoda do obsługi gradientu morfologicznego



    //FUNKCJE CALLBACK
    //Funkcja typu callback dla zmiany jasności i kontrastu
    static void JasKon_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->on_jasnosc_kontrast_obrazu_RGB();
    }

    //Funkcja typu callback dla progrowania obrazu RGB
    static void Prog_RGB_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Progowanie_RGB();
    }

    //Funkcja typu callback dla binaryzacji obrazu YCrCb
    static void Bin_YCrCb_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Binaryzacja_YCrCb();
    }

    //Funkcja typu callback dla obrotu obrazu RGB
    static void Rotacja_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Rotacja();
    }

    //Funkcja typu callback dla skalowania obrazu RGB
    static void Skaluj_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Skaluj();
    }

    //Funkcja typu callback dla filtracji medianowej
    static void Mediana_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Mediana();
    }

    //Funkcja typu callback dla filtracji uśredniającej
    static void Usrednianie_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Usrednianie();
    }

    //Funkcja typu callback dla filtracji Gaussa
    static void Gauss_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Gauss();
    }

    //Funkcja typu callback dla filtracji Canny'ego
    static void Canny_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Canny();
    }

    //Funkcja typu callback dla erozji
    static void Erozja_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Erozja();
    }

    //Funkcja typu callback dla dylatacji
    static void Dylatacja_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Dylatacja();
    }

    //Funkcja typu callback dla otwarcia
    static void Otwarcie_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Otwarcie();
    }

    //Funkcja typu callback dla zamknięcia
    static void Zamkniecie_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->Zamkniecie();
    }

    //Funkcja typu callback dla gradientu morfologicznego
    static void MorphGrad_callback(int v, void* p)
    {
        MojeObrazki *img = (MojeObrazki*) p;
        img->MorphGrad();
    }

private slots:
    void on_action_Koniec_triggered();

    void on_zegarTimeout();

    void on_action_Otw_rz_triggered();

    void on_action_Zapisz_triggered();

    void on_actionPokaz_obrazek_triggered();

    void on_actionPodzia_na_RGB_triggered();

    void on_actionPodzia_na_YCrCb_triggered();

    void on_actionPodzia_na_HSV_triggered();

    void on_actionJasno_i_kontrast_triggered();

    void on_actionProgowanie_obrazu_RGB_triggered();

    void on_actionBinaryzacja_obrazu_YCrCb_triggered();

    void on_actionObr_t_obrazu_RGB_triggered();

    void on_actionSkalowanie_obrazu_RGB_triggered();

    void on_actionFiltracja_medianowa_obrazu_RGB_triggered();

    void on_actionFiltracja_u_redniaj_ca_obrazu_RGB_triggered();

    void on_actionFiltracja_Gaussa_obrazu_RGB_triggered();

    void on_actionDetekcja_krawedzi_obrazu_RGB_metoda_Sobela_triggered();

    void on_actionDetekcja_krawedzi_obrazu_RGB_metoda_Prewitta_triggered();

    void on_actionDetekcja_kraw_dzi_obrazu_RGB_metod_Canny_ego_triggered();

    void on_actionErozja_triggered();

    void on_actionDylatacja_triggered();

    void on_actionOtwarcie_triggered();

    void on_actionZamkniecie_triggered();

    void on_actionGradient_morfologiczny_triggered();

private:
    Ui::MojeObrazki *ui;

    //Elementy pozwalające na dynamiczne pobieranie i przezentację obrazów w aplikacji
    QImage *obraz;
    QGraphicsScene *scena;
    QPixmap pixmapa;
    QGraphicsPixmapItem *obrazek_sceny;

    //Elementy prezentujące w statusBar aktualny czas, postęp wykonywania operacji oraz pole informacyjne
    QLabel info, czas_info;
    QProgressBar pasek_postepu;
    QTimer *zegar;
    QTime czas;

    //Element do przechowywania ścieżki
    QString wybranyObraz;
};
#endif // MOJEOBRAZKI_H
