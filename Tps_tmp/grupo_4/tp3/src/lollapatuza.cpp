//
// Created by magal on 6/25/2023.
//
#include "lollapatuza.h"

using namespace std;
Lollapatuza::Lollapatuza() {}


Lollapatuza::Lollapatuza(const set<Persona> &m, const map<IdPuesto, Puesto> &infoPuestos) :_puestos(infoPuestos), _personasSet(m),_personas(m),_personaQueMasGasto(make_pair(0, _personas.min())){
    //idea copiar la info al dccionario PuestosxIterador. O(cantDePuestos)
    map<IdPuesto, Puesto>::iterator it = _puestos.begin();
    while(it != _puestos.end()){
        _puestosXiteradores[it->first] = it;
        it++;
    }
}

void Lollapatuza::registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto) {
    Puesto& p = _puestos.at(idPuesto);
    p.vender(persona, producto, cant);

    if(_gastosXpersona.count(persona) == 0){
        _gastosXpersona[persona] = 0;
    }

    personaYgasto pyg = personaYgasto(_gastosXpersona[persona], persona);
    if(_gastosOrdenados.pertenece(pyg)){
        _gastosOrdenados.eliminar(pyg);
    }
    Nat precio = p.calcularPrecio(producto, cant);
    _gastosXpersona[persona] = _gastosXpersona[persona] + precio;
    _gastosOrdenados.Agregar(make_pair(_gastosXpersona[persona], persona));
    _personaQueMasGasto = _gastosOrdenados.max();
    if (!p.tieneDescuento(producto, cant)) {
        map<IdPuesto, Puesto>::iterator it = _puestosXiteradores[idPuesto];
        _puestosOrdenados[persona][producto].Agregar(make_pair(idPuesto, it));
    }

    if(!_puestosOrdenados[persona][producto].esVacio()){
        idPuestoYiterador aux = _puestosOrdenados[persona][producto].min();
        _proximoHackeo[persona][producto] = aux;
    }

}

void Lollapatuza::hackear(Persona persona, Producto producto) {
    //Obteniendo puesto y su id
    idPuestoYiterador puestoEIterador = _proximoHackeo[persona][producto];
    IdPuesto id = puestoEIterador.id;

    map<IdPuesto, Puesto>::iterator itPuesto = puestoEIterador.it;
    Puesto& puesto = (*itPuesto).second;

    //Eliminando la tupla que continene a la persona y a su gasto total antes de ser hackeada a _gastosOrdenados
    personaYgasto personaYGastoAHackear = personaYgasto(_gastosXpersona[persona], persona);
    _gastosOrdenados.eliminar(personaYGastoAHackear);

    //Hackeando el puesto
    puesto.anularCompra(persona, producto);

    //Actualizando el gasto toal de la persona luego de ser hackeada
    _gastosXpersona[persona] -= puesto.obtenerPrecio(producto);

    //Agregando la tupla que continene a la persona y a su gasto total luego de ser hackeada a _gastosOrdenados
    personaYgasto personaYGastoNuevo = personaYgasto(_gastosXpersona[persona], persona);
    _gastosOrdenados.Agregar(personaYGastoNuevo);

    //Actualizando la persona que más gastó
    _personaQueMasGasto = _gastosOrdenados.max();

    //Si el puesto deja de ser hackeable, acutalizamos proximoHackeo
    if(!puesto.comproItemSinDescuento(persona, producto)){
        ConjLog<idPuestoYiterador> puestosEnLosQueComproSinDescuento = _puestosOrdenados[persona][producto];
        puestosEnLosQueComproSinDescuento.eliminar(puestoEIterador);
        //Si no quedan puestos hackeables para ese producto, eliminamos la clave del producto de puestosOrdenados y proximoHackeo para esa persona
        if (puestosEnLosQueComproSinDescuento.esVacio()){
            _puestosOrdenados[persona].erase(_puestosOrdenados[persona].find(producto));
            _proximoHackeo[persona].erase(_proximoHackeo[persona].find(producto));
        }
        //Si hay puestos hackeables, elejimos al de menor id para el proxio hackeo
        else{
            _proximoHackeo[persona][producto] = _puestosOrdenados[persona][producto].min();
        }
    }
}

Nat Lollapatuza::gastoTotal(Persona persona) const {
    if( _gastosXpersona.count(persona) == 0){
        return 0;
    }
    else{
        return _gastosXpersona.at(persona);
    }
}

Persona Lollapatuza::mayorGastador() const {
    return _personaQueMasGasto.persona;
}

IdPuesto Lollapatuza::menorStock(Producto producto) const {
    auto itPuesto = _puestos.begin();
    IdPuesto idMenorStock = itPuesto->first;
    Puesto puestoMenorStock = itPuesto->second;

    for(pair<IdPuesto, Puesto> ps : _puestos){
        Puesto puesto = ps.second;
        IdPuesto id = ps.first;

        if((puesto.obtenerStock(producto) < puestoMenorStock.obtenerStock(producto)) || (puesto.obtenerStock(producto) == puestoMenorStock.obtenerStock(producto) && id < idMenorStock)){
            idMenorStock = id;
            puestoMenorStock = puesto;
        }
    }

    return idMenorStock;
}

const set<Persona> &Lollapatuza::personas() const{
    return _personasSet;
}

const map<IdPuesto, Puesto> &Lollapatuza::puestos() const{
    return _puestos;
}