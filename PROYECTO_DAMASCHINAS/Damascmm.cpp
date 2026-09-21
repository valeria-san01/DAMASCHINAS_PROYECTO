#include <iostream>
#include <cmath>
#include <cctype> // Necesario para toupper()
using namespace std;

int tablero[8][8];

// 1. Declaramos y definimos las funciones primero
void InicializarTablero(){
    for(int f=0; f<8; f++){
        for(int c=0; c<8; c++){
            tablero[f][c] = 0;
        }
    }
    for(int f=0; f<3; f++){ //fichas negras
        for(int c=0; c<8; c++){
            if((f+c)%2==1) tablero[f][c]=2;{
            }
        }
    }

    for(int f=5; f<8; f++){
        for(int c=0; c<8; c++){
            if((f+c)%2==1){ tablero[f][c]=1;
            }
        }
    }
            
}

void MostrarTablero(){
    cout << "   A  B  C  D  E  F  G  H" << endl; // Columnas en orden alfabético
    for(int f=0; f<8; f++){
        cout << f << " ";
        for(int c=0; c<8; c++){
            // Alternar colores simulando madera clara y madera oscura
            bool casillaOscura = (f + c) % 2 == 1;
            if (casillaOscura) {
                cout << "\033[48;5;94m"; // Fondo marrón madera
            } else {
                cout << "\033[48;5;223m"; // Fondo beige / madera clara
            }

            // Imprimir la pieza o celda vacía con colores contrastantes
            if(tablero[f][c]==0) {
                cout << "   \033[0m"; // Celda vacía manteniendo el fondo
            }
            else if(tablero[f][c]==1) {
                cout << "\033[38;5;232m\033[1m b \033[0m"; // Ficha blanca/clara
            }
            else if(tablero[f][c]==2) {
                cout << "\033[38;5;16m\033[1m n \033[0m"; // Ficha negra
            }
            else if(tablero[f][c]==3) {
                cout << "\033[38;5;21m\033[1m B \033[0m"; // Dama blanca (resaltada)
            }
            else if(tablero[f][c]==4) {
                cout << "\033[38;5;5m\033[1m N \033[0m"; // Dama negra (resaltada)
            }
        }
        cout << endl; // Salto de línea al terminar cada fila del tablero
    }
}

// Revisa si una pieza puede comer desde esa posicion
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

        // Fichas normales: solo comen hacia adelante
        if(pieza==1 && df != -1) continue;//blnca arriba 
        if(pieza==2 && df != 1) continue;//negra abajo

        int fMedio = f + df;
        int cMedio = c + dc;
        int fDestino = f + df*2;
        int cDestino = c + dc*2;

        // Comprobar que ambas casillas estén dentro del tablero
        if(fMedio < 0 || fMedio >= 8 ||
           cMedio < 0 || cMedio >= 8)
            continue;

        if(fDestino < 0 || fDestino >= 8 ||
           cDestino < 0 || cDestino >= 8)
            continue;

        // El destino debe estar vacío
        if(tablero[fDestino][cDestino] != 0)
            continue;

        int enemigo = tablero[fMedio][cMedio];

        // Blanca o dama blanca
        if((pieza==1 || pieza==3) &&
           (enemigo==2 || enemigo==4))
            return true;

        // Negra o dama negra
        if((pieza==2 || pieza==4) &&
           (enemigo==1 || enemigo==3))
            return true;
    }

    return false;
}

// Revisa si HAY alguna comida obligatoria en todo el tablero para ese turno
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

    // Si hay comida obligatoria, solo se permiten movimientos de 2
    if(HayComidaObligatoria(turno) && abs(df)!=2) return false;

    // movimiento de 1 casilla
    if(abs(df)==1){
        if(pieza==1 && df>0) return false;
        if(pieza==2 && df<0) return false;
        return true;
    }

    // movimiento de 2 casillas = comer
    if(abs(df)==2){
        int fm=(fo+fd)/2;
        int cm=(co+cd)/2;
        int enemigo=tablero[fm][cm];
        if(turno==1 && (enemigo==2 || enemigo==4)) return true;
        if(turno==2 && (enemigo==1 || enemigo==3)) return true;
    }
    return false;
}

