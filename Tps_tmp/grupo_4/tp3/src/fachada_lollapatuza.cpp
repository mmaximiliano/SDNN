#include "fachada_lollapatuza.h"


FachadaLollapatuza::FachadaLollapatuza(const set<Persona> &personas, const map<IdPuesto, aed2_Puesto> &infoPuestos) {
    map<IdPuesto, Puesto> infoPuestosParametro;

    for(pair<IdPuesto, aed2_Puesto> tupla: infoPuestos){
        map<Producto, Nat> precios = tupla.second.menu;
        map<Producto, Nat> stocks = tupla.second.stock;
        map<Producto, vector<pair<Nat, Nat>>> descuentos;

        for(pair<Producto, map<Nat, Nat>> promos: tupla.second.promociones){
            vector<pair<Nat, Nat>> descuentosDelProducto = vector<pair<Nat, Nat>>();
            for(pair<Nat,Nat> promo: promos.second){
                descuentosDelProducto.push_back(promo);
            }
            descuentos[promos.first] = descuentosDelProducto;
        }

        Puesto puesto = Puesto(precios, stocks, descuentos);
        infoPuestosParametro[tupla.first] = puesto;
    }

    _lolla = Lollapatuza(personas, infoPuestosParametro);
}

void FachadaLollapatuza::registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto) {
    _lolla.registrarCompra(persona, producto, cant, idPuesto);
}

void FachadaLollapatuza::hackear(Persona persona, Producto producto) {
    _lolla.hackear(persona, producto);
}

Nat FachadaLollapatuza::gastoTotal(Persona persona) const {
    return _lolla.gastoTotal(persona);
}

Persona FachadaLollapatuza::mayorGastador() const {
    return _lolla.mayorGastador();
}

IdPuesto FachadaLollapatuza::menorStock(Producto producto) const {
    return _lolla.menorStock(producto);
}

const set<Persona> &FachadaLollapatuza::personas() const {
    return _lolla.personas();
}

Nat FachadaLollapatuza::stockEnPuesto(IdPuesto idPuesto, const Producto &producto) const {
    Puesto puesto = _lolla.puestos().at(idPuesto);
    return puesto.obtenerStock(producto);
}

Nat FachadaLollapatuza::descuentoEnPuesto(IdPuesto idPuesto, const Producto &producto, Nat cantidad) const {
    Puesto puesto = _lolla.puestos().at(idPuesto);
    return puesto.obtenerDescuento(producto, cantidad);
}

Nat FachadaLollapatuza::gastoEnPuesto(IdPuesto idPuesto, Persona persona) const {
    Puesto puesto = _lolla.puestos().at(idPuesto);
    return puesto.obtenerGasto(persona);
}

set<IdPuesto> FachadaLollapatuza::idsDePuestos() const {
    set<IdPuesto> res = set<IdPuesto>();

    map<IdPuesto, Puesto> puestos = _lolla.puestos();
    for(pair<IdPuesto, Puesto> tupla: puestos){
        res.insert(tupla.first);
    }

    return res;
}