//Grupo Chip
//Integrante                LU      Correo electrónico
//Beren, Manuel Andrés      320/22  m.beren83@gmail.com
//Carrillo, Mariano         358/22  carr.mariano@gmail.com
//Roitman, Sofía            563/21  sofiroit@gmail.com
//Valentini, Justo Agustin  566/22  justovalentini@gmail.com

#include "fachada_lollapatuza.h"

map<int, Puesto> FachadaLollapatuza::cambioPuestos(const map<IdPuesto, aed2_Puesto> &infoPuestos){
    map<int, Puesto> puestos;
    for(pair<IdPuesto, aed2_Puesto> puesto: infoPuestos) {
        map<int, int> menu;
        map<int, int> stock;
        map<int, map<int, int>> desc;
        for (pair<int, unsigned int> m: puesto.second.menu) {
            menu.insert(m);
        }
        for (pair<int, unsigned int> s: puesto.second.stock) {
            stock.insert(s);
        }
        for (pair<const int, map<unsigned int, unsigned int>> producto: puesto.second.promociones) {
            map<int, int> promos;
            for (pair<unsigned int, unsigned int> cantidad: producto.second) {
                promos.insert(make_pair(cantidad.first, cantidad.second));
            }
            desc.insert(make_pair(producto.first, promos));
        }
        puestos.insert(make_pair(puesto.first, Puesto(menu, stock, desc, puesto.first)));
    }
    return puestos;
}


FachadaLollapatuza::FachadaLollapatuza(const set<Persona> &personas, const map<IdPuesto, aed2_Puesto> &infoPuestos) :
_lolla(cambioPuestos(infoPuestos), personas){}

void FachadaLollapatuza::registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto) {
    _lolla.comprar(_lolla.infoPuestos().at(idPuesto), persona, producto, cant);
}

void FachadaLollapatuza::hackear(Persona persona, Producto producto) {
    _lolla.hackear(persona, producto);
}

Nat FachadaLollapatuza::gastoTotal(Persona persona) const {
    Lolla lollaCopia = _lolla;
    int res = lollaCopia.gastoTotal(persona);
    return res;
}

Persona FachadaLollapatuza::mayorGastador() const {
    return _lolla.maxConsumidor();
}

IdPuesto FachadaLollapatuza::menorStock(Producto producto) const {
    Lolla lollaCopia = _lolla;
    int res = lollaCopia.puestoIDMenorStock(producto);
   return res;
}

const set<Persona> &FachadaLollapatuza::personas() const {
    return _lolla.infoPersonas();
}

Nat FachadaLollapatuza::stockEnPuesto(IdPuesto idPuesto, const Producto &producto) const {
    Lolla copia = _lolla;
    Puesto p = copia.infoPuestos().at(idPuesto);
    return p.stock()[producto];
}

Nat FachadaLollapatuza::descuentoEnPuesto(IdPuesto idPuesto, const Producto &producto, Nat cantidad) const {
    Lolla copia = _lolla;
    Puesto p = copia.infoPuestos().at(idPuesto);
    int item = producto;
    int cant = cantidad;
    return p.obtenerDescuento(item, cant);
}

Nat FachadaLollapatuza::gastoEnPuesto(IdPuesto idPuesto, Persona persona) const {
    Lolla copia = _lolla;
    Puesto p = copia.infoPuestos().at(idPuesto);
    int res = p.obtenerGasto(persona);
    return res;
}

set<IdPuesto> FachadaLollapatuza::idsDePuestos() const {
    set<int> res;
    Lolla copia = _lolla;
    for(pair<int, Puesto> p : copia.infoPuestos()){
        res.insert(p.first);
    }
    return res;
}
