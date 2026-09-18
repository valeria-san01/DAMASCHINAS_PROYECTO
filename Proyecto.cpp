#include <iostream>
#include <cmath>
using namespace std;

int tablero[8][8];

void InicializarTablero(){
    for(int f=0; f<8; f++)
        for(int c=0; c<8; c++)
            tablero[f][c] = 0;

    for(int f=0; f<3; f++)
        for(int c=0; c<8; c++)
            if((f+c)%2==1) tablero[f][c]=2;

    for(int f=5; f<8; f++)
        for(int c=0; c<8; c++)
            if((f+c)%2==1) tablero[f][c]=1;
}

void MostrarTablero(){
    cout << " 0 1 2 3 4 5 6 7";
    for(int f=0; f<8; f++){
        cout << f << " ";
        for(int c=0; c<8; c++){
            if(tablero[f][c]==0) cout << ". ";
            else if(tablero[f][c]==2) cout << "n ";
            else if(tablero[f][c]==3) cout << "B ";
            else if(tablero[f][c]==4) cout << "N ";
        }
        cout << endl;
    }
}

// Revisa si una pieza puede comer desde esa posicion
bool PuedeComer(int f, int c){
    int pieza = tablero[f][c];
    if(pieza==0) return false;

    int direcciones[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};

    for(int i=0; i<4; i++){
        int df = direcciones[i][0];
        int dc = direcciones[i][1];

        // Peones normales solo comen hacia adelante
        if(pieza==1 && df!= -1) continue; // blanca sube
        if(pieza==2 && df!= 1) continue; // negra baja

        int fMedio = f + df;
        int cMedio = c + dc;
        int fDestino = f + df*2;
        int cDestino = c + dc*2;

        if(fDestino<0 || fDestino>=8 || cDestino<0 || cDestino>=8) continue;
        if(tablero[fDestino][cDestino]!=0) continue;

        int enemigo = tablero[fMedio][cMedio];
        if(pieza==1 || pieza==3){ // blanca o dama blanca
            if(enemigo==2 || enemigo==4) return true;
        }
        if(pieza==2 || pieza==4){ // negra o dama negra
            if(enemigo==1 || enemigo==3) return true;
        }
    }
    return false;
}

// Revisa si HAY alguna comida obligatoria en todo el tablero para ese turno
bool HayComidaObligatoria(int turno){
    for(int f=0; f<8; f++)
        for(int c=0; c<8; c++)
            if((turno==1 && (tablero[f][c]==1 || tablero[f][c]==3)) ||
               (turno==2 && (tablero[f][c]==2 || tablero[f][c]==4)))
                if(PuedeComer(f,c)) return true;
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

void MovimientosTablero(int fo, int co, int fd, int cd){
    if(abs(fd-fo)==2){
        tablero[(fo+fd)/2][(co+cd)/2]=0;
    }
    tablero[fd][cd]=tablero[fo][co];
    tablero[fo][co]=0;

    if(tablero[fd][cd]==1 && fd==0) tablero[fd][cd]=3;
    if(tablero[fd][cd]==2 && fd==7) tablero[fd][cd]=4;
}

int main(){
    InicializarTablero();
    int turno = 1;
    int fo, co, fd, cd;

    while(true){
        MostrarTablero();
        cout << "Turno " << (turno==1? "BLANCAS (b/B)" : "NEGRAS (n/N)") << endl;

        if(HayComidaObligatoria(turno))
            cout << "¡COMIDA OBLIGATORIA!" << endl;

        cout << "Origen fila col: ";
        cin >> fo >> co;
        if(fo==-1) break; // para salir

        cout << "Destino fila col: ";
        cin >> fd >> cd;

        if(fo<0||fo>=8||co<0||co>=8){
            cout << "Origen fuera del tablero"; continue;
        }
        if(turno==1 && tablero[fo][co]!=1 && tablero[fo][co]!=3){
            cout << "Esa no es una blanca tuya"; continue;
        }
        if(turno==2 && tablero[fo][co]!=2 && tablero[fo][co]!=4){
            cout << "Esa no es una negra tuya"; continue;
        }
    }//ACTUALIZAR LA CONSOLA, PONER COLOR 
    //HACER COMMIT DE CADA COSA Q HAGA , COMER OBLIGATORIO,PINTAR DICHAS
    //HACERLO MAS RAPIDO