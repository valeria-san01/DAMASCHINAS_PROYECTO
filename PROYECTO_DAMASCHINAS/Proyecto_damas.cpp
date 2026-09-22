#include <iostream>
#include <cmath>
#include <cctype> 
#include <thread>
#include <chrono>
#include <atomic>
#include <fstream>
#include <conio.h> 

using namespace std;

int tablero[8][8];

// Variables atómicas para controlar el tiempo entre hilos
std::atomic<bool> tiempoAgotado(false);
std::atomic<bool> turnoTerminado(false);

// Función del temporizador en segundo plano
void temporizadorTurno(int segundos) {
    for (int i = 0; i < segundos; ++i) {
        if (turnoTerminado) return; 
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    if (!turnoTerminado) {
        tiempoAgotado = true;
        std::cout << "\n\n¡Se acabó el tiempo de este turno! \nPresiona cualquier tecla para continuar...\n";
    }
}

// Función para guardar el movimiento en archivo
void GuardamovimientoenArchivo(int turno, int fo, char co_letra, int fd, int cd) {
    ofstream archivo("partida_guardada.txt", ios::app);
    
    if (archivo.is_open()) {
        archivo << "Turno de " << (turno == 1 ? "BLANCAS" : "NEGRAS") << ": "
                << "Mover de (" << fo << ", " << co_letra << ") "
                << "a destino (" << fd << ", " << cd << ")\n";
        archivo.close();
    } else {
        cout << "No se pudo abrir el archivo para guardar la partida.\n";
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

void MostrarTablero(){
    cout << "   A  B  C  D  E  F  G  H" << endl; 
    for(int f=0; f<8; f++){
        cout << f << " ";
        for(int c=0; c<8; c++){
            bool casillaOscura = (f + c) % 2 == 1;
            if (casillaOscura) {
                cout << "\033[48;5;94m"; // Fondo marrón madera
            } else {
                cout << "\033[48;5;223m"; // Fondo beige / madera clara
            }

            if(tablero[f][c]==0) {
                cout << "   \033[0m"; 
            }
            else if(tablero[f][c]==1) {
                cout << "\033[38;5;232m\033[1m⚪ \033[0m"; 
            }
            else if(tablero[f][c]==2) {
                cout << "\033[38;5;16m\033[1m⚫ \033[0m"; 
            }
            else if(tablero[f][c]==3) {
                cout << "\033[38;5;21m\033[1m♔ \033[0m"; 
            }
            else if(tablero[f][c]==4) {
                cout << "\033[38;5;5m\033[1m♚ \033[0m"; 
            }
        }
        cout << endl; 
    }
}

bool PuedeComer(int f, int c){
    int pieza = tablero[f][c];
    if(pieza==0) return false;

    int direcciones[4][2] = {
        {-1,-1}, {-1,1},
        {1,-1}, {1,1}
    };

    for(int i=0; i<4; i++){
        int df = direcciones[i][0];
        int dc = direcciones[i][1];

        if(pieza==1 && df != -1) continue;
        if(pieza==2 && df != 1) continue;

        int fMedio = f + df;
        int cMedio = c + dc;
        int fDestino = f + df*2;
        int cDestino = c + dc*2;

        if(fMedio < 0 || fMedio >= 8 || cMedio < 0 || cMedio >= 8) continue;
        if(fDestino < 0 || fDestino >= 8 || cDestino < 0 || cDestino >= 8) continue;

        if(tablero[fDestino][cDestino] != 0) continue;

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
    system("chcp 65001 > nul"); // Permite ver emojis en Windows
    InicializarTablero();
    int turno = 1;

    while(true){
        MostrarTablero();
        cout << "Turno de " << (turno==1? "BLANCAS (b/B)" : "NEGRAS (n/N)") << endl;

        if(HayComidaObligatoria(turno))
            cout << "¡COMIDA OBLIGATORIA!" << endl;

        cout << " Tienes 30 segundos para hacer un movimiento \n";

        tiempoAgotado = false;
        turnoTerminado = false;

        std::thread hiloReloj(temporizadorTurno, 30);

        int fo, fd;
        char co_letra;
        char direccion;

        cout << "Origen fila (0-7) y columna (A-H): ";
        
        // Bucle para leer entrada comprobando si el tiempo se agotó
        while (!tiempoAgotado && !(_kbhit())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (tiempoAgotado) {
            turnoTerminado = true;
            if (hiloReloj.joinable()) hiloReloj.join();
            
            cout << "\n ¡Tiempo agotado! Pierdes el turno automáticamente.\n";
            cout << "Presiona ENTER para continuar...\n";
            cin.ignore(10000, '\n');
            cin.get(); 

            turno = (turno == 1) ? 2 : 1; 
            continue;
        }

        if (!(cin >> fo)) { 
            turnoTerminado = true;
            if (hiloReloj.joinable()) hiloReloj.join();
            cin.clear(); 
            cin.ignore(10000, '\n'); 
            cout << "Entrada inválida.\n";
            continue;
        }
        
        if (fo == -1) {
            turnoTerminado = true;
            if (hiloReloj.joinable()) hiloReloj.join();
            break; 
        }
        
        cin >> co_letra;
        int co = toupper(co_letra) - 'A'; 

        cout << "¿Hacia dónde quieres mover? (I = Izquierda, D = Derecha): ";
        cin >> direccion;

        turnoTerminado = true;

        if (hiloReloj.joinable()) {
            hiloReloj.join();
        }

        direccion = toupper(direccion);
        int cd;
        int distancia = 1;
        int avance;

        if(tablero[fo][co] == 1 || tablero[fo][co] == 3)
            avance = -1; 
        else
            avance = 1;  

        if(HayComidaObligatoria(turno))
            distancia = 2;

        fd = fo + avance * distancia;

        if(direccion == 'I')
            cd = co - distancia;
        else if(direccion == 'D')
            cd = co + distancia;
        else {
            cout << "Dirección inválida. Usa I o D.\n\n";
            continue;
        }

        if(fo<0||fo>=8||co<0||co>=8){
            cout << "Origen fuera del tablero o columna inválida\n\n"; continue;
        }
        if(turno==1 && tablero[fo][co]!=1 && tablero[fo][co]!=3){
            cout << "Esa no es una ficha blanca tuya\n\n"; continue;
        }
        if(turno==2 && tablero[fo][co]!=2 && tablero[fo][co]!=4){
            cout << "Esa no es una ficha negra tuya\n\n"; continue;
        }

        if(esMovimientoValido(fo, co, fd, cd, turno)){
            MovimientosTablero(fo, co, fd, cd);
            
            // GUARDAMOS EL MOVIMIENTO EN EL ARCHIVO
            GuardamovimientoenArchivo(turno, fo, co_letra, fd, cd);

            turno = (turno == 1) ? 2 : 1; 
        } else {
            cout << "Movimiento inválido. Intenta de nuevo.\n\n";
        }
    }
    return 0;
}