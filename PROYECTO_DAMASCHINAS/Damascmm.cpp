#include <iostream>//entrada y salida de datos 
#include <cmath>//opc matematicas
#include <cctype>//manipula y clasifica caracteres 
#include <thread>//gestiona procesos de ejecucion
#include <chrono>//gestiona el tiempo
#include <atomic>//sincroniza y ordena el acceso ala memoria 
#include <fstream>//lee y escribe 
#include <conio.h>//entrada y salida de consola
#include <windows.h> // Para activar colores y emojis en Windows
#include <sstream>//opera sobre cadenas
#include <vector>

using namespace std;
using namespace std::chrono;

int tablero[8][8];

int tiempoBlancas = 300; // 5 minutos
int tiempoNegras = 300;
int turno = 1;//controla los turnos 

// Control del temporizador
atomic<bool> temporizadorActivo(false);
atomic<bool> tiempoAgotado(false);
atomic<bool> partidaTerminada(false);

string formatoTiempo(int segundos) {
    int minutos = segundos / 60;
    int segs = segundos % 60;
    return to_string(minutos) + ":" + (segs < 10? "0" : "") + to_string(segs);
}

void MostrarRelojes() {
    cout << "---- RELOJ ----" << endl;
    cout << "   BLANCAS:" << formatoTiempo(tiempoBlancas) << endl;
    cout << "   NEGRAS:" << formatoTiempo(tiempoNegras) << endl;
    cout << "---------------" << endl;
}

void InicializarTablero(){
    for(int f=0; f<8; f++){
        for(int c=0; c<8; c++){
            tablero[f][c] = 0;
        }
    }
    for(int f=0; f<3; f++){ // fichas negras
        for(int c=0; c<8; c++){
            if((f+c)%2==1) tablero[f][c]=2;
        }
    }

    for(int f=5; f<8; f++){
        for(int c=0; c<8; c++){
            if((f+c)%2==1){ tablero[f][c]=1; }
        }
    }
}

// Función auxiliar para verificar si un archivo ya existe
bool ExisteArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    return archivo.is_open();
}

void GuardarPartida() {
    string nombrePartida;
    cin.ignore(10000, '\n'); // Limpiar búfer
    
    cout << "\nIngrese un nombre para su partida guardada (sin espacios preferiblemente): ";
    getline(cin, nombrePartida);

    string nombreArchivoBin = "partida_" + nombrePartida + ".dat";
    string nombreArchivoTxt = "partida_" + nombrePartida + ".txt";

    // VALIDACIÓN: Evitar que se guarde con un nombre existente
    if (ExisteArchivo(nombreArchivoBin)) {
        cout << "\n[ERROR] ¡Ya existe una partida guardada con ese nombre! Elige otro diferente para evitar sobrescribir.\n";
        system("pause");
        return;
    }

    // Guardar datos binarios de la partida
    ofstream archivo(nombreArchivoBin, ios::binary);
    if (archivo.is_open()) {
        archivo.write(reinterpret_cast<char*>(&turno), sizeof(turno));
        archivo.write(reinterpret_cast<char*>(&tiempoBlancas), sizeof(tiempoBlancas));
        archivo.write(reinterpret_cast<char*>(&tiempoNegras), sizeof(tiempoNegras));
        archivo.write(reinterpret_cast<char*>(tablero), sizeof(tablero));
        archivo.close();

        // Registrar también en un archivo maestro de partidas guardadas para listarlas fácilmente
        ofstream indice("partidas_registradas.txt", ios::app);
        if (indice.is_open()) {
            indice << nombrePartida << endl;
            indice.close();
        }

        cout << "\n¡Partida '" << nombrePartida << "' guardada exitosamente!\n";
    } else {
        cout << "\nError al intentar guardar la partida.\n";
    }
    system("pause");
}

bool CargarPartida() {
    vector<string> partidasDisponibles;
    ifstream indice("partidas_registradas.txt");
    
    if (indice.is_open()) {
        string linea;
        while (getline(indice, linea)) {
            if (!linea.empty()) {
                partidasDisponibles.push_back(linea);
            }
        }
        indice.close();
    }

    if (partidasDisponibles.empty()) {
        cout << "\nNo se encontraron partidas guardadas registradas.\n";
        system("pause");
        return false;
    }

    system("cls");
    cout << "====================================================" << endl;
    cout << "           SELECCIONA UNA PARTIDA A CARGAR          " << endl;
    cout << "====================================================" << endl;
    for (size_t i = 0; i < partidasDisponibles.size(); ++i) {
        cout << i + 1 << ". " << partidasDisponibles[i] << endl;
    }
    cout << "0. Volver al menú principal" << endl;
    cout << "Elige una opción: ";

    int seleccion;
    cin >> seleccion;

    if (cin.fail() || seleccion == 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        return false;
    }

    if (seleccion < 1 || seleccion > (int)partidasDisponibles.size()) {
        cout << "\nOpción inválida.\n";
        system("pause");
        return false;
    }

    string nombreElegido = partidasDisponibles[seleccion - 1];
    string nombreArchivoBin = "partida_" + nombreElegido + ".dat";

    ifstream archivo(nombreArchivoBin, ios::binary);
    if (archivo.is_open()) {
        archivo.read(reinterpret_cast<char*>(&turno), sizeof(turno));
        archivo.read(reinterpret_cast<char*>(&tiempoBlancas), sizeof(tiempoBlancas));
        archivo.read(reinterpret_cast<char*>(&tiempoNegras), sizeof(tiempoNegras));
        archivo.read(reinterpret_cast<char*>(tablero), sizeof(tablero));
        archivo.close();
        cout << "\n¡Partida '" << nombreElegido << "' cargada con éxito!\n";
        system("pause");
        return true;
    } else {
        cout << "\nNo se pudo abrir el archivo de la partida seleccionada.\n";
        system("pause");
        return false;
    }
}

