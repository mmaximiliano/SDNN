#include "Lollapatuza.h"

Lollapatuza::Lollapatuza(const map<IdPuesto, Puesto> puestos, const set<Persona> personas):
        Precios(preciosLolla(puestos)), //O(P * I * log(I))
        Puestos(puestos), //O(P * copiar puesto)
        Personas(personas),//O(A)
        PersonaQueMasGasto(personaConMenorId(personas)), //O(A)
        GastoTotalPorPersona(gastosEnCero(personas)), //O(A * log(A))
        HackeosPosibles(map<Producto, map<Persona, map<IdPuesto, Puesto*>>>()), //O(1)
        PersonaPorGasto(crearPersonaPorGasto(personas)) // O(A * log(A))
{}
//copiar puesto = O(I * CantidadDeDescuentos + I * max(longitud vector) + A * longListaMaxVentas + A * I)
//Complejidad de la implementación: O(P * I * log(I) + P * copia puesto + A * log(A))

void Lollapatuza::registrarCompra(Producto item, Nat cantidad, Persona persona, IdPuesto idPuesto){ //O(log(I) + log(A) + log(P) + log(Cant))
    Puesto& puesto = Puestos.at(idPuesto); //O(log(P))
    puesto.vender(persona, item, cantidad); //O(log (A) + log(I) + log(cant)).
    Nat gastoAnterior = GastoTotalPorPersona[persona]; //O(log(A))
    Nat gastoTotal = gastoAnterior + puesto.gastoDeVenta(item, cantidad); //O(log(I) + log(cant)).
    map<Persona, Nat> diccPersonas = PersonaPorGasto.at(gastoAnterior); //O(log(gasto)) donde gasto es cantidad total de gastos, que es menor igual a la cantidad de personas
    if (diccPersonas.size() == 1) { //O(1)
        PersonaPorGasto.erase(gastoAnterior); //O(log(A))
    } else {
        diccPersonas.erase(persona); //O(log(A))
    }
    if (PersonaPorGasto.count(gastoTotal) == 0){ //O(log(A))
        map<Persona, Nat> diccPersonasAAgregar; //O(1)
        diccPersonasAAgregar[persona] = 0; //O(log(A)) como era un diccionario vacio = O(1)
        PersonaPorGasto[gastoTotal] = diccPersonasAAgregar; //O(log(A))
    } else {
        PersonaPorGasto[gastoTotal][persona] = 0; //O(log(A))
    }
    GastoTotalPorPersona[persona] = gastoTotal; //O(log(A))
    if (GastoTotalPorPersona[PersonaQueMasGasto] < gastoTotal ||
    (GastoTotalPorPersona[PersonaQueMasGasto] == gastoTotal && persona < PersonaQueMasGasto)){ //O(log(A))
        PersonaQueMasGasto = persona; //O(1)
        if (puesto.cantidadConDescuento(item, cantidad) == 0) { //O(log(I) + log(cant))
            if (HackeosPosibles.count(item)== 0){ //O(log(I))
                HackeosPosibles[item] = map<Persona, map<IdPuesto, Puesto*>>(); //O(log(I))
            }
            if (HackeosPosibles[item].count(persona)== 0){ //O(log(I) + log(A))
                HackeosPosibles[item][persona] = map<IdPuesto, Puesto*>(); //O(log(I) + log(A))
            }
            if (HackeosPosibles[item][persona].count(idPuesto)== 0){ //O(log(I)) + log(A) + log(P))
                HackeosPosibles[item][persona][idPuesto] = &puesto; //O(log(I) + log(A) + log(P))
            }
        }
    }
}

