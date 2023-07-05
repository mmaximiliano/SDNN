//
// Created by Admin on 26/6/2023.
//

#ifndef TP_LOLLA_PUESTO_H
#define TP_LOLLA_PUESTO_H

#include "../tipos.h"
#include <set>
#include <list>

class Puesto{
public:
    Puesto(const Menu& menu, const Promociones& descuentos, const Stock& stocks);

    //aux para el destructor del lolla
    void borrarPuesto(){
        auto it = _items.begin();
        while(it != _items.end()){
            delete it->second.descuentos;
            it++;
        }
    }

    const Nat descuentoItem(Producto item, Nat cantidad);
    Nat cuantoGasto(Persona dni) const;
    Nat stock(Producto item) const;
    Nat precioItem(Producto item) const;

    void eliminarAPersona(Persona dni);

    void cambiarStock(Producto item, Nat nuevoStock);
    void cambiarGastoDePersona(Persona dni, Nat NuevoGasto);
    list<Producto> devolverItems();
private:
    struct info_item{
        Nat stock;
        Nat precio;
        map<Nat,Nat>* descuentos;

        info_item(Nat precio, Nat stock, map<Nat, Nat>* dtos): stock(stock), precio(precio), descuentos(dtos){}
    };

    map<Producto, info_item> _items;
    map<Persona , Nat> _clientes;
};

#endif //TP_LOLLA_PUESTO_H
