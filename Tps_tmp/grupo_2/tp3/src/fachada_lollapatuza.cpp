#include "fachada_lollapatuza.h"


map<IdPuesto, Puesto> FachadaLollapatuza ::convertir(const map<IdPuesto, aed2_Puesto> &infoPuestos) {
    map<IdPuesto, Puesto> puestos_lolla;
    auto it = infoPuestos.begin();
    while(it != infoPuestos.end()){
        Menu m = it->second.menu;
        Promociones pr = it->second.promociones;
        Puesto p = Puesto(m, pr, it->second.stock);
        IdPuesto pId = it->first;
        puestos_lolla.insert(make_pair(pId, p));
        it++;
    }
    return puestos_lolla;
}


FachadaLollapatuza::FachadaLollapatuza(const set<Persona>& personas, const map<IdPuesto, aed2_Puesto> &infoPuestos){
    map<IdPuesto, Puesto> d = convertir(infoPuestos);
    set<Persona> p = personas;
    Lollapatuza* l = new Lollapatuza(d,p);
    lolla = l;
}

void FachadaLollapatuza::registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto) {
    lolla->venta(producto, persona, idPuesto, cant);
}

void FachadaLollapatuza::hackear(Persona persona, Producto producto) {
    lolla->hackear(producto,persona);
}

Nat FachadaLollapatuza::gastoTotal(Persona persona) const {
    return lolla->gastoDePersona(persona);
}

Persona FachadaLollapatuza::mayorGastador() const {
    return lolla->mayorGastador();
}

IdPuesto FachadaLollapatuza::menorStock(Producto producto) const {
   return lolla->puestoConMenorStock(producto);
}

const set<Persona> &FachadaLollapatuza::personas() const {
    return *lolla->personas();
}

Nat FachadaLollapatuza::stockEnPuesto(IdPuesto idPuesto, const Producto &producto) const {
    auto puesto = lolla->puestos()->at(idPuesto);
    return puesto.stock(producto);
}

Nat FachadaLollapatuza::descuentoEnPuesto(IdPuesto idPuesto, const Producto &producto, Nat cantidad) const {
    Puesto puesto = lolla->puestos()->at(idPuesto);
    return puesto.descuentoItem(producto, cantidad);
}

Nat FachadaLollapatuza::gastoEnPuesto(IdPuesto idPuesto, Persona persona) const {
    Puesto puesto = lolla->puestos()->at(idPuesto);
    return puesto.cuantoGasto(persona);
}

set<IdPuesto> FachadaLollapatuza::idsDePuestos() const {
    auto puestos = lolla->puestos();
    set<IdPuesto> res;
    for (const auto& puesto : *puestos) {
        res.insert(puesto.first);
    }
    return res;
}
