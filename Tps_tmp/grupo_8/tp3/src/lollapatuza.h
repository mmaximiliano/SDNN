#ifndef TP_LOLLA_LOLLAPATUZA_H
#define TP_LOLLA_LOLLAPATUZA_H

#include <set>
#include <map>
#include <vector>
#include <queue>
#include "tipos.h"
#include "puesto.h"
#include "colaprior.h"
#include <tuple>

class Lollapatuza {
    public:
        //constructor
        Lollapatuza(const map<IdPuesto, Puesto>& p, const set<Persona>& a);

        //registra una compra dada una persona un producto una cantidad y un puesto
        void transaccion(Persona a, IdPuesto id, Producto i, Nat cant);

        //elimina una unidad de una compra de una persona y puesto dado un producto.
        bool hackear(Producto i, Persona a);

        //gasto total de una persona
        Nat gastoPersona(Persona a) const;

        //la persona que mas gasto
        Persona personaQueMasGasto() const;

        IdPuesto menorStock(Producto i, bool& x) const;

        const set<Persona>& personas() const;

        const map<IdPuesto, Puesto>& puestos() const;

        //Este solo esta para que c++ no se queje, no usar
        Lollapatuza();

        //No hace falta destructor porque no hicimos nada con memoria
        //Tampoco hace falta constructor "=" o "Lollapatuza(Lollapatuza l)" porque los que vienen por defecto con c++ copian las
        //variables de private y en nuestro caso eso es suficiente.

    private:
        struct PuestoIdIt {
            IdPuesto id;
            map<IdPuesto, Puesto>::iterator it;
            PuestoIdIt(IdPuesto i, map<IdPuesto, Puesto>::iterator t) : id(i), it(t) {}
            bool operator<(const PuestoIdIt& a) const {
                return id < a.id;
            }
        };

        struct PerGasto {
            Persona id;
            Nat gasto;
            PerGasto(Persona i, Nat g) : id(i), gasto(g) {}
            bool operator<(const PerGasto& a) const {
                return gasto > a.gasto || (gasto == a.gasto && id < a.id);
            }
        };

        map<IdPuesto, Puesto> _puestos;
        map<IdPuesto, map<IdPuesto,Puesto>::iterator> _puestosIts;
        set<Persona> _personas;
        map<Persona, Nat> _gastoPersona;
        ColaPrior<PerGasto> _masGastaron;
        vector<int> _ordenGasto;
        map<Persona, map<Producto, tuple<ColaPrior<PuestoIdIt>, map<IdPuesto, bool>>>> _comprasHackeables;
        //0 cola, 1 enCola
};

#endif //TP_LOLLA_LOLLAPATUZA_H
