#include <iostream> // Entrada y salida de datos
#include <cmath>    // Operaciones matemáticas
#include <cctype>   // Manipula y clasifica caracteres
#include <thread>   // Gestiona procesos de ejecución
#include <chrono>   // Gestiona el tiempo
#include <atomic>   // Sincroniza y ordena el acceso a la memoria
#include <fstream>  // Lee y escribe archivos
#include <conio.h>  // Entrada y salida de consola (principalmente para _kbhit en otros sistemas, pero usado aquí para Windows)
#include <windows.h> // Para activar colores y emojis en Windows (CP_UTF8)
#include <sstream>  // Opera sobre cadenas
#include <iomanip>  // Para std::setw y std::setfill en formato de tiempo

// Usamos los namespaces para simplificar el código
using namespace std;
using namespace std::chrono;

// --- Variables Globales ---
// Tablero de juego: 0 = vacío, 1 = ficha blanca, 2 = ficha negra, 3 = dama blanca, 4 = dama negra
int tablero[8][8];

// Tiempos iniciales y actuales para cada jugador en segundos
// Se inicializan a 5 minutos (300 segundos)
int tiempoBlancas = 300;
int tiempoNegras = 300;

// Control del turno: 1 para blancas, 2 para negras
int turno = 1;

// Flags atómicos para la sincronización del hilo del temporizador
atomic<bool> temporizadorActivo(false); // Indica si el temporizador debe contar
atomic<bool> tiempoAgotado(false);      // Indica si algún jugador ha agotado su tiempo
atomic<bool> partidaTerminada(false);   // Indica si la partida ha finalizado

// --- Funciones de Utilidad ---

/**
 * @brief Formatea un número de segundos a un string MM:SS.
 * @param segundos El número total de segundos.
 * @return String formateado como "minutos:segundos".
 */
string formatoTiempo(int segundos) {
    if (segundos < 0) segundos = 0; // Asegura que el tiempo no sea negativo
    int minutos = segundos / 60;
    int segs = segundos % 60;
    // Usa std::setw y std::setfill para asegurar dos dígitos en los segundos (ej: 05 en lugar de 5)
    stringstream ss;
    ss << minutos << ":" << setw(2) << setfill('0') << segs;
    return ss.str();
}

/**
 * @brief Muestra los tiempos restantes de ambos jugadores en la consola.
 */
void MostrarRelojes() {
    cout << "\n--- RELOJ ---" << endl;
    cout << "BLANCAS: " << formatoTiempo(tiempoBlancas) << endl;
    cout << "NEGRAS:  " << formatoTiempo(tiempoNegras) << endl;
    cout << "-------------" << endl;
}

// --- Hilo del Temporizador ---

/**
 * @brief Función ejecutada en un hilo separado para gestionar el temporizador del ajedrez.
 * Decrementa el tiempo del jugador actual cada segundo mientras la partida esté activa y el temporizador habilitado.
 * Detecta el agotamiento del tiempo y marca la partida como terminada.
 */
