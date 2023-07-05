#ifndef FACHADA_LOLLAPATUZA_H
#define FACHADA_LOLLAPATUZA_H

#include <set>
#include <map>
#include <vector>
#include "tipos.h"
#include "lollapatuza.h"
#include "puesto.h"
#include "colaprior.h"

class FachadaLollapatuza {
    public:
        //constructor
        FachadaLollapatuza(const set<Persona>& personas, const map<IdPuesto, aed2_Puesto>& infoPuestos);

        //registra una compra dada una persona un producto una cantidad y un puesto
        void registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto);

        //elimina una unidad de una compra de una persona y puesto dado un producto.
        void hackear(Persona persona, Producto producto);

        //gasto total de una persona
        Nat gastoTotal(Persona persona) const;

        //la persona que mas gasto
        Persona mayorGastador() const;


        IdPuesto menorStock(Producto producto) const;

        const set<Persona>& personas() const;

        // Operaciones para obtener información de los puestos
        Nat stockEnPuesto(IdPuesto idPuesto, const Producto& producto) const;
        Nat descuentoEnPuesto(IdPuesto idPuesto, const Producto& producto, Nat cantidad) const;
        Nat gastoEnPuesto(IdPuesto idPuesto, Persona persona) const;

        /**
         * Devuelve el conjunto de los los ids de todos los puestos;
        */
        set<IdPuesto> idsDePuestos() const;

    private:
        Lollapatuza _lolla;
        set<Persona> _personas; //las personas
        map<IdPuesto, aed2_Puesto> _infoPuestos; //un diccionario con clave puestoID significado puesto.
};

#endif
