//
// Created by magal on 6/25/2023.
//

#include "puesto.h"
#include "algorithm"

Puesto::Puesto() {}

Puesto::Puesto(map<Producto, Nat> precios, map<Producto, Nat> stocks, map<Producto, vector<pair<Nat, Nat>>> descuentos){
    _stocks = stocks;
    _precios = precios;
    _gastosTotales = map<Producto, Nat>();
    _gastos = map<Persona, map<Producto, list<Nat>[2]>>();

    _descuentos = map<Producto, vector<descuento>>();

    for(pair<Producto, vector<pair<Nat, Nat>>> prodYDescuentos : descuentos){
        vector<descuento> descuentosDelProducto = vector<descuento>();
        for(pair<Nat,Nat> tupla: prodYDescuentos.second){
            descuento desc = descuento(tupla);
            descuentosDelProducto.push_back(desc);
        }
        sort(descuentosDelProducto.begin(), descuentosDelProducto.end());
        _descuentos[prodYDescuentos.first] = descuentosDelProducto;
    }
}

Nat Puesto::obtenerStock(Producto i) {
    return _stocks[i];
}

Nat Puesto::obtenerDescuento(Producto i, Nat cant) {
    if(this->tieneDescuento(i, cant)){
        Nat l = 0;
        Nat r = _descuentos[i].size()-1;

        if(cant >= _descuentos[i][r].cantidad){
            return _descuentos[i][r].porcentaje;
        } else {
            while(l < r-1){
                Nat m = (l + r)/2;
                if(cant >= _descuentos[i][m].cantidad){
                    l = m;
                } else {
                    r = m;
                }
            }
            return _descuentos[i][l].porcentaje;
        }
    } else {
        return 0;
    }
}

Nat Puesto::obtenerGasto(Persona a){
    return _gastosTotales[a];
}

Nat Puesto::obtenerPrecio(Producto i){
    return _precios[i];
}

void Puesto::vender(Persona a, Producto i, Nat cant){
    _stocks[i] = _stocks[i] - cant;

    if(_gastosTotales.count(a) == 0){
        _gastosTotales[a] = 0;
    }

    Nat precio = calcularPrecio(i, cant);
    _gastosTotales[a] = _gastosTotales[a] + precio;

    if(_gastos[a].count(i) == 0){
        _gastos[a][i][0] = list<Nat>(0);
        _gastos[a][i][1] = list<Nat>(0);
    }

    if(!tieneDescuento(i, cant)){
        _gastos[a][i][0].push_back(cant);
    } else {
        _gastos[a][i][1].push_back(cant);
    }
}

bool Puesto::tieneDescuento(Producto i, Nat cant) {
    return (!_descuentos[i].empty() && cant >= _descuentos[i][0].cantidad);
}

Nat Puesto::calcularPrecio(Producto i, Nat cant){
    Nat res = 0;
    Nat precio = obtenerPrecio(i);

    if(tieneDescuento(i, cant)){
        Nat descuento = obtenerDescuento(i, cant);
        res = precio * cant * (100 - descuento)/100;
    } else {
        res = precio * cant;
    }

    return res;
}
//HAY QUE ELIMINAR SOLO UN ELEMENTO. CAMBIARLO AUN VOID?
void Puesto::anularCompra(Persona a, Producto i){
    //YA NO HACE FALTA, LO DEJO PARA QUE NO SE ROMPA
    Nat cantidad = _gastos[a][i][0].front();

    //Nat precio = obtenerPrecio(i) * cantidad;

    //RESTA EL VALOR DE UNA UNIDAD AL GASTO TOAL
    _gastosTotales[a] = _gastosTotales[a] - obtenerPrecio(i);

    //RESTA UN ITEM DE LA CANTIDAD COMPRADA POR LA PERSONA
    _gastos[a][i][0].front()--;

    //AGREGA UN ITEM AL STOCK
    _stocks[i] = _stocks[i] + 1;

}

bool Puesto::comproItemSinDescuento(Persona a, Producto i){
    return _gastos[a][i][0].size() != 0;
}

/*Puesto& Puesto::operator=(const Puesto& otro){
    return *this;
}*/