void Lollapatuza::hackear(Producto item, Persona persona) { //O(log(I) + log(P) + log(A))
    auto itPuestoAHackear = HackeosPosibles[item][persona].begin(); //O(log(I) + log(A))
    Puesto* puestoAHackear = itPuestoAHackear->second; //O(1)
    IdPuesto IDpuestoaHackear = itPuestoAHackear->first; //O(1)
    auto itVentasSinDescuento = (*puestoAHackear).obtenerVentasSinDescuento().at(persona).at(item);//O(log(P) + log(I))
    if ((*itVentasSinDescuento).second == 1) {
        HackeosPosibles[item][persona].erase(IDpuestoaHackear); //O(log(I) + log(P) + log(A))
    }
    Nat gastoAnterior = GastoTotalPorPersona.at(persona); //O(log(A))
    map<Persona, Nat> diccPersonas = PersonaPorGasto.at(gastoAnterior); //O(log(A))
    if (diccPersonas.size() == 1){ //O(1)
        PersonaPorGasto.erase(gastoAnterior); //O(log(A))
    } else {
        diccPersonas.erase(persona); //O(log(P))
    }
    Nat gastoActualizado = gastoAnterior - (*puestoAHackear).obtenerPrecio(item); //O(log(I))
    GastoTotalPorPersona[persona] = gastoActualizado; //O(log(A))
    if (PersonaPorGasto.count(gastoActualizado) == 0){ //O(log(A))
        map<Persona, Nat> diccPersonasAgregar = map<Persona, Nat>(); //O(1)
        diccPersonasAgregar[persona] = 0; //O(log(A))
        PersonaPorGasto[gastoActualizado] = diccPersonasAgregar; //O(log(A))
    } else {
        PersonaPorGasto.at(gastoActualizado)[persona] = 0; //O(log(A))
    }
    (*puestoAHackear).hackearPuesto(persona, item); //O(log(A)+log(I))
    if (PersonaQueMasGasto == persona){ //O(1)
        auto itGastoMax = PersonaPorGasto.end(); //O(1)
        itGastoMax--; //O(1)
        map<Persona, Nat> diccPersonasQueMasGastaron = PersonaPorGasto.at(itGastoMax->first); //O(log(A))
        auto itPersonaMenorId = diccPersonasQueMasGastaron.begin(); //O(1)
        PersonaQueMasGasto = itPersonaMenorId->first; //O(1)
    }
}

Nat Lollapatuza::obtenerGastoTotalDeUnaPersona(Persona persona) const{ //O(log(A))
    return GastoTotalPorPersona.at(persona); //O(log(A))
}

Persona Lollapatuza::personaQueMasGasto() const { //O(1)
    return PersonaQueMasGasto;
}

IdPuesto Lollapatuza::puestoConMenorStockDeItem(Producto item) const { //O(P * I)
    auto it_res = Puestos.begin(); //O(1)
    IdPuesto res = it_res->first; //O(1)
    if (Precios.count(item) != 0) { //O(log(I))
        for (auto it = Puestos.begin(); it != Puestos.end(); it++){ //O(P)
            if (it->second.obtenerPrecios().count(item) != 0 ){ //O(I + log(I))
                if(it->second.obtenerElStock(item) < it_res->second.obtenerElStock(item) ||
                (it->second.obtenerElStock(item) == it_res->second.obtenerElStock(item) && it->first < res)) { //O(log(I))
                    it_res = it;
                    res = it->first; //O(1)
                }
            } else {
                it_res = it;
                res = it->first; //O(1)
            }
        }
    }
    return res;
}

const set<Persona>& Lollapatuza::obtenerPersonas() const{ //O(1)
    return Personas;
}

const map<IdPuesto, Puesto>& Lollapatuza::obtenerPuestos() const{ //O(1)
    return Puestos;
}

map<Producto,Nat> Lollapatuza::preciosLolla(const map<IdPuesto, Puesto>& puestos) const{ //O(P * I * log(I))
    map<Producto, Nat> res = map<Producto, Nat>(); //O(1)
    for (auto itPuestos = puestos.begin(); itPuestos != puestos.end(); itPuestos++){ //O(P)
        map<Producto, Nat> m = itPuestos->second.obtenerPrecios(); //O(I)
        for(auto itItem = m.begin(); itItem != m.end(); itItem++){ //O(I)
            if (res.count(itItem->first) == 0){ //O(log(I))
                res[itItem->first] = itItem->second; //O(log(I))
            }
        }
    }
    return res;
}

Persona Lollapatuza::personaConMenorId(const set<Persona>& personas) const { //O(A)
    auto itr = personas.begin(); //O(1)
    Persona res = *itr; //O(1)
    for (itr = personas.begin(); itr != personas.end(); itr++){ //O(A)
        if (*itr < res){ //O(1)
            res = *itr; //O(1)
        }
    }
    return res;
}

map<Persona, Nat> Lollapatuza::gastosEnCero(const set<Persona>& personas){ //O(A * log(A))
    map<Persona, Nat> res = map<Persona, Nat>(); //O(1)
    for (auto itr = personas.begin(); itr != personas.end(); itr++){ //O(A)
        res[*itr]= 0; //O(log(A))
    }
    return res;
}

map<Nat, map<Persona, Nat>> Lollapatuza::crearPersonaPorGasto(const set<Persona>& personas){ //O(A * log(A))
    map<Nat, map<Persona, Nat >> res = map<Nat, map<Persona, Nat >>(); //O(1)
    res[0] = gastosEnCero(personas); //O(log(A) + A * log(A))
    return res;
}

set<IdPuesto> Lollapatuza::ObtenerClavesPuesto() const{ //O(P)
    set<IdPuesto> claves = set<IdPuesto>(); //O(1)
    for (auto it = Puestos.begin(); it != Puestos.end(); it++) { //O(P)
        claves.insert(it->first); //O(1)
    }
    return claves;
}

Lollapatuza::Lollapatuza() {} //O(1)