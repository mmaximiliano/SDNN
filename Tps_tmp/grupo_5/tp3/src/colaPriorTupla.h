#ifndef TP_LOLLA_COLAPRIORTUPLA_H
#define TP_LOLLA_COLAPRIORTUPLA_H

#include <vector>
#include <map>
using namespace std;

template<class A, class B>
class colaPriorTupla{
public:
    /**
     * CONSTRUCTOR CON CLAVE DE COMPARACION
     * Crea una cola vacía a partir de la clave de comparación de los elementos.
     * Pre: La función de comparación debe devolver cuando la tupla x tiene mayor prioridad que y.
     */
    colaPriorTupla(bool compare(pair<A, B> x, pair<A, B> y));

    /**
     * esVacia
     * Devuelve true si la cola está vacía, caso contrario devuelve false.
     * Complejidad: θ(1)
     */
    bool esVacia() const;

    /**
     * ENCOLAR
     * Encola la tupla t.
     * Complejidad: θ(log(n)^2), con n la cantidad de elementos encolados.
     */
    void encolar(const pair<A, B> &t);

    /**
     * DESENCOLAR
     * Pre: la cola no es vacía
     * Desencola el próximo de la cola (el elemento con mayor prioridad).
     * Complejidad: θ(log(n)^2), con n la cantidad de elementos encolados.
     */
    void desencolar();

    /**
     * ENCOLADO
     * Devuelve true si t está en la cola, caso contrario devuelve false.
     * Complejidad: θ(log(n)), con n la cantidad de elementos encolados.
     */
    bool encolado(const pair<A,B> &t);

    /**
     * POSICIONELEMENTO
     * Devuelve en que posición de la cola esta el par t.
     * Pre: t está en la cola
     * Complejidad: θ(log(n)), con n la cantidad de elementos encolados.
     */
    int posicionEnCola(const pair<A, B> &t) const;

    /**
     * TAMAÑO
     * Devuelve la cantidad de elementos que tiene la cola.
     * Complejidad: θ(1)
     */
    int tamano() const;

    /**
     * ELIMINAR
     * Borra de la cola al elemento que se encuentra en la posición i.
     * Pre: i < tamaño de la cola
     * Complejidad: θ(log(n)^2), con n la cantidad de elementos encolados.
     */
    void eliminar(int i);

    /**
     * PROXIMO
     * Devuelve el próximo en la cola (el elemento de mayor prioridad de la cola).
     * Pre: la cola no es vacía
     * Complejidad: θ(1)
     */
    pair<A,B> proximo() const;

private:
    vector<pair<A, B>> cola;
    map<pair<A, B>, int> posicionPorElemento;
    bool (*cmp)(pair<A, B>, pair<A, B>);
    int encolados;

};


#endif //TP_LOLLA_COLAPRIORTUPLA_H
