#ifndef FACHADA_LOLLAPATUZA_H
#define FACHADA_LOLLAPATUZA_H

#include <set>
#include "tipos.h"
#include "solucion/lollapatuza.h"


class FachadaLollapatuza {
public:
    FachadaLollapatuza(const set<Persona>& personas, const map<IdPuesto, aed2_Puesto>& infoPuestos);

    ~FachadaLollapatuza(){
        delete lolla;
    }

    void registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto);
    void hackear(Persona persona, Producto producto);

    Nat gastoTotal(Persona persona) const;
    Persona mayorGastador() const;
    IdPuesto menorStock(Producto producto) const;

    const set<Persona>& personas() const;

/*****Cuando se use el generador de puesto, nos tenemos que acordar de hacer un "new" para los parametros a pasar*****/

    // Operaciones para obtener información de los puestos
    Nat stockEnPuesto(IdPuesto idPuesto, const Producto& producto) const;
    Nat descuentoEnPuesto(IdPuesto idPuesto, const Producto& producto, Nat cantidad) const;
    Nat gastoEnPuesto(IdPuesto idPuesto, Persona persona) const;

    /**
     * Devuelve el conjunto de los los ids de todos los puestos;
    */
    set<IdPuesto> idsDePuestos() const;

private:
    /*  TODO: Completar.

        En la parte privada deben guardarse una instancia de su  modulo Lollapatuza.
        Por ejemplo:

        Lollapatuza lolla;

    */
    Lollapatuza* lolla;

    map<IdPuesto, Puesto> convertir(const map<IdPuesto, aed2_Puesto>& infoPuestos);
};

#endif