void temporizadorAjedrez() {
    // Bucle principal del hilo del temporizador
    while (!partidaTerminada) {
        // Si el temporizador está activo, contamos el tiempo
        if (temporizadorActivo) {
            // Esperamos 1 segundo. Esta es la unidad de tiempo que se decrementa.
            this_thread::sleep_for(chrono::seconds(1));

            // Verificamos de nuevo las condiciones de salida después de la espera
            if (!temporizadorActivo || partidaTerminada)
                continue; // Si se desactivó o terminó mientras esperábamos, salimos de esta iteración

            // Decrementamos el tiempo del jugador actual
            if (turno == 1) {
                tiempoBlancas--;
            } else {
                tiempoNegras--;
            }

            // Verificamos si el tiempo de las blancas se agotó
            if (tiempoBlancas <= 0) {
                tiempoBlancas = 0; // Aseguramos que el tiempo no sea negativo
                tiempoAgotado = true;
                partidaTerminada = true; // Terminamos la partida

                // Mensaje de fin de partida por tiempo
                cout << "\n\n¡TIEMPO AGOTADO DE BLANCAS!\n";
                cout << "¡LAS NEGRAS GANAN POR TIEMPO!\n";
                // No es necesario un break aquí, el bucle principal detectará partidaTerminada
            }

            // Verificamos si el tiempo de las negras se agotó
            if (tiempoNegras <= 0) {
                tiempoNegras = 0; // Aseguramos que el tiempo no sea negativo
                tiempoAgotado = true;
                partidaTerminada = true; // Terminamos la partida

                // Mensaje de fin de partida por tiempo
                cout << "\n¡TIEMPO AGOTADO DE NEGRAS!\n";
                cout << "¡LAS BLANCAS GANAN POR TIEMPO!\n";
                // No es necesario un break aquí, el bucle principal detectará partidaTerminada
            }
        } else {
            // Si el temporizador no está activo, esperamos un corto período para no consumir CPU innecesariamente
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
    // Al salir del bucle, el hilo del temporizador termina.
}

// --- Inicialización y Visualización del Tablero ---

/**
 * @brief Inicializa el tablero de damas con la configuración inicial.
 * Las fichas negras se colocan en las filas superiores, las blancas en las inferiores.
 * Solo se colocan fichas en las casillas oscuras.
 */
void InicializarTablero() {
    // Limpia todo el tablero a 0 (casillas vacías)
    for (int f = 0; f < 8; ++f) {
        for (int c = 0; c < 8; ++c) {
            tablero[f][c] = 0;
        }
    }

    // Coloca las fichas negras (valor 2) en las primeras 3 filas
    for (int f = 0; f < 3; ++f) {
        for (int c = 0; c < 8; ++c) {
            // Las fichas solo van en casillas oscuras (suma de fila y columna impar)
            if ((f + c) % 2 == 1) {
                tablero[f][c] = 2; // Ficha negra
            }
        }
    }

    // Coloca las fichas blancas (valor 1) en las últimas 3 filas
    for (int f = 5; f < 8; ++f) {
        for (int c = 0; c < 8; ++c) {
            // Las fichas solo van en casillas oscuras
            if ((f + c) % 2 == 1) {
                tablero[f][c] = 1; // Ficha blanca
            }
        }
    }
}

/**
 * @brief Muestra el tablero de damas en la consola con colores y emojis.
 * Utiliza secuencias de escape ANSI para los colores de fondo y texto.
 */
void MostrarTablero() {
    // Encabezado de columnas (A-H)
    cout << "    A   B   C   D   E   F   G   H" << endl;

    // Definiciones de colores ANSI para casillas y fichas
    // Nota: Estos códigos pueden variar ligeramente entre terminales.
    const string colorFondoOscuro = "\033[48;5;94m";  // Fondo oscuro (ej: azul oscuro)
    const string colorFondoClaro = "\033[48;5;223m"; // Fondo claro (ej: gris claro)
    const string colorReset = "\033[0m";            // Resetea todos los atributos de color

    const string colorFichaBlanca = "\033[38;5;15m⚪"; // Blanco brillante
    const string colorFichaNegra = "\033[38;5;0m⚫";   // Negro
    const string colorDamaBlanca = "\033[38;5;21m♔";  // Blanco con borde (ej: amarillo)
    const string colorDamaNegra = "\033[38;5;5m♚";   // Negro con borde (ej: rojo)

    // Iteramos sobre cada fila del tablero
    for (int f = 0; f < 8; ++f) {
        // Número de fila (0-7)
        cout << f << " ";

        // Iteramos sobre cada columna de la fila
        for (int c = 0; c < 8; ++c) {
            // Determinamos si la casilla actual es oscura o clara
            bool casillaOscura = (f + c) % 2 == 1;
            string colorFondoActual = casillaOscura ? colorFondoOscuro : colorFondoClaro;

            // Aplicamos el color de fondo de la casilla
            cout << colorFondoActual;

            // Verificamos el contenido de la casilla
            if (tablero[f][c] == 0) {
                // Casilla vacía: imprimimos 4 espacios para mantener el tamaño de la celda
                cout << "    ";
            } else {
                // Casilla con ficha: determinamos el emoji y color de la ficha
                string fichaStr = "";
                switch (tablero[f][c]) {
                    case 1: fichaStr = colorFichaBlanca; break; // Ficha blanca
                    case 2: fichaStr = colorFichaNegra; break;  // Ficha negra
                    case 3: fichaStr = colorDamaBlanca; break; // Dama blanca
                    case 4: fichaStr = colorDamaNegra; break;  // Dama negra
                }
                // Imprimimos: espacio + emoji + color de fondo restaurado + espacio de relleno
                // Esto asegura que el emoji esté centrado y el fondo de la celda se mantenga.
                cout << " " << fichaStr << colorFondoActual << " ";
            }
            // Reseteamos el color al final de cada celda para evitar propagación
            cout << colorReset;
        }
        // Nueva línea al final de cada fila
        cout << endl;
    }
}

// --- Lógica de Juego ---

/**
 * @brief Guarda un movimiento realizado en un archivo de texto.
 * @param turno El turno en el que se realizó el movimiento (1 para blancas, 2 para negras).
 * @param fo Fila de origen.
 * @param co Columna de origen.
 * @param fd Fila de destino.
 * @param cd Columna de destino.
 */
void GuardamovimientoenArchivo(int turno, int fo, int co, int fd, int cd) {
    // Abre el archivo en modo append (añadir al final)
    ofstream archivo("partida_guardada.txt", ios::app);

    if (archivo.is_open()) {
        // Convierte la columna numérica a letra (0 -> A, 1 -> B, etc.)
        char co_letra = 'A' + co;
        char cd_letra = 'A' + cd;

        // Escribe la información del movimiento en el archivo
        archivo << "Turno de " << (turno == 1 ? "BLANCAS" : "NEGRAS") << ": "
                << "Mover de (" << fo << ", " << co_letra << ") "
                << "a destino (" << fd << ", " << cd_letra << ")\n";
        archivo.close(); // Cierra el archivo
    } else {
        // Manejo de error si el archivo no se puede abrir
        cerr << "Error: No se pudo abrir el archivo 'partida_guardada.txt' para escribir." << endl;
    }
}

/**
 * @brief Verifica si una pieza en la posición (f, c) puede realizar una captura.
 * Considera las direcciones de movimiento permitidas para fichas y damas.
 * @param f Fila de la pieza.
 * @param c Columna de la pieza.
 * @return true si la pieza puede comer, false en caso contrario.
 */
bool PuedeComer(int f, int c) {
    int pieza = tablero[f][c];
    if (pieza == 0) return false; // No hay pieza en la casilla

    // Direcciones de movimiento posibles para una captura (diagonal, 2 casillas)
    // {df, dc}: cambio en fila, cambio en columna
    int direcciones[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    // Iteramos sobre las 4 direcciones diagonales
    for (int i = 0; i < 4; ++i) {
        int df = direcciones[i][0]; // Cambio en fila
        int dc = direcciones[i][1]; // Cambio en columna

        // Restricciones de movimiento para fichas simples:
        // Ficha blanca (1) solo puede moverse hacia arriba (df = -1)
        if (pieza == 1 && df != -1) continue;
        // Ficha negra (2) solo puede moverse hacia abajo (df = 1)
        if (pieza == 2 && df != 1) continue;
        // Las damas (3, 4) pueden moverse en cualquier dirección diagonal, así que no hay restricción aquí.

        // Calculamos la posición de la casilla intermedia (donde estaría la pieza a capturar)
        int fMedio = f + df;
        int cMedio = c + dc;
        // Calculamos la posición de la casilla de destino (después de la captura)
        int fDestino = f + df * 2;
        int cDestino = c + dc * 2;

        // Verificamos si las casillas intermedia y de destino están dentro del tablero
        if (fMedio < 0 || fMedio >= 8 || cMedio < 0 || cMedio >= 8) continue;
        if (fDestino < 0 || fDestino >= 8 || cDestino < 0 || cDestino >= 8) continue;

        // Verificamos si la casilla de destino está vacía (condición necesaria para la captura)
        if (tablero[fDestino][cDestino] != 0) continue;

        // Obtenemos la pieza en la casilla intermedia (la que podría ser capturada)
        int enemigo = tablero[fMedio][cMedio];

        // Verificamos si la pieza en (fMedio, cMedio) es un enemigo
        // Si la pieza actual es blanca (1 o 3) y la intermedia es negra (2 o 4)
        if ((pieza == 1 || pieza == 3) && (enemigo == 2 || enemigo == 4)) return true;
        // Si la pieza actual es negra (2 o 4) y la intermedia es blanca (1 o 3)
        if ((pieza == 2 || pieza == 4) && (enemigo == 1 || enemigo == 3)) return true;
    }
    // Si no se encontró ninguna captura posible en ninguna dirección
    return false;
}

/**
 * @brief Verifica si existe alguna captura obligatoria para el jugador cuyo turno es.
 * @param turno El turno del jugador actual (1 para blancas, 2 para negras).
 * @return true si hay al menos una captura obligatoria, false en caso contrario.
 */
bool HayComidaObligatoria(int turno) {
    // Iteramos sobre todo el tablero
    for (int f = 0; f < 8; ++f) {
        for (int c = 0; c < 8; ++c) {
            // Verificamos si la pieza en (f, c) pertenece al jugador actual
            bool esPiezaDelJugador = false;
            if (turno == 1) { // Turno de blancas
                if (tablero[f][c] == 1 || tablero[f][c] == 3) { // Ficha blanca o dama blanca
                    esPiezaDelJugador = true;
                }
            } else { // Turno de negras
                if (tablero[f][c] == 2 || tablero[f][c] == 4) { // Ficha negra o dama negra
                    esPiezaDelJugador = true;
                }
            }

            // Si es una pieza del jugador actual, verificamos si puede comer
            if (esPiezaDelJugador) {
                if (PuedeComer(f, c)) {
                    return true; // Si encontramos una pieza que puede comer, retornamos true inmediatamente
                }
            }
        }
    }
    // Si recorrimos todo el tablero y no encontramos ninguna captura obligatoria
    return false;
}

/**
 * @brief Valida si un movimiento propuesto es legal según las reglas de las damas.
 * Considera movimientos simples, capturas, coronaciones y la regla de captura obligatoria.
 * @param fo Fila de origen.
 * @param co Columna de origen.
 * @param fd Fila de destino.
 * @param cd Columna de destino.
 * @param turno El turno del jugador actual.
 * @return true si el movimiento es válido, false en caso contrario.
 */
bool esMovimientoValido(int fo, int co, int fd, int cd, int turno) {
    // 1. Verificaciones básicas de límites y casilla de destino
    if (fd < 0 || fd >= 8 || cd < 0 || cd >= 8) return false; // Destino fuera del tablero
    if (tablero[fd][cd] != 0) return false;                  // Casilla de destino ocupada
    if ((fd + cd) % 2 == 0) return false;                   // Solo se puede mover a casillas oscuras (suma de índices impar)

    // 2. Verificaciones de movimiento diagonal
    if (abs(fd - fo) != abs(cd - co)) return false; // El movimiento debe ser estrictamente diagonal

    int pieza = tablero[fo][co];
    int df = fd - fo; // Diferencia en filas (positiva si baja, negativa si sube)
    int dc = cd - co; // Diferencia en columnas

    // 3. Verificación de captura obligatoria
    bool comidaObligatoria = HayComidaObligatoria(turno);
    // Si hay comida obligatoria, el movimiento debe ser una captura (distancia 2)
    if (comidaObligatoria && abs(df) != 2) return false;

    // 4. Verificación de movimientos simples (distancia 1)
    if (abs(df) == 1) {
        // Ficha blanca (1) solo puede subir (df < 0)
        if (pieza == 1 && df > 0) return false;
        // Ficha negra (2) solo puede bajar (df > 0)
        if (pieza == 2 && df < 0) return false;
        // Si es una dama (3, 4), puede moverse en cualquier dirección diagonal simple.
        // Si las condiciones anteriores se cumplen, el movimiento simple es válido.
        return true;
    }

    // 5. Verificación de movimientos de captura (distancia 2)
    if (abs(df) == 2) {
        // Calculamos la casilla intermedia (la que contiene la pieza a capturar)
        int fm = (fo + fd) / 2;
        int cm = (co + cd) / 2;
        int enemigo = tablero[fm][cm]; // Pieza en la casilla intermedia

        // Verificamos si la pieza en la casilla intermedia es un enemigo del jugador actual
        if (turno == 1) { // Turno de blancas
            // Si la pieza es blanca (1 o 3) y la intermedia es negra (2 o 4)
            if (enemigo == 2 || enemigo == 4) return true;
        } else { // Turno de negras
            // Si la pieza es negra (2 o 4) y la intermedia es blanca (1 o 3)
            if (enemigo == 1 || enemigo == 3) return true;
        }
        // Si la casilla intermedia no contiene un enemigo válido, la captura no es posible.
        return false;
    }

    // Si el movimiento no es de distancia 1 ni 2, o no cumple las condiciones anteriores
    return false;
}

/**
 * @brief Ejecuta el movimiento de una pieza en el tablero.
 * Actualiza las posiciones de las fichas y maneja las capturas y coronaciones.
 * @param fo Fila de origen.
 * @param co Columna de origen.
 * @param fd Fila de destino.
 * @param cd Columna de destino.
 */
void MovimientosTablero(int fo, int co, int fd, int cd) {
    // Si el movimiento es una captura (distancia 2 en filas)
    if (abs(fd - fo) == 2) {
        // Eliminamos la pieza capturada de la casilla intermedia
        tablero[(fo + fd) / 2][(co + cd) / 2] = 0;
    }

    // Movemos la pieza de origen a la casilla de destino
    tablero[fd][cd] = tablero[fo][co];
    // Dejamos la casilla de origen vacía
    tablero[fo][co] = 0;

    // --- Manejo de Coronaciones ---
    // Si una ficha blanca (1) llega a la última fila (fila 0)
    if (tablero[fd][cd] == 1 && fd == 0) {
        tablero[fd][cd] = 3; // Se convierte en dama blanca (3)
        cout << "¡La ficha blanca coronó y se convirtió en DAMA ♔!\n";
    }
    // Si una ficha negra (2) llega a la primera fila (fila 7)
    if (tablero[fd][cd] == 2 && fd == 7) {
        tablero[fd][cd] = 4; // Se convierte en dama negra (4)
        cout << "¡La ficha negra coronó y se convirtió en DAMA ♚!\n";
    }
}

// --- Función Principal ---

int main() {
    // Configura la consola para soportar caracteres UTF-8 (necesario para emojis)
    SetConsoleOutputCP(CP_UTF8);
    // Activa el modo de procesamiento virtual de terminal para que las secuencias ANSI funcionen en Windows 10+
    system(" ");

    // Inicializa el tablero al inicio del juego
    InicializarTablero();

    // Variables para almacenar la entrada del usuario (origen y destino del movimiento)
    int fo, co, fd, cd;

    // Guarda el tiempo de inicio de la partida o del último movimiento para calcular el tiempo transcurrido
    auto ultimoTiempo = steady_clock::now();

    // Inicia el hilo del temporizador
    thread hiloTemporizador(temporizadorAjedrez);

    // Bucle principal del juego
    while (true) {
        
        system("cls");

        // Muestra el estado actual del tablero y los relojes
        MostrarTablero();
        MostrarRelojes();

        // --- Verificación de fin de partida por tiempo ---
        // Estas verificaciones se hacen en el bucle principal para asegurar que los mensajes se muestren
        // y el juego termine de forma limpia después de que el hilo del temporizador marque tiempoAgotado.
        if (tiempoBlancas <= 0) {
            cout << "\n¡GANAN LAS NEGRAS POR TIEMPO!" << endl;
            partidaTerminada = true; // Asegura que el hilo del temporizador también termine
            break; // Sale del bucle principal del juego
        }
        if (tiempoNegras <= 0) {
            cout << "\n¡GANAN LAS BLANCAS POR TIEMPO!" << endl;
            partidaTerminada = true; // Asegura que el hilo del temporizador también termine
            break; // Sale del bucle principal del juego
        }
        // Si la partida ya fue marcada como terminada por el hilo del temporizador (ej: mensaje de tiempo agotado)
        if (partidaTerminada) {
            break;
        }

        // Muestra el turno actual y si hay comida obligatoria
        cout << "Turno " << (turno == 1 ? "BLANCAS ⚪" : "NEGRAS ⚫") << endl;
        if (HayComidaObligatoria(turno)) {
            cout << "¡COMIDA OBLIGATORIA!" << endl;
        }

        // --- Entrada de Usuario para el Movimiento ---
        cout << "Origen ficha (fila columna, ej: 5 0) o -1 para salir: ";
        cin >> fo; // Lee la fila de origen

        // Permite al usuario salir del juego ingresando -1
        if (fo == -1) {
            partidaTerminada = true; // Indica al hilo del temporizador que termine
            break;
        }
        cin >> co; // Lee la columna de origen

        // Validación básica de la entrada de origen
        if (fo < 0 || fo >= 8 || co < 0 || co >= 8) {
            cout << "Origen fuera del tablero. Inténtalo de nuevo.\n";
            continue; // Vuelve al inicio del bucle
        }

        // Verifica si la pieza seleccionada pertenece al jugador actual
        if (turno == 1 && (tablero[fo][co] != 1 && tablero[fo][co] != 3)) {
            cout << "Esa no es una ficha blanca tuya. Inténtalo de nuevo.\n";
            continue;
        }
        if (turno == 2 && (tablero[fo][co] != 2 && tablero[fo][co] != 4)) {
            cout << "Esa no es una ficha negra tuya. Inténtalo de nuevo.\n";
            continue;
        }

        // Lee la entrada para el destino del movimiento
        cout << "Destino ficha (fila columna, ej: 4 1): ";
        cin >> fd >> cd;

        // --- Validación y Ejecución del Movimiento ---
        if (esMovimientoValido(fo, co, fd, cd, turno)) {
            // Si el movimiento es válido:
            // 1. Guarda el movimiento en el archivo
            GuardamovimientoenArchivo(turno, fo, co, fd, cd);

            // 2. Calcula el tiempo transcurrido desde el último movimiento
            auto ahora = steady_clock::now();
            int segGastados = duration_cast<seconds>(ahora - ultimoTiempo).count();

            // 3. Resta el tiempo gastado del jugador actual
            if (turno == 1) {
                tiempoBlancas -= segGastados;
            } else {
                tiempoNegras -= segGastados;
            }

            // 4. Ejecuta el movimiento en el tablero
            MovimientosTablero(fo, co, fd, cd);

            // 5. Cambia el turno al siguiente jugador
            turno = (turno == 1) ? 2 : 1;
            // 6. Actualiza el tiempo de referencia para el próximo cálculo
            ultimoTiempo = steady_clock::now();
            // 7. Activa el temporizador para el siguiente jugador
            temporizadorActivo = true;

        } else {
            // Si el movimiento no es válido
            cout << "MOVIMIENTO ILEGAL. Inténtalo de nuevo.\n";
            // No se cambia el turno ni se descuenta tiempo, el jugador puede reintentar.
        }
    }

    // Espera a que el hilo del temporizador termine antes de salir del programa principal
    hiloTemporizador.join();

    // Mensaje final al salir del juego
    cout << "\n¡Partida terminada! Gracias por jugar." << endl;

    return 0; // Indica que el programa finalizó correctamente
}