#ifndef TP_LOLLA_COLAPRIORTUPLA_HPP
#define TP_LOLLA_COLAPRIORTUPLA_HPP
#include "colaPriorTupla.h"
using namespace std;

template<class A, class B>
colaPriorTupla<A, B>::colaPriorTupla(bool compare(pair<A, B>, pair<A, B>)): cola(), posicionPorElemento(), cmp(compare), encolados(0) {}
// Complejidad: θ(1)

template<class A, class B>
void colaPriorTupla<A, B>::encolar(const pair<A, B> &a) {
    // Se agrega el elemento al final de la cola.
    cola.push_back(a); // O(1) amortizado. Al igual que en el TP2 lo tomamos como O(1) siempre.
    int i = cola.size() - 1; // θ(1)
    // Se actualiza el diccionario.
    posicionPorElemento[a] = i; // θ(log(n))
    // Se 'sube' el elemento, asegurando que se mantiene la propiedad del heap: mientras el elemento no es raíz y tiene mayor prioridad que su padre, se intercambia de lugar con el padre.
    while (i > 0 && cmp(cola[i], cola[(i-1) / 2])){ // En peor caso este ciclo itera log(n) veces (cuando el elemento encolado tiene que pasar a ser la raíz).
        posicionPorElemento[cola[i]] = (i - 1) / 2; // θ(log(n))
        posicionPorElemento[cola[(i - 1) / 2]] = i; // θ(log(n))
        swap(cola[i], cola[(i - 1) / 2]); // θ(1)
        i = (i - 1) / 2; // θ(1)
    }
    // Se aumenta en 1 el tamaño de la cola.
    encolados++; // θ(1)
}
// Complejidad: θ(log(n)^2), n cantidad de elementos encolados.

template<class A, class B>
void colaPriorTupla<A, B>::eliminar(int i){
    if (i == cola.size() - 1) {
        // Caso donde se elimina el último elemento de la cola.
        posicionPorElemento.erase(cola[i]); // θ(log(n))
        cola.pop_back(); // θ(1)
    } else {
        // Se elimina al par <elemento, posicion> del diccionario.
        posicionPorElemento.erase(cola[i]); // θ(log(n))
        // Se coloca el último elemento en la posición que se quiere eliminar.
        cola[i] = cola[cola.size() - 1]; // θ(1)
        // Se actualiza la posición del que era el último elemento de la cola y se hace un pop de la cola.
        posicionPorElemento[cola[i]] = i; // θ(log(n))
        cola.pop_back(); // O(1)
        if (i > 0 && cmp(cola[i], cola[(i-1) / 2])) {
            // Caso donde, el colocar el último elemento en la posición a eliminar hizo que quede "más abajo" de donde debería estar en el heap.
            // Se itera igual que en el algoritmo de encolar para "subir" el elemento en el heap.
            while (i > 0 && cmp(cola[i], cola[(i-1) / 2])) {
                posicionPorElemento[cola[i]] = (i - 1) / 2; // θ(log(n))
                posicionPorElemento[cola[(i - 1) / 2]] = i; // θ(log(n))
                swap(cola[i], cola[(i - 1) / 2]); // θ(1)
                i = (i - 1) / 2; // θ(1)
            }
        } else {
            // Caso donde, el colocar el último elemento en la posición a eliminar hizo que quede "más arriba" de donde debería estar en el heap. En este caso "se baja" el elemento:
            // mientras la prioridad del elemento sea menor que la prioridad de alguno de sus hijos, se van intercambiando de lugar.
            while( (2 * i + 1 < cola.size() and !cmp(cola[i], cola[2 * i + 1])) or (2 * i + 2 < cola.size() and !cmp(cola[i], cola[2 * i + 2]))) {
                if ( (2 * i + 1) < cola.size() and (2 * i + 2) < cola.size()) {
                    // Caso donde tiene tanto hijo izquierdo como derecho.
                    if (cmp(cola[2 * i + 1], cola[2 * i + 2])) {
                        // Caso donde el hijo izquierdo tiene mayor prioridad que el derecho.
                        posicionPorElemento[cola[i]] = 2 * i + 1; // θ(log(n))
                        posicionPorElemento[cola[2 * i + 1]] = i; // θ(log(n))
                        swap(cola[i], cola[2 * i + 1]); // θ(1)
                        i = 2 * i + 1;
                    } else {
                        // Caso donde el hijo derecho tiene mayor prioridad que el izquierdo.
                        posicionPorElemento[cola[i]] = 2 * i + 2; // θ(log(n))
                        posicionPorElemento[cola[2 * i + 2]] = i; // θ(log(n))
                        swap(cola[i], cola[2 * i + 2]); // θ(1)
                        i = 2 * i + 2;
                    }
                } else if (2 * i + 1 < cola.size()) {
                    // Caso donde tiene solo hijo derecho.
                    posicionPorElemento[cola[i]] = 2 * i + 1; // θ(log(n))
                    posicionPorElemento[cola[2 * i + 1]] = i; // θ(log(n))
                    swap(cola[i], cola[2 * i + 1]); // O(1)
                    i = 2 * i + 1;
                } else {
                    // Caso donde solo tiene hijo izquierdo.
                    posicionPorElemento[cola[i]] = 2 * i + 2; // θ(log(n))
                    posicionPorElemento[cola[2 * i + 2]] = i; // θ(log(n))
                    swap(cola[i], cola[2 * i + 2]); // θ(1)
                    i = 2 * i + 2;
                }
            }
        }
    }
    encolados--;
}
// El peor caso de eliminar un elemento es cuando se elimina la raíz, puesto que se intercambia con el último elemento y luego se
// hacen los swapeos necesarios para mantener la propiedad de heap, que en peor caso son proporcionales a la altura h = log(n).
// Luego, por las definiciones en el diccionario posicionPorElemento de cada iteración, la complejidad queda θ(log(n)^2).
// n = cantidad de elementos encolados.

template<class A, class B>
void colaPriorTupla<A,B>::desencolar() {
    eliminar(0);
}
// Complejidad: θ(log(n)^2)

template<class A, class B>
pair<A, B> colaPriorTupla<A, B>::proximo() const {
    return cola[0];
}
// Complejidad: θ(1)

template<class A, class B>
bool colaPriorTupla<A,B>::esVacia() const{
    return encolados == 0;
}
// Complejidad: θ(1)

template<class A, class B>
int colaPriorTupla<A,B>::tamano() const{
    return encolados;
}
// Complejidad: θ(1)

template<class A, class B>
bool colaPriorTupla<A,B>::encolado(const pair<A, B> &t){
    // find() de std::map devuelve un iterador al elemento en caso de encontrarlo y un iterador a end en caso contrario.
    auto buscar = posicionPorElemento.find(t); // θ(log(n))
    if(buscar == posicionPorElemento.end()){ // θ(1)
        return false;
    }else{
        return true;
    }
}
// Complejidad: θ(log(n)), con n la cantidad de elementos encolados.

template<class A, class B>
int colaPriorTupla<A, B>::posicionEnCola(const pair<A, B> &a) const{
    return posicionPorElemento.at(a); // θ(log(n))
}
// Complejidad: θ(log(n)), con n la cantidad de elementos encolados.

#endif //TP_LOLLA_COLAPRIORTUPLA_HPP