void MovimientosTablero(int fo, int co, int fd, int cd)
{
    if(abs(fd-fo)==2){
        tablero[(fo+fd)/2][(co+cd)/2]=0;
    }
    tablero[fd][cd]=tablero[fo][co];
    tablero[fo][co]=0;
//CORONACIONES
    if(tablero[fd][cd]==1 && fd==0){
         tablero[fd][cd]=3;
    cout << "La ficha blanca corono!";
   }
    if(tablero[fd][cd]==2 && fd==7){
         tablero[fd][cd]=4;
    cout << "La ficha negra corono!";
  }
}

bool CalcularDestino(
    int fo,int co,char direccion,int distancia,int &fd,int &cd
){

    int pieza = tablero[fo][co];

    int df = 0;
    int dc = 0;

    direccion = toupper(direccion);

    // Fichas normales
    if(pieza==1 || pieza==2){

        if(direccion=='I')
            dc = -1;
        else if(direccion=='D')
            dc = 1;
        else
            return false;

        if(pieza==1)
            df = -1;
        else
            df = 1;
    }

    // Damas
    else if(pieza==3 || pieza==4){

        if(direccion=='1'){
            df = -1;
            dc = -1;
        }
        else if(direccion=='2'){
            df = -1;
            dc = 1;
        }
        else if(direccion=='3'){
            df = 1;
            dc = -1;
        }
        else if(direccion=='4'){
            df = 1;
            dc = 1;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }

    fd = fo + df * distancia;
    cd = co + dc * distancia;

    return true;
}

// 2. El main va al final para que ya conozca todas las funciones anteriores
int main(){
    InicializarTablero();
    int turno = 1;
    int fo, fd;
    char co_letra;
    char direccion;

    while(true){
        MostrarTablero();
        cout << "Turno " << (turno==1? "BLANCAS (b/B)" : "NEGRAS (n/N)") << endl;

        if(HayComidaObligatoria(turno))
            cout << "¡COMIDA OBLIGATORIA!" << endl;

        cout << "Origen fila (0-7) y columna (A-H): ";
        if (!(cin >> fo)) { // detecta el error de entrada
            cin.clear(); // Limpia el estado de error de cin
            cin.ignore(10000, '\n'); // Descarta la entrada incorrecta
            cout << "Entrada inválida.\n";
            continue;
        }
        if (fo == -1) break;//nos ayuda a salir del juego
        cin >> co_letra;
        int co = toupper(co_letra) - 'A'; // Convierte la letra a un número de 0 a 7

        cout << "¿Hacia dónde quieres mover? (I = Izquierda, D = Derecha): ";
        cin >> direccion;

        direccion = toupper(direccion);

        int cd;
        int distancia = 1;

        // Determinar la dirección de avance según el color
        int avance;

        if(tablero[fo][co] == 1 || tablero[fo][co] == 3)
            avance = -1; // Blancas suben
        else
            avance = 1;  // Negras bajan

        // Determinar si hay captura obligatoria
        if(HayComidaObligatoria(turno))
            distancia = 2;

        // Calcular la fila destino
        fd = fo + avance * distancia;

        // Calcular la columna destino
        if(direccion == 'I')
            cd = co - distancia;
        else if(direccion == 'D')
            cd = co + distancia;
        else {
            cout << "Dirección inválida. Usa I o D.\n\n";
            continue;
        }

        if(fo<0||fo>=8||co<0||co>=8){
            cout << "Origen fuera del tablero o columna invalida\n\n"; continue;
        }
        if(turno==1 && tablero[fo][co]!=1 && tablero[fo][co]!=3){
            cout << "Esa no es una blanca tuya\n\n"; continue;
        }
        if(turno==2 && tablero[fo][co]!=2 && tablero[fo][co]!=4){
            cout << "Esa no es una negra tuya\n\n"; continue;
        }

        if(esMovimientoValido(fo, co, fd, cd, turno)){
            MovimientosTablero(fo, co, fd, cd);
            turno = (turno == 1) ? 2 : 1; // Cambiar de turno
        } else {
            cout << "Movimiento invalido. Intenta de nuevo.\n\n";
        }
    }
    return 0;
}