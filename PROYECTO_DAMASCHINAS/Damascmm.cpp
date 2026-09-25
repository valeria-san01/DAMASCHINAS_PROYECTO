#include <iostream>
#include <cmath>
#include <cctype>
#include <thread>
#include <chrono>
#include <atomic>
#include <fstream>
#include <conio.h>
#include <windows.h> 
#include <sstream>
#include <vector>

using namespace std;
using namespace std::chrono;

int tablero[8][8];

int tiempoBlancas = 300; // 5 minutos
int tiempoNegras = 300;
int turno = 1; // Controla los turnos 

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
    for(int f=0; f<3; f++){ // Fichas negras
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

    // VALIDACIÓN: Evitar que se guarde con un nombre existente
    if (ExisteArchivo(nombreArchivoBin)) {
        cout << "\n[ERROR] ¡Ya existe una partida guardada con ese nombre! Elige otro diferente.\n";
        system("pause");
        return;
    }

    // Guardar datos binarios
    ofstream archivo(nombreArchivoBin, ios::binary);
    if (archivo.is_open()) {
        archivo.write(reinterpret_cast<char*>(&turno), sizeof(turno));
        archivo.write(reinterpret_cast<char*>(&tiempoBlancas), sizeof(tiempoBlancas));
        archivo.write(reinterpret_cast<char*>(&tiempoNegras), sizeof(tiempoNegras));
        archivo.write(reinterpret_cast<char*>(tablero), sizeof(tablero));
        archivo.close();

        // Registrar en archivo índice
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

// Tablero que resalta la posición del cursor con las flechitas
void MostrarTableroConCursor(int cursorF, int cursorC){
    cout << "    A   B   C   D   E   F   G   H" << endl;
    
    for(int f=0; f<8; f++){
        cout << f << " ";
        
        for(int c=0; c<8; c++){
            bool casillaOscura = (f + c) % 2 == 1;
            string colorFondo;

            if (f == cursorF && c == cursorC) {
                colorFondo = "\033[48;5;33m"; // Azul brillante para la celda seleccionada por el cursor
            } else {
                colorFondo = casillaOscura ? "\033[48;5;94m" : "\033[48;5;223m";
            }

            cout << colorFondo; 

            if(tablero[f][c] == 0) {
                cout << "    "; 
            }
            else {
                string fichaStr = "";
                if(tablero[f][c] == 1) fichaStr = "\033[38;5;15m⚪\033[0m"; // Blanca
                else if(tablero[f][c] == 2) fichaStr = "\033[38;5;0m⚫\033[0m";  // Negra
                else if(tablero[f][c] == 3) fichaStr = "\033[38;5;21m♔\033[0m"; // Dama blanca
                else if(tablero[f][c] == 4) fichaStr = "\033[38;5;5m♚\033[0m";  // Dama negra

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

        // Bucle principal del juego usando las FLECHAS del teclado
        if(iniciarJuego) {
            auto ultimoTiempo = steady_clock::now();

            while(true){
                int cursorF = 0, cursorC = 0;
                int fo = -1, co = -1, fd = -1, cd = -1;
                bool seleccionandoOrigen = true;

                // --- FASE 1: SELECCIONAR ORIGEN CON FLECHAS ---
                while (seleccionandoOrigen) {
                    system("cls");
                    MostrarTableroConCursor(cursorF, cursorC);
                    MostrarRelojes();

                    if(tiempoBlancas <= 0) { 
                        cout << "¡GANAN LAS NEGRAS POR TIEMPO!" << endl;
                        system("pause");
                        fo = -1; break; 
                    }
                    if(tiempoNegras <= 0) { 
                        cout << "¡GANAN LAS BLANCAS POR TIEMPO!" << endl;
                        system("pause");
                        fo = -1; break; 
                    }

                    cout << "____________________________________________" << endl; 
                    cout << "| Usa las FLECHAS para mover el cursor     |" << endl;
                    cout << "| ENTER para seleccionar ficha             |" << endl;
                    cout << "| ESC para salir al menu principal         |" << endl;
                    cout << "| G para guardar partida                   |" << endl;
                    cout << "|__________________________________________|" << endl; 
                    cout << "Turno " << (turno==1? "BLANCAS ⚪" : "NEGRAS ⚫") << endl;
                    if(HayComidaObligatoria(turno))
                        cout << "¡COMIDA OBLIGATORIA!" << endl;

                    int tecla = _getch();
                    if (tecla == 224 || tecla == 0) {
                        int direccion = _getch();
                        if (direccion == 72) cursorF = (cursorF > 0) ? cursorF - 1 : 7;    // Arriba
                        else if (direccion == 80) cursorF = (cursorF < 7) ? cursorF + 1 : 0; // Abajo
                        else if (direccion == 75) cursorC = (cursorC > 0) ? cursorC - 1 : 7; // Izquierda
                        else if (direccion == 77) cursorC = (cursorC < 7) ? cursorC + 1 : 7; // Derecha
                    } 
                    else if (tecla == 13) { // ENTER
                        fo = cursorF;
                        co = cursorC;
                        
                        if(turno==1 && tablero[fo][co]!=1 && tablero[fo][co]!=3){
                            cout << "\n¡Esa no es una ficha blanca tuya!\n";
                            system("pause");
                            continue;
                        }
                        if(turno==2 && tablero[fo][co]!=2 && tablero[fo][co]!=4){
                            cout << "\n¡Esa no es una ficha negra tuya!\n"; 
                            system("pause");
                            continue;
                        }
                        seleccionandoOrigen = false; 
                    }
                    else if (tecla == 27) { // ESC
                        fo = -1;
                        break;
                    }
                    else if (tecla == 'g' || tecla == 'G') { // Guardar
                        GuardarPartida();
                    }
                }

                if(fo == -1) break; // Salir al menú principal

                // --- FASE 2: SELECCIONAR DESTINO CON FLECHAS ---
                bool seleccionandoDestino = true;
                while (seleccionandoDestino) {
                    system("cls");
                    MostrarTableroConCursor(cursorF, cursorC);
                    MostrarRelojes();

                    cout << "____________________________________________" << endl; 
                    cout << "| Seleccionando DESTINO para ficha en (" << fo << ", " << (char)('A' + co) << ") |" << endl;
                    cout << "| ENTER para confirmar movimiento          |" << endl;
                    cout << "| Otra vez enter para cambiar origen       |" << endl;
                    cout << "|__________________________________________|" << endl; 

                    int tecla = _getch();
                    if (tecla == 224 || tecla == 0) {
                        int direccion = _getch();
                        if (direccion == 72) cursorF = (cursorF > 0) ? cursorF - 1 : 7;
                        else if (direccion == 80) cursorF = (cursorF < 7) ? cursorF + 1 : 0;
                        else if (direccion == 75) cursorC = (cursorC > 0) ? cursorC - 1 : 7;
                        else if (direccion == 77) cursorC = (cursorC < 7) ? cursorC + 1 : 7;
                    } 
                    else if (tecla == 13) { // ENTER
                        fd = cursorF;
                        cd = cursorC;
                        seleccionandoDestino = false;
                    }
                    else if (tecla == 8) { // Backspace (Borrar) para re-elegir origen
                        seleccionandoDestino = false;
                        seleccionandoOrigen = true;
                        break;
                    }
                }

                if (seleccionandoOrigen) continue; 

                // --- VALIDACIÓN Y EJECUCIÓN DEL MOVIMIENTO ---
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
                    cout << "\n¡MOVIMIENTO ILEGAL!\n";
                    system("pause");
                }
            }
        }
    } while(opcion != 3);

    cout << "Gracias por jugar, saliendo del juego...\n"; 
    return 0;
}