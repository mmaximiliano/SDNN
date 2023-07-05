//Grupo Chip
//Integrante                LU      Correo electrónico
//Beren, Manuel Andrés      320/22  m.beren83@gmail.com
//Carrillo, Mariano         358/22  carr.mariano@gmail.com
//Roitman, Sofía            563/21  sofiroit@gmail.com
//Valentini, Justo Agustin  566/22  justovalentini@gmail.com

#include "Puesto.h"

Puesto::Puesto(map<int, int>& precios, map<int, int>& stock, map<int, map<int, int>>& descuentos, int& id) :   //O(copy(map) + I + cantMax)
_id(id), _stock(stock), _precios(precios), _sinDesc(map<int, map<int, int>>()), _ventas(map<int, int>()) {
    _descuentos = map<int, map<int, int>>();
    for(pair<int, map<int, int>> item : descuentos){ //item es diccLog(item, diccLog(cant, desc))
        map<int,int>::const_iterator it = item.second.end();
        it--; //it está en la última posición de los descuentos del ítem
        pair<int, int> maxDesc = *it; // devuelve el máximo descuento de un ítem como (cant, desc)
        map<int, int> descItem; //descItem es el diccionario que despues se va a insertar en el de los descuentos
        int descActual = 0;
        for(int i = 1; i<=maxDesc.first;i++){
            if(item.second.count(i)==1){ //chequea si esta definido el descuento
                descActual = item.second[i]; //Si esta definido, cambio el valor de descActual y le pongo el mismo valor hasta encontrar un desc distinto
            }
            descItem.insert(make_pair(i, descActual));
        }
        _descuentos.insert(make_pair(item.first, descItem)); //Inserta el dicc del item
    }
}    //En el diseño hicimos esto distinto, el dicc de descuentos está
     //definido a partir de la mínima cantidad, aca esta definido entre 1 y la cantidad máxima


map<int, int>& Puesto::stock(){ //O(1)
    return _stock;
}

int Puesto::obtenerDescuento(int& item, int& cant){     //O(log(I) + log(cantMax))
    int res = 0;
    if(_descuentos.count(item)!=0) { //Hay que poner este if porque en los tests hay casos en que el puesto no vende el item
        map<int, int> descItem = _descuentos[item];
        map<int, int>::const_iterator it = descItem.end();
        it--;
        int max = (*it).first;
        if (max <= cant) { //Si la cantidad es mayor a la del maximo descuento, devuelvo el descuento maximo
            res = (*it).second;
        } else { //Si no, devuelvo el descuento apropiado para la cantidad
            res = descItem[cant];
        }
    }
    return res;
}

int& Puesto::obtenerGasto(int& persona){ //O(log (A))
    return _ventas[persona];
}

const int Puesto::id(){ //O(1)
    return _id;
}

map<int, int> Puesto::precios(){ //O(1)
    return _precios;
}

map<int, map<int, int>>& Puesto::sinDesc(){ //O(1)
    return _sinDesc;
}

map<int, int>& Puesto::ventas(){ //O(1)
    return _ventas;
}

Puesto& Puesto::operator=(const Puesto& p){
    this->_id = p._id;
    this->_stock = p._stock;
    this->_descuentos = p._descuentos;
    this->_precios = p._precios;
    this->_ventas = p._ventas;
    this->_sinDesc = p._sinDesc;
    return *this;
}