void MostrarTablero(){
    cout << "    A   B   C   D   E   F   G   H" << endl;
    
    for(int f=0; f<8; f++){
        cout << f << " ";
        
        for(int c=0; c<8; c++){
            bool casillaOscura = (f + c) % 2 == 1;
            
            string colorFondo = casillaOscura ? "\033[48;5;94m" : "\033[48;5;223m";
            cout << colorFondo; 

            if(tablero[f][c] == 0) {
                cout << "    "; 
            }
            else {
                string fichaStr = "";
                if(tablero[f][c] == 1) fichaStr = "\033[38;5;15m⚪\033[0m"; // blanca
                else if(tablero[f][c] == 2) fichaStr = "\033[38;5;0m⚫\033[0m";  // negra
                else if(tablero[f][c] == 3) fichaStr = "\033[38;5;21m♔\033[0m"; // dama blanca
                else if(tablero[f][c] == 4) fichaStr = "\033[38;5;5m♚\033[0m";  // dama negra

                cout << " " << fichaStr << colorFondo << " "; 
            }
            cout << "\033[0m";
        }
        cout << endl;
    }
}

void GuardamovimientoenArchivo(int turno, int fo, int co, int fd, int cd) {
    ofstream archivo("partida_en_curso.txt", ios::app);
    if (archivo.is_open()) {
        char co_letra = 'A' + co; 
        char cd_letra = 'A' + cd;
        archivo << "Turno de " << (turno == 1? "BLANCAS" : "NEGRAS") << ": "
                << "Mover de (" << fo << ", " << co_letra << ")\n"
                << "a destino (" << fd << ", " << cd_letra << ")\n";
        archivo.close();
    }
}

bool PuedeComer(int f, int c){
    int pieza = tablero[f][c];
    if(pieza==0) return false;

    int direcciones[4][2] = {{-1,-1}, {-1,1}, {1,-1}, {1,1}};

    for(int i=0; i<4; i++){
        int df = direcciones[i][0];
        int dc = direcciones[i][1];

        if(pieza==1 && df!= -1) continue; 
        if(pieza==2 && df!= 1) continue; 

        int fMedio = f + df;
        int cMedio = c + dc;
        int fDestino = f + df*2;
        int cDestino = c + dc*2;

        if(fMedio < 0 || fMedio >= 8 || cMedio < 0 || cMedio >= 8) continue;
        if(fDestino < 0 || fDestino >= 8 || cDestino < 0 || cDestino >= 8) continue;
        if(tablero[fDestino][cDestino]!= 0) continue;

        int enemigo = tablero[fMedio][cMedio];

        if((pieza==1 || pieza==3) && (enemigo==2 || enemigo==4)) return true;
        if((pieza==2 || pieza==4) && (enemigo==1 || enemigo==3)) return true;
    }
    return false;
}

bool HayComidaObligatoria(int turno){
    for(int f=0; f<8; f++){
        for(int c=0; c<8; c++){
            if((turno==1 && (tablero[f][c]==1 || tablero[f][c]==3)) ||
               (turno==2 && (tablero[f][c]==2 || tablero[f][c]==4))){
                if(PuedeComer(f,c)) return true;
            }
         }
      }
    return false;
}

bool esMovimientoValido(int fo, int co, int fd, int cd, int turno){
    if(fd<0||fd>=8||cd<0||cd>=8) return false;
    if(tablero[fd][cd]!=0) return false;
    if((fd+cd)%2==0) return false; 
    if(abs(fd-fo)!=abs(cd-co)) return false; 

    int pieza = tablero[fo][co];
    int df = fd - fo;

    if(HayComidaObligatoria(turno) && abs(df)!=2) return false;

    if(abs(df)==1){
        if(pieza==1 && df>0) return false;
        if(pieza==2 && df<0) return false;
        return true;
    }

    if(abs(df)==2){
        int fm=(fo+fd)/2;
        int cm=(co+cd)/2;
        int enemigo=tablero[fm][cm];
        if(turno==1 && (enemigo==2 || enemigo==4)) return true;
        if(turno==2 && (enemigo==1 || enemigo==3)) return true;
    }
    return false;
}

void MovimientosTablero(int fo, int co, int fd, int cd){
    if(abs(fd-fo)==2){
        tablero[(fo+fd)/2][(co+cd)/2]=0; 
    }
    tablero[fd][cd]=tablero[fo][co];
    tablero[fo][co]=0;

    if(tablero[fd][cd]==1 && fd==0){
         tablero[fd][cd]=3;
         cout << "¡La ficha blanca coronó!\n";
   }
    if(tablero[fd][cd]==2 && fd==7){
         tablero[fd][cd]=4;
         cout << "¡La ficha negra coronó!\n";
    }
}

int main(){
    SetConsoleOutputCP(CP_UTF8);
    system(" ");

    int opcion;
    bool iniciarJuego = false;

    do{
        system("cls");
        cout << "====================================================" << endl ;
        cout << "                    JUEGO  DE  DAMAS                    " << endl ;
        cout << "====================================================" << endl ;

        cout << "1. Nueva Partida" << endl;
        cout << "2. Cargar partida guardada" << endl;
        cout << "3. Salir" << endl;
        cout << "Elige una opcion: ";
        cin >> opcion;

        if(cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if(opcion == 1) {
            InicializarTablero();
            tiempoBlancas = 300; 
            tiempoNegras = 300;
            turno = 1;
            partidaTerminada = false;
            tiempoAgotado = false;
            iniciarJuego = true;
        }
        else if(opcion == 2) {
            if(CargarPartida()) {
                partidaTerminada = false;
                tiempoAgotado = false;
                iniciarJuego = true;
            } else {
                iniciarJuego = false;
            }
        }

        if(iniciarJuego) {
            int fo, co, fd, cd;
            auto ultimoTiempo = steady_clock::now();

            while(true){
                system("cls");

                MostrarTablero();
                MostrarRelojes();

                if(tiempoBlancas <= 0) { 
                    cout << "¡GANAN LAS NEGRAS POR TIEMPO!" << endl;
                    system("pause");
                    break; 
                }
                if(tiempoNegras <= 0) { 
                    cout << "¡GANAN LAS BLANCAS POR TIEMPO!" << endl;
                    system("pause");
                    break; 
                }
                
                cout << "____________________________________________" << endl; 
                cout << "|Si quieres salir de la partida escribe -1 |" << endl; 
                cout << "|Si quieres guardar la partida escribe  -2 |" << endl;
                cout << "|__________________________________________|" << endl; 

                cout << "Turno " << (turno==1? "BLANCAS ⚪" : "NEGRAS ⚫") << endl;
                if(HayComidaObligatoria(turno))
                    cout << "¡COMIDA OBLIGATORIA!" << endl;

                char colOrigenChar, colDestinoChar;

                cout << "Origen ficha (ej: 5 A): " ; 
                cin >> fo;
                if(fo==-1) break;
                if(fo == -2) {
                    GuardarPartida();
                    continue;
                }

                cin >> colOrigenChar;

                cout << "Destino ficha (ej: 4 B): "; 
                cin >> fd >> colDestinoChar;

                if(cin.fail()) {
                    cin.clear(); 
                    cin.ignore(10000, '\n'); 
                    cout << "Entrada inválida. Usa formato de número y letra (ej: 5 A)\n";
                    system("pause");
                    continue;
                }
                
                colOrigenChar = toupper(colOrigenChar);
                colDestinoChar = toupper(colDestinoChar);
                co = colOrigenChar - 'A';
                cd = colDestinoChar - 'A';

                if(fo<0||fo>=8||co<0||co>=8){
                    cout << "Origen fuera del tablero\n";
                    system("pause"); 
                    continue;
                }
                if(turno==1 && tablero[fo][co]!=1 && tablero[fo][co]!=3){
                    cout << "Esa no es una blanca tuya\n";
                    system("pause");
                    continue;
                }
                if(turno==2 && tablero[fo][co]!=2 && tablero[fo][co]!=4){
                    cout << "Esa no es una negra tuya\n"; 
                    system("pause");
                    continue;
                }

                if(esMovimientoValido(fo, co, fd, cd, turno)) {
                    GuardamovimientoenArchivo(turno, fo, co, fd, cd);

                    MovimientosTablero(fo, co, fd, cd);
                    auto ahora = steady_clock::now();
                    int segGastados = duration_cast<seconds>(ahora - ultimoTiempo).count();

                    if(turno == 1) tiempoBlancas -= segGastados;
                    else tiempoNegras -= segGastados;

                    turno = (turno==1)? 2 : 1; 
                    ultimoTiempo = steady_clock::now();

                } else {
                    cout << "MOVIMIENTO ILEGAL\n";
                    system("pause");
                }
            }
        }
    }while(opcion != 3);

    cout << "Gracias por jugar, saliendo del juego...\n"; 
    return 0;
}