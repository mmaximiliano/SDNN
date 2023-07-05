#include "fachada_lollapatuza.h"


FachadaLollapatuza::FachadaLollapatuza(const set<Persona> &personas, const map<IdPuesto, aed2_Puesto> &infoPuestos): lolla(Lollapatuza(personas, infoPuestos)) {};

void FachadaLollapatuza::registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto) {
    lolla.registrarCompra(persona, idPuesto, producto, cant);
}

void FachadaLollapatuza::hackear(Persona persona, Producto producto) {
    lolla.Hackear(persona, producto);
}

Nat FachadaLollapatuza::gastoTotal(Persona persona) const {
    return lolla.cuantoGasto(persona);
}

Persona FachadaLollapatuza::mayorGastador() const {
    return lolla.laPersonaQueMasGasto();
}

IdPuesto FachadaLollapatuza::menorStock(Producto producto) const {
   return lolla.puestoConMenorStock(producto);
}

const set<Persona> &FachadaLollapatuza::personas() const {
    return lolla.personasParticipantes();
}

Nat FachadaLollapatuza::stockEnPuesto(IdPuesto idPuesto, const Producto &producto) const {
    return lolla.stockProductoPuesto(producto, idPuesto);
}

Nat FachadaLollapatuza::descuentoEnPuesto(IdPuesto idPuesto, const Producto &producto, Nat cantidad) const {
    auto puestos = lolla.puestosConIDS();
    return puestos.at(idPuesto).porcentajeDescuento(producto, cantidad);
}

Nat FachadaLollapatuza::gastoEnPuesto(IdPuesto idPuesto, Persona persona) const {
    auto puestos = lolla.puestosConIDS();
    return puestos.at(idPuesto).cuantoGastoPersona(persona);
}

set<IdPuesto> FachadaLollapatuza::idsDePuestos() const {
    auto puestos = lolla.puestosConIDS();
    set<IdPuesto> ids;
    for(auto it = puestos.begin(); it != puestos.end(); ++it){
        ids.insert(it->first);
    }
    return ids;
}