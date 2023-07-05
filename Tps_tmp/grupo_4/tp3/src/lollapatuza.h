//
// Created by magal on 6/25/2023.
//
#include <set>
#include "tipos.h"
#include "puesto.h"
#include "ConjuntoLog.hpp"
#ifndef TP_LOLLA_LOLLAPATUZA_H
#define TP_LOLLA_LOLLAPATUZA_H

#endif //TP_LOLLA_LOLLAPATUZA_H

class Lollapatuza {

    public:
        Lollapatuza();

        Lollapatuza(const set<Persona>& personas, const map<IdPuesto, Puesto>& infoPuestos);

        //~Lollapatuza();

        void registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto);

        void hackear(Persona persona, Producto producto);

        Nat gastoTotal(Persona persona) const;

        Persona mayorGastador() const;

        IdPuesto menorStock(Producto producto) const;

        const set<Persona>& personas() const;

        const map<IdPuesto, Puesto>& puestos() const;

    private:


        map<IdPuesto, Puesto> _puestos;

        set<Persona> _personasSet;

        ConjLog<Persona> _personas;

        map<IdPuesto, map<IdPuesto, Puesto>::iterator> _puestosXiteradores;

        map<Persona, Nat> _gastosXpersona;

        struct personaYgasto{
            Nat gasto;
            Persona persona;

            personaYgasto(){};

            personaYgasto(const pair<Nat, Persona>& t){
                gasto = t.first;
                persona = t.second;
            }

            personaYgasto(const Nat gasto, const Persona& persona){
                this->gasto = gasto;
                this->persona = persona;
            }

            bool operator<(const personaYgasto& otro) const {
                return this->gasto < otro.gasto || (this->gasto == otro.gasto && this->persona < otro.persona);
            }
        };

        personaYgasto _personaQueMasGasto;

        ConjLog<personaYgasto> _gastosOrdenados;

        struct idPuestoYiterador {
            IdPuesto id;
            map<IdPuesto, Puesto>::iterator it;

            idPuestoYiterador(){};

            idPuestoYiterador(pair<IdPuesto, map<IdPuesto, Puesto>::iterator> p){
                id = p.first;
                it = p.second;
            }

            idPuestoYiterador(const IdPuesto& idPuesto, const map<IdPuesto, Puesto>::iterator& iterador){
                id = idPuesto;
                it = iterador;
            }

            bool operator<(const idPuestoYiterador& otro) const {
                return this->id < otro.id;
            }
        };

        map<Persona, map<Producto, idPuestoYiterador>> _proximoHackeo;

        map<Persona, map<Producto, ConjLog<idPuestoYiterador>>> _puestosOrdenados;
};