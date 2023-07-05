//
// Created by magal on 6/25/2023.
//

#include "tipos.h"
#include "vector"
#include "list"
#ifndef TP_LOLLA_PUESTO_H
#define TP_LOLLA_PUESTO_H

#endif //TP_LOLLA_PUESTO_H

using namespace std;

class Puesto{
    public:

        Puesto();

        Puesto(map<Producto, Nat> precios, map<Producto, Nat> stocks, map<Producto, vector<pair<Nat, Nat>>> descuentos);

        //~Puesto();

        Nat obtenerStock(Producto i);

        Nat obtenerDescuento(Producto i, Nat cant);

        Nat obtenerGasto(Persona a);

        Nat obtenerPrecio(Producto i);

        void vender(Persona a, Producto i, Nat cant);

        bool tieneDescuento(Producto i, Nat cant);

        Nat calcularPrecio(Producto i, Nat cant);

        void anularCompra(Persona a, Producto i);

        bool comproItemSinDescuento(Persona a, Producto i);

        //Puesto& operator=(const Puesto& otro);

    private:
        struct descuento{
            Nat cantidad;
            Nat porcentaje;

            descuento(pair<Nat, Nat> t){
                cantidad = t.first;
                porcentaje = t.second;
            }

            bool operator<(const descuento& otro) const {
                return this->cantidad < otro.cantidad;
            }
        };

        map<Producto, Nat> _stocks;
        map<Producto, Nat> _precios;
        map<Producto, vector<descuento>> _descuentos;
        map<Persona, Nat> _gastosTotales;
        map<Persona, map<Producto, list<Nat>[2]>> _gastos;
};