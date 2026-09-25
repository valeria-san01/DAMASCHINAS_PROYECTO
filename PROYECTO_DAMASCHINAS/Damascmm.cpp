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
    cout << "  BLANCAS:" << formatoTiempo(tiempoBlancas) << endl;
    cout << "  NEGRAS:" << formatoTiempo(tiempoNegras) << endl;
    cout << "---------------" << endl;
}


void temporizadorAjedrez() {
    while (!partidaTerminada ) {
        if (temporizadorActivo) {
            this_thread::sleep_for(chrono::seconds(1));

            if (!temporizadorActivo || partidaTerminada)
                continue;

            if (turno == 1) {
                tiempoBlancas--;
            }
            else {
                tiempoNegras--;
            }

            if (tiempoBlancas <= 0) 
            {
                tiempoBlancas = 0;
                tiempoAgotado = true;
                partidaTerminada = true;

                cout << "\n¡Tiempo agotado de BLANCAS!\n";
                cout << "\n¡NEGRAS GANAN POR TIEMPO!\n";
            }

            if (tiempoNegras <= 0) {
                tiempoNegras = 0;
                tiempoAgotado = true;
                partidaTerminada = true;

                cout << "\n¡Tiempo agotado de NEGRAS!\n";
                cout << "\n¡BLANCAS GANAN POR TIEMPO!\n";
            }
        }
        else {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
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

void GuardarPartida() {
    ofstream archivo("partida_guardada.dat", ios::binary);
    if (archivo.is_open()) {
        archivo.write(reinterpret_cast<char*>(&turno), sizeof(turno));
        archivo.write(reinterpret_cast<char*>(&tiempoBlancas), sizeof(tiempoBlancas));
        archivo.write(reinterpret_cast<char*>(&tiempoNegras), sizeof(tiempoNegras));
        archivo.write(reinterpret_cast<char*>(tablero), sizeof(tablero));
        archivo.close();
        cout << "\n¡Partida guardada exitosamente!\n";
    } else {
        cout << "\nError al intentar guardar la partida.\n";
    }
    system("pause");
}

bool CargarPartida() {
    ifstream archivo("partida_guardada.dat", ios::binary);
    if (archivo.is_open()) {
        archivo.read(reinterpret_cast<char*>(&turno), sizeof(turno));
        archivo.read(reinterpret_cast<char*>(&tiempoBlancas), sizeof(tiempoBlancas));
        archivo.read(reinterpret_cast<char*>(&tiempoNegras), sizeof(tiempoNegras));
        archivo.read(reinterpret_cast<char*>(tablero), sizeof(tablero));
        archivo.close();
        cout << "\n¡Partida cargada con éxito!\n";
        system("pause");
        return true;
    } else {
        cout << "\nNo se encontró ninguna partida guardada.\n";
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
            
            // Definimos el código de fondo para reutilizarlo
            string colorFondo = casillaOscura ? "\033[48;5;94m" : "\033[48;5;223m";
            cout << colorFondo; // Aplicamos el fondo

            if(tablero[f][c] == 0) {
                cout << "    "; // 4 espacios vacíos
            }
            else {
                // Si hay ficha, imprimimos un espacio, el emoji, y re-aplicamos el fondo inmediatamente 
                // para que el resto de los espacios de la celda conserven el color del tablero.
                string fichaStr = "";
                if(tablero[f][c] == 1) fichaStr = "\033[38;5;15m⚪\033[0m"; // blanca
                else if(tablero[f][c] == 2) fichaStr = "\033[38;5;0m⚫\033[0m";  // negra
                else if(tablero[f][c] == 3) fichaStr = "\033[38;5;21m♔\033[0m"; // dama blanca
                else if(tablero[f][c] == 4) fichaStr = "\033[38;5;5m♚\033[0m";  // dama negra

                // Imprimimos: espacio + emoji + color de fondo restaurado + espacio de relleno
                cout << " " << fichaStr << colorFondo << " "; 
            }
            
            // Reseteamos el color al terminar la celda por completo
            cout << "\033[0m";
        }
        cout << endl;
    }
}

// Función para guardar el movimiento en archivo
void GuardamovimientoenArchivo(int turno, int fo, int co, int fd, int cd) {
    ofstream archivo("partida_guardada.txt", ios::app);

    if (archivo.is_open()) {
        char co_letra = 'A' + co; // Convierte 0 a A, 1 a B
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

        if(pieza==1 && df!= -1) continue; // blanca solo sube
        if(pieza==2 && df!= 1) continue; // negra solo baja

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
    if((fd+cd)%2==0) return false; // solo casillas negras
    if(abs(fd-fo)!=abs(cd-co)) return false; // solo diagonal

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
        tablero[(fo+fd)/2][(co+cd)/2]=0; // come
    }
    tablero[fd][cd]=tablero[fo][co];
    tablero[fo][co]=0;

    // CORONACIONES
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
                cout << "                  JUEGO  DE  DAMAS                  " << endl ;
                cout << "====================================================" << endl ;

                cout << "1. Nueva Partida" << endl;
                cout << "2. Cargar partida guardada" << endl;
                cout << "3. Salir" << endl;
                cout << "Elige una opcion: ";
                cin >> opcion;

                // Valida si el usuario escribe letras en el menú principal
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
            }
        }
             if(iniciarJuego) {

            int fo, co, fd, cd;
            auto ultimoTiempo = steady_clock::now();

            while(true){
            system("cls");

                MostrarTablero();
                MostrarRelojes();

                // Revisa si alguien perdió por tiempo
                if(tiempoBlancas <= 0) { cout << "¡GANAN LAS NEGRAS POR TIEMPO!" << endl;
                    system("pause");
                            break; 
                }
                if(tiempoNegras <= 0) { cout << "¡GANAN LAS BLANCAS POR TIEMPO!" << endl;
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
                if(fo==-1) break;//sale al menu pricipal
                if(fo == -2) {
                    GuardarPartida();
                    continue;
                }

                cin >> colOrigenChar;

                cout << "Destino ficha (ej: 4 B): "; 
                cin >> fd >> colDestinoChar;

                // Limpiar el flujo por si el usuario escribe algo inválido y evita el bucle infinito
                if(cin.fail()) {
                    cin.clear(); // Limpia el error
                    cin.ignore(10000, '\n'); // Descarta la entrada incorrecta
                    cout << "Entrada inválida. Usa formato de número y letra (ej: 5 A)\n";
                    system("pause");
                    continue;
                }
                
                // Convertir la letra de la columna a número (A/a -> 0, B/b -> 1, etc.)
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

                    turno = (turno==1)? 2 : 1; // Cambia de turno
                    ultimoTiempo = steady_clock::now();

                } else {
                    cout << "MOVIMIENTO ILEGAL\n";
                    system("pause");
                }
            }
        }
    }while(opcion !=3);

        cout << "gracias por jugar, saliendo del juego...\n"; 
        return 0;
    }