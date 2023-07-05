#ifndef TP_LOLLA_LOLLAPATUZA_H
#define TP_LOLLA_LOLLAPATUZA_H
#include "Puesto.h"
#include <set>
#include "colaPriorTupla.hpp"

using Personas = set<Persona>;
using Puestos = map<IdPuesto, aed2_Puesto>;

class Lollapatuza {
    public:
    /*
     * CONSTRUCTOR
     * Dado un conjunto de personas y un diccionario <ID, PUESTO>, inicializa el sistema de un nuevo lollapatuza.
     * Pre: pers y pues son conjuntos no vacíos. Los puestos no tienen ventas registradas y si dos puestos venden el mismo producto, ambos lo hacen al mismo precio.
     * Complejidad: θ(∑p∈pues(crearPuesto(p)) + ∑i=1to#pers(log(i) + log(i)^2))
     */
    Lollapatuza(const Personas& pers, const Puestos& pues);


    /*
     * REGISTRARCOMPRA
     * Registra en el sistema la compra de una cierta cantidad de un producto en un puesto (identificado por id) hecha por alguna persona.
     * Pre: El ID del puesto y la persona corresponden a ID's registrados en el sistema. El puesto vende el producto y hay suficiente stock para que se concrete una venta.
     * Complejidad: Complejidad: θ(log(P) + log(I) + log(A)^2). Con A = cantidad de personas del sistema, P = cantidad de puestos del sistema e I = cantidad de productos en el sistema.
     */
    void registrarCompra(Persona per, IdPuesto id, Producto item, Nat cant);

    /*
     * HACKEAR
     * Hackea la compra de una unidad (comprada sin promoción) del producto realizada por la persona del puesto de menor ID donde hizo una compra del producto sin descuento.
     * Con hackear se quiere decir que se elimina todo registro de la transacción en el sistema, tanto para la persona como el puesto.
     * Es decir, la persona nunca compró esa unidad del ítem en el puesto y el puesto nunca la vendió.
     * Pre: La persona está registrada en el sistema y existe un puesto registrado donde la persona efectúo una compra sin descuento del producto.
     * Complejidad: θ(log(P) + log(A) + log(I) + log(A)^2). Con A = cantidad de personas del sistema, P = cantidad de puestos del sistema e I = cantidad de productos en el sistema.
     */
    void Hackear(Persona per, Producto item);

    /*
     * CUANTOGASTO
     * Devuelve el gasto registrado de la persona.
     * Pre: La persona está registrada en el sistema.
     * Complejidad: θ(log(A)), con A = cantidad de personas del sistema.
     */
    Nat cuantoGasto(Persona per) const;

    /*
     * LAPERSONAQUEMASGASTO
     * Devuelve la persona que tiene registrado el mayor gasto en el sistema.  Si hay más de una persona que gastó el monto máximo, desempata por la persona de 'menor ID'.
     * Complejidad: θ(1)
     */
    Persona laPersonaQueMasGasto() const;

    /*
     * PUESTOCONMENORSTOCK
     * Devuelve el ID del puesto que tiene la menor cantidad de stock disponible del producto.
     * En caso de empate se devuelve el puesto de menor ID (aplica también en el caso que ningún puesto venda el producto).
     * Complejidad: θ(P * log(I)), con P = cantidad de puestos del sistema e I = cantidad de productos en el sistema.
     *
     */
    IdPuesto puestoConMenorStock(Producto item) const;

    /*
     * PERSONASPARTICIPANTES
     * Devuelve una referencia no modificable al conjunto de las personas registradas en el sistema como participantes del festival.
     * Complejidad: θ(1)
     */
    const set<Persona>& personasParticipantes() const;

    /*
     * PUESTOSCONIDS
     * Devuelve una referencia no modificable a un diccionario tal que los pares clave-significado son <id, puesto>.
     * Complejidad: θ(1)
     */
    const map<IdPuesto, Puesto>& puestosConIDS() const;

    /*
     * COMPROPRODUCTOSINDESCUENTO
     * Devuelve true si y sólo si la persona realizó una compra sin descuento en alguno de los puestos del sistema. Si la persona no está registrada en el sistema, devolverá false.
     * Complejidad: θ(P * (log(A) + log(I))), con P = cantidad de puestos del sistema, A = cantidad de personas del sistema e I = cantidad de productos en el sistema.
     */
    bool comproProductoSinDescuento(Persona per, Producto item) const;

    /*
     * LOVENDEALGUNPUESTOCONSTOCK
     * Devuelve true si y solo si el producto está en el menú de alguno de los puestos del festival.
     * En el caso que haya alguno, en ids se devuelve una lista de pares <id, stock> de los puestos que lo venden, caso contrario se devuelve una lista vacía.
     * Complejidad: θ(P * log(I)), con P = cantidad de puestos del sistema e I = cantidad de productos en el sistema.
     */
    bool loVendeAlgunPuestoConStock(Producto item, list<pair<IdPuesto, Nat>> &ids) const;

    /*
     * STOCKPRODUCTOPUESTO
     * Devuelve el stock disponible del producto en el puesto identificado por el ID.
     * Pre: El ID del puesto está registrado en el sistema y el puesto vende el producto.
     * Complejidad: θ(log(P) + log(I)), con P = cantidad de puestos del sistema e I = cantidad de productos en el sistema.
     */
    Nat stockProductoPuesto(Producto item, IdPuesto id) const;


    private:
        map<IdPuesto, Puesto> puestos;
        set<Persona> personas;
        colaPriorTupla<Nat, Persona> colaGastos;
        map<Persona, Nat> gastoPorPersona;
        map<Persona, map<Producto, map<IdPuesto, Puesto*>>> IDpuestosDondeComproSinDescuento;
        Nat calcularDescuento(Nat precio, Nat cant, Nat desc){
            return ((precio * cant * (100 - desc)) / 100);
        }

};


#endif //TP_LOLLA_LOLLAPATUZA_H
