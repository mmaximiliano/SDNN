#ifndef TP_LOLLA_PUESTO_H
#define TP_LOLLA_PUESTO_H
#include "tipos.h"
#include <vector>
#include <queue>
#include <list>

class Puesto {
    public:
    /*
     * CONSTRUCTOR
     * Construye un puesto a partir de su stock, menú y descuentos.
     * Pre: Los productos del stock son los mismos que los del menú, y el conjunto de los productos con promociones está incluido en el de los productos del stock/menú.
     * Complejidad: θ(I * (log(I) + D' * D)), donde I = #claves(m) = #claves(s), D = máxima cantidad de descuentos que tiene un ítem y D' = máxima cantidad mínima requerida para aplicar un descuento que tiene un ítem.
     */
    Puesto(Stock s, Menu m, Promociones p);



    /*
     *  VENDER
     *  Realiza una venta de una cierta cantidad de un producto a una persona.
     *  Pre: El puesto vende el producto y hay suficiente stock.
     *  Complejidad: θ(log(A) + log(I))
     *  A = cantidad de personas que realizaron alguna compra en el puesto.
     *  I = cantidad de productos que vende el puesto.
     */
    void Vender(Persona per, Producto item, Nat cant);

    /*
     *  PORCENTAJEDESCUENTO
     *  Devuelve que porcentaje de descuento ofrece el puesto para una cierta cantidad de un producto.
     *  Pre: El puesto vende el producto.
     *  Complejidad: θ(log(I))
     *  I = cantidad de productos que vende el puesto.
     */
    Nat porcentajeDescuento(Producto item, Nat cant) const;

    /*
     * ENMENU
     * Devuelve true si y sólo si el puesto tiene al producto en su menú.
     * Complejidad: θ(log(I))
     * I = cantidad de productos que vende el puesto.
     */
    bool enMenu(Producto item) const;

    /*
     * STOCKPRODUCTO
     * Devuelve el stock disponible del producto en el puesto.
     * Pre: El puesto vende el producto.
     * Complejidad: θ(log(I))
     * I = cantidad de productos que vende el puesto.
     */
    Nat stockProducto(Producto item) const;

    /*
     * PRECIO
     * Devuelve el valor de una unidad del producto.
     * Pre: El puesto vende el producto.
     * Complejidad: θ(log(I))
     * I = cantidad de productos que vende el puesto.
     */
    Nat Precio(Producto item) const;

    /*
     * CUANTOGASTOPERSONA
     * Dada una persona, devuelve su gasto en el puesto.
     * Complejidad: θ(log(A))
     * A = cantidad de personas que realizaron alguna compra en el puesto.
     */
    Nat cuantoGastoPersona(Persona per) const;

    /*
     * COMPROITEMSINDESCUENTO
     * Dada una persona y un producto, devuelve true si y solo si existe registro de una compra sin descuento del producto hecha por la persona.
     * Pre: El puesto vende el producto.
     * Complejidad: θ(log(A) + log(I))
     * A = cantidad de personas que realizaron alguna compra en el puesto.
     * I = cantidad de productos que vende el puesto.
     */
    bool comproItemSinDescuento(Persona per, Producto item) const;

    /*
     * OLVIDARCOMPRASINDESCUENTO
     * Elimina el registro de la compra de una unidad del producto (aunque haya sido una compra de más de una unidad, tan solo se 'olvida' una) hecha por la persona en la cual no se le aplicó un descuento.
     * Pre: El puesto vende el producto y la persona hizo una compra sin descuento del producto en el puesto.
     * Complejidad: θ(log(A) + log(I))
     * A = cantidad de personas que realizaron alguna compra en el puesto.
     * I = cantidad de productos que vende el puesto.
     */
    void olvidarCompraSinDescuento(Persona per, Producto item);


    private:
        Menu menu;
        Stock stock;
        // Para cada producto con descuento, en la posición i del vector está el porcentaje de descuento aplicable a una compra de i unidades o más.
        // Con esto, al momento de querer saber un descuento para una cantidad k, si k es una posición del vector v simplemente se accede a v[k], y si k llega
        // a ser mayor que la longitud del vector, entonces el descuento que aplica es el de mayor cantidad mínima, es decir, v[|v| - 1].
        map<Producto, vector<Nat>> descuentos;
        map<Persona, Nat> gastoPorPersona;
        map<Persona, map<Producto, list<Nat>>> comprasPorPersona;
        map<Persona, map<Producto, queue<list<Nat>::iterator>>> comprasSinDescuentoPorPersona;
        Nat calcularDescuento(Nat precio, Nat cant, Nat desc){
            return ((precio * cant * (100 - desc)) / 100);
        }


};

#endif //TP_LOLLA_PUESTO_H
