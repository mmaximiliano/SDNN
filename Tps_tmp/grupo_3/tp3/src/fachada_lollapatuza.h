
//Grupo Chip
//Integrante                LU      Correo electrónico
//Beren, Manuel Andrés      320/22  m.beren83@gmail.com
//Carrillo, Mariano         358/22  carr.mariano@gmail.com
//Roitman, Sofía            563/21  sofiroit@gmail.com
//Valentini, Justo Agustin  566/22  justovalentini@gmail.com
#ifndef FACHADA_LOLLAPATUZA_H
#define FACHADA_LOLLAPATUZA_H

#include <set>
#include "tipos.h"
#include "Lolla.h"


class FachadaLollapatuza {
public:
    FachadaLollapatuza(const set<Persona>& personas, const map<IdPuesto, aed2_Puesto>& infoPuestos);

    void registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto);
    void hackear(Persona persona, Producto producto);

    Nat gastoTotal(Persona persona) const;
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


        Lolla _lolla;
        map<int, Puesto> cambioPuestos(const map<IdPuesto, aed2_Puesto> &infoPuestos);
        //Hicimos esta funcion auxiliar porque, si no, no tipaba el constructor.


};

#endif