#include "puesto.h"

Puesto::Puesto(const Menu& p, const Stock& s, const Promociones& d) : _stock(s), _menu(p), _gastoPorPersona(), _ventas(), _descuentos(), _ventasSinDescuento() {
    Promociones::const_iterator itItem = d.begin();
    while (itItem != d.end()) {
        vector<Nat> desc;
        desc.push_back(0);
        vector<Nat> cortes;
        cortes.push_back(0);
        map<Nat, Nat>::const_iterator itDesc = itItem->second.begin();
        Nat pos = 1;
        Nat prev = 0;
        while (itDesc != itItem->second.end()) {
            cortes.push_back(itDesc->first);
            while (pos < itDesc->first) {
                desc.push_back(prev);
                pos++;
            }
            prev = itDesc->second;
            ++itDesc;
        }
        desc.push_back(prev);
        _descuentos[itItem->first] = make_tuple(desc, cortes);
        ++itItem;
    }
}

Nat Puesto::devolverStock(Producto i) {
    return _stock.count(i) == 0 ? 0 : _stock[i];
}

Nat Puesto::descuento(Producto i, Nat c) {
    if (_descuentos.count(i)) {
        tuple<vector<Nat>, vector<Nat>> descuentos_item = _descuentos[i];
        if (c >= get<0>(descuentos_item).size()) {
            return get<0>(descuentos_item)[get<0>(descuentos_item).size()-1];
        } else {
            return get<0>(descuentos_item)[c];
        }
    } else {
        return 0;
    }
}

Nat Puesto::gasto(Persona a) {
    return _gastoPorPersona.count(a) ? _gastoPorPersona[a] : 0;
}

Nat Puesto::vender(Persona a, Producto i, Nat c, bool &huboDesc) {
    _stock[i] -= c;
    Nat desc = descuento(i,c);
    Nat valorVenta = (((100-desc) * _menu[i]*c)/100);
    _gastoPorPersona[a] += valorVenta;
    huboDesc = desc > 0;

    if (!_ventas.count(a)){
        _ventas[a] = list<tuple<Producto,Nat>>();
    }

    auto it = _ventas[a].insert(_ventas[a].end(), make_tuple(i, c));

    if (!huboDesc){
        if (!_ventasSinDescuento.count(a)){
            _ventasSinDescuento[a] = map<Producto, list<list<tuple<Producto, Nat>>::iterator>>();
        }
        if (!_ventasSinDescuento[a].count(i)){
            _ventasSinDescuento[a][i] = list<list<tuple<Producto, Nat>>::iterator>();
        }
        _ventasSinDescuento[a][i].push_back(it);
    }
    return valorVenta;
}

Nat Puesto::olvidar(Persona a, Producto i, bool &noHack){
    auto venta = _ventasSinDescuento[a][i].back();
    get<1>(*venta)--;

    if(get<1>(*venta) == 0){
        _ventas[a].erase(venta);
        _ventasSinDescuento[a][i].pop_back();
    }

    _stock[i]++;
    Nat res = _menu[i];
    _gastoPorPersona[a] -= res;
    noHack = _ventasSinDescuento[a][i].empty();
    return res;
}

//Este solo existe para que map[] no se queje. No hay que usarlo.
Puesto::Puesto() : _stock(), _menu(), _gastoPorPersona(), _ventas(), _descuentos(), _ventasSinDescuento() {}