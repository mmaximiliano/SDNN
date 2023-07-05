#include "fachada_lollapatuza.h"


FachadaLollapatuza::FachadaLollapatuza(const set<Persona> &personas, const map<IdPuesto, aed2_Puesto> &infoPuestos) : _personas(personas), _infoPuestos(infoPuestos) {
    map<IdPuesto, Puesto> puestos;
    for (auto const& p : _infoPuestos) {
        puestos[p.first] = Puesto(p.second.menu, p.second.stock, p.second.promociones);
    }
    _lolla = Lollapatuza(puestos, personas);
}

void FachadaLollapatuza::registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto) {
    _lolla.transaccion(persona, idPuesto, producto, cant);
}

void FachadaLollapatuza::hackear(Persona persona, Producto producto) {
    _lolla.hackear(producto, persona);
}

Nat FachadaLollapatuza::gastoTotal(Persona persona) const {
    return _lolla.gastoPersona(persona);
}

Persona FachadaLollapatuza::mayorGastador() const {
    return _lolla.personaQueMasGasto();
}

IdPuesto FachadaLollapatuza::menorStock(Producto producto) const {
    bool hayStock = true;
    return _lolla.menorStock(producto, hayStock);
}

const set<Persona> &FachadaLollapatuza::personas() const {
    return _lolla.personas();
}

Nat FachadaLollapatuza::stockEnPuesto(IdPuesto idPuesto, const Producto &producto) const {
    map<IdPuesto, Puesto> puestos = _lolla.puestos();
    return puestos[idPuesto].devolverStock(producto);
}

Nat FachadaLollapatuza::descuentoEnPuesto(IdPuesto idPuesto, const Producto &producto, Nat cantidad) const {
    map<IdPuesto, Puesto> puestos = _lolla.puestos();
    return puestos[idPuesto].descuento(producto, cantidad);
}

Nat FachadaLollapatuza::gastoEnPuesto(IdPuesto idPuesto, Persona persona) const {
    map<IdPuesto, Puesto> puestos = _lolla.puestos();
    return puestos[idPuesto].gasto(persona);
}

set<IdPuesto> FachadaLollapatuza::idsDePuestos() const {
    set<IdPuesto> ids;
    for(auto &par : _lolla.puestos()){
        ids.insert(par.first);
    }
    return ids;
}