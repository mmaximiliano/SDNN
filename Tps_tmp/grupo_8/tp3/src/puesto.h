#ifndef TP_LOLLA_PUESTO_H
#define TP_LOLLA_PUESTO_H

#include "tipos.h"

class Puesto {
    public:
        Puesto(const Menu& p, const Stock& s, const Promociones& d);

        Nat devolverStock(Producto i);

        Nat descuento(Producto i , Nat c);

        Nat gasto(Persona a);

        Nat vender(Persona a, Producto i, Nat c, bool &huboDesc);

        Nat olvidar(Persona a, Producto i, bool &noHack);

        //Este solo existe para que map[] no se queje. No hay que usarlo.
        Puesto();

        //No hace falta destructor porque no hicimos nada con memoria
        //Tampoco hace falta constructor "=" o "Lollapatuza(Lollapatuza l)" porque los que vienen por defecto con c++ copian las
        //variables de private y en nuestro caso eso es suficiente.

    private:
        Stock _stock;
        map<Producto, tuple<vector<Nat>, vector<Nat>>> _descuentos;                             //0 es descuentos, 1 es cortes
        map<Persona, Nat> _gastoPorPersona;
        Menu _menu;
        map<Persona, list<tuple<Producto,Nat>>> _ventas;                                        //0 es item, 1 es cantidad
        map<Persona, map<Producto, list<list<tuple<Producto, Nat>>::iterator>>> _ventasSinDescuento;
};

#endif //TP_LOLLA_PUESTO_H
