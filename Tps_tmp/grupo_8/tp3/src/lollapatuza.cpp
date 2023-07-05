#include "lollapatuza.h"

Lollapatuza::Lollapatuza(const map<IdPuesto, Puesto>& p, const set<Persona>& a) : _puestos(p), _puestosIts(), _personas(a), _gastoPersona(), _masGastaron(), _ordenGasto(), _comprasHackeables() {
    set<Persona>::const_iterator i = a.begin();
    set<Persona>::const_iterator z = a.end();
    int contador = 0;
    int maxPersona = *(--z);

    for(int x = 0; x <= maxPersona; x++){
        _ordenGasto.push_back(-1);
    }

    while (i != a.end()) {
        _gastoPersona[*i] = 0;
        _comprasHackeables[*i] = {};
        _masGastaron.encolar(PerGasto(*i, 0));
        _ordenGasto[*i] = contador;
        contador++;
        ++i;
    }

    map<IdPuesto, Puesto>::iterator j = _puestos.begin();

    while (j != _puestos.end()) {
        _puestosIts[j->first] = j;
        ++j;
    }
}

void Lollapatuza::transaccion(Persona a, IdPuesto id, Producto i, Nat c) {
    bool huboDesc;
    Nat gasto = _puestos[id].vender(a, i, c, huboDesc);
    _gastoPersona[a] += gasto;
    _masGastaron.siftUpTrack(_ordenGasto[a], _ordenGasto, gasto);

    if(!huboDesc){
        if(_comprasHackeables[a].count(i) == 0){
            _comprasHackeables[a][i] = make_tuple(ColaPrior<PuestoIdIt>(), map<IdPuesto, bool>());
        }
        if(get<1>(_comprasHackeables[a][i]).count(id) == 0){
            get<0>(_comprasHackeables[a][i]).encolar(PuestoIdIt(id, _puestosIts[id]));
            get<1>(_comprasHackeables[a][i])[id] = true;
        }
    }
}

bool Lollapatuza::hackear(Producto i, Persona a) {
    if (_comprasHackeables[a].count(i)) {
        bool noHack;

        int puestoId = get<0>(_comprasHackeables[a][i]).proximo().id;
        auto puestoIt = get<0>(_comprasHackeables[a][i]).proximo().it;

        Nat gasto = puestoIt->second.olvidar(a, i, noHack);
        _gastoPersona[a] -= gasto;
        _masGastaron.siftDownTrack(_ordenGasto[a], _ordenGasto, gasto);

        if (noHack) {
            get<0>(_comprasHackeables[a][i]).desencolar();
            get<1>(_comprasHackeables[a][i]).erase(get<1>(_comprasHackeables[a][i]).find(puestoId));
            if (get<0>(_comprasHackeables[a][i]).tam() == 0) {
                _comprasHackeables[a].erase(_comprasHackeables[a].find(i));
            }
        }
        return true;
    } else {
        return false;
    }
}

Nat Lollapatuza::gastoPersona(Persona a) const {
    return _gastoPersona.at(a);
}

Persona Lollapatuza::personaQueMasGasto() const {
    return _masGastaron.proximo().id;
}

IdPuesto Lollapatuza::menorStock(Producto i, bool &x) const {
    auto it = _puestos.begin();
    int stockDelMenor = -1;
    int res = 0;

    while (it != _puestos.end()) {
        int pid = it->first;
        Puesto p = it->second;
        if (stockDelMenor == -1 || stockDelMenor > p.devolverStock(i) || (stockDelMenor ==  p.devolverStock(i) && res > pid )) {
            stockDelMenor = p.devolverStock(i);
            res = pid;
        }
        ++it;
    }

    x = stockDelMenor != 0;
    return res;
}

const set<Persona>& Lollapatuza::personas() const {
    return _personas;
}

const map<IdPuesto, Puesto>& Lollapatuza::puestos() const {
    return _puestos;
}

Lollapatuza::Lollapatuza() : _puestos(), _puestosIts(), _personas(), _gastoPersona(), _masGastaron(), _ordenGasto(), _comprasHackeables() {}