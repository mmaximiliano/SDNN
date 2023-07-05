//
// Created by Admin on 26/6/2023.
//

#ifndef TP_LOLLA_LOLLAPATUZA_H
#define TP_LOLLA_LOLLAPATUZA_H

#include "../tipos.h"
#include <set>
#include <utility>
#include "puesto.h"
#include <list>

class Lollapatuza {
public:
    Lollapatuza(map<IdPuesto, Puesto>& puestos, set<Persona> dnis);

    ~Lollapatuza(){
        delete _conjPersonas;
        auto it = _puestos.begin();
        while(it != _puestos.end()){
            it->second.borrarPuesto();
            it++;
        }
    }

    void venta(Producto item, Persona dni, IdPuesto id, Nat stock);

    void hackear(Producto item, Persona dni);

    Nat gastoDePersona(Persona dni) const;
    IdPuesto puestoConMenorStock(Producto item) const;

    Persona mayorGastador() const;
    set<Persona>* personas() const;

    const map<IdPuesto, Puesto>* puestos() const;
private:

    using itGastos = map<Nat,set<Persona>>::iterator;

    struct Max {
        itGastos it;
        Persona id;
    };

    using itPuestos = map<IdPuesto,Puesto>::iterator;

    struct Hack{
        itPuestos it_puesto;
        Nat cantHackeable;

        Hack(itPuestos it,Nat cant): it_puesto(it), cantHackeable(cant){}
    };

    using itpuestosAHackear = map<IdPuesto,Hack>::iterator;

    struct nodoCompras{
        itpuestosAHackear it_puestosAHackear;
        map<IdPuesto,Hack> puestosAHackear;

        nodoCompras(IdPuesto clave, Hack significado){
            map<IdPuesto,Hack> dicc;
            puestosAHackear = dicc;
            auto it = puestosAHackear.insert(make_pair(clave, significado));
            it_puestosAHackear = it.first;
        }
    };

    struct info_persona{
        Nat gastoTotal;
        map<Producto, nodoCompras> compras;
    };

    //Variables privadas
    map<Persona, info_persona> _personas;
    set<Persona>* _conjPersonas;
    map<IdPuesto, Puesto> _puestos;
    Max _maxGastador;
    map<Nat, set<Persona>> _gastos;
    map<Producto, map<IdPuesto, itPuestos>> _items;

    void AgregarPersonas(set<Persona>& dnis);

    void AgregarPuestos(map<IdPuesto, Puesto>& puestos);

    void actualizarGastosVenta(Persona dni, Nat gastado, itPuestos itPuestoVenta);

    void actulizarHackeableVenta(info_persona& persona, Producto item, IdPuesto idPuesto, Nat cantidad, itPuestos itPuestoVenta);

    map<Nat, set<Persona>>::iterator hackearGastosyStock(info_persona &persona, Producto item, Persona dni, itPuestos iterMap);

    void hackearMaximoGastador(map<Nat, set<Persona>>::iterator nodoGastoPrevio);

};
#endif //TP_LOLLA_LOLLAPATUZA_H
