#include "fachada_lollapatuza.h"


FachadaLollapatuza::FachadaLollapatuza(const set<Persona> &personas, const map<IdPuesto, aed2_Puesto> &infoPuestos) :
        Lolla(Lollapatuza(cambiarPuestos(infoPuestos), personas)) {}

void FachadaLollapatuza::registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto) {
    Lolla.registrarCompra(producto, cant, persona, idPuesto);
}

void FachadaLollapatuza::hackear(Persona persona, Producto producto) {
    Lolla.hackear(producto, persona);
}

Nat FachadaLollapatuza::gastoTotal(Persona persona) const {
    return Lolla.obtenerGastoTotalDeUnaPersona(persona);
}

Persona FachadaLollapatuza::mayorGastador() const {
    return Lolla.personaQueMasGasto();
}

IdPuesto FachadaLollapatuza::menorStock(Producto producto) const {
    return Lolla.puestoConMenorStockDeItem(producto);
}

const set<Persona> &FachadaLollapatuza::personas() const {
    return Lolla.obtenerPersonas();
}

Nat FachadaLollapatuza::stockEnPuesto(IdPuesto idPuesto, const Producto &producto) const {
    map<IdPuesto, Puesto> puestos = Lolla.obtenerPuestos();
    Puesto checkearStock = puestos.at(idPuesto);
    return checkearStock.obtenerElStock(producto);
}

Nat FachadaLollapatuza::descuentoEnPuesto(IdPuesto idPuesto, const Producto &producto, Nat cantidad) const {
    map<IdPuesto, Puesto> puestos = Lolla.obtenerPuestos();
    Puesto checkearDto = puestos.at(idPuesto);
    return checkearDto.obtenerDescuento(producto, cantidad);
}

Nat FachadaLollapatuza::gastoEnPuesto(IdPuesto idPuesto, Persona persona) const {
    map<IdPuesto, Puesto> puestos = Lolla.obtenerPuestos();
    Puesto checkearGasto = puestos.at(idPuesto);
    return checkearGasto.obtenerGasto(persona);
}

set<IdPuesto> FachadaLollapatuza::idsDePuestos() const {
    return Lolla.ObtenerClavesPuesto();
}

map<IdPuesto, Puesto> FachadaLollapatuza::cambiarPuestos(const map<IdPuesto, aed2_Puesto> &infoPuestos) {
    map<IdPuesto, Puesto> puestos = map<IdPuesto, Puesto>();
    for (auto it = infoPuestos.begin(); it != infoPuestos.end(); it++) {
        puestos[it->first] = Puesto(it->second.menu,
                                    it->second.stock,
                                    it->second.promociones);
    }
    return puestos;
}