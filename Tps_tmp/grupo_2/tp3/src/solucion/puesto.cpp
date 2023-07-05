//
// Created by Admin on 26/6/2023.
//
#include "puesto.h"

template<typename T, typename S>
bool estaDefinido(const map<T, S>& m, T clave){
    return m.count(clave) == 1;
}

//O(I*(log(I)+cant)) , nota para el corrector: calculamos mal la complejidad de esta operacion en el TP 2
Puesto::Puesto(const Menu& menu, const Promociones& descuentos, const Stock& stocks) {
    //Itero sobre el parametro menu, que tiene todos los items
    auto itDeMenu = menu.begin();
    while(itDeMenu != menu.end()){ //O(I)
        Producto item = itDeMenu->first;

        //Define el item con su precio, stock y, en caso de tenerlos, sus descuentos
        Nat precioDelItem = menu.at(item);
        Nat stockDelItem = stocks.at(item);
        map<Nat, Nat>* descuentosDelItem = nullptr;

        //O(log(I)+cant)
        if (estaDefinido(descuentos,item)) { //O(log(I))
            descuentosDelItem = new map<Nat, Nat>(descuentos.at(item)); //O(log(I)+ cant)
        }

        //Se insertan los 3 datos obtenidos, en el caso donde el item no tuviera descuentos, se le define un diccionario vacio
        //O(log(I))
        _items.insert(std:: pair<Producto ,info_item>(item, info_item(precioDelItem, stockDelItem, descuentosDelItem)));

        itDeMenu++;
    } //O(I*(log(I)+cant))
}

//O(log(A))
Nat Puesto::cuantoGasto(Persona persona) const{
    Nat res = 0;
    if (estaDefinido(_clientes, persona)){
        res = _clientes.at(persona);
    }
    return res;
}

//O(log(I)+log(cant))
const Nat Puesto::descuentoItem(Producto item, Nat cantidad){
    //La informacion del item contiene stock, precio y dtos
    info_item& informacionItem = _items.at(item); //O(log(I))

    Nat res=0;
    if (informacionItem.descuentos != nullptr){
        //Crea un puntero al diccionario de descuentos
        map<Nat, Nat>* dtosDelItem = informacionItem.descuentos;

        if (estaDefinido(*dtosDelItem, cantidad)){ //O(log(cant))
            //Si la cantidad tiene un dto definido lo devuelve
            res =  dtosDelItem->find(cantidad)->second; //O(log(cant))
        }else{
            //Define la cantidad recibida con 0% de descuento para buscar la cantidad antecesora y nombra al iterador devuelto para facilitar la lectura
            auto temp = dtosDelItem->insert(std:: pair<Nat, Nat>(cantidad,0)); //O(log(cant))
            auto itCantidad = temp.first;

            //Al definir un nuevo elemento, este se ordena comparandose con las demas claves mediante el operador <, por lo que
            //toma la clave anterior al elemento recien definido y retorna su significado (en caso ser la primera clave, no hay descuento por lo que se retorna 0)
            if (itCantidad != dtosDelItem->begin()){
                //Accedo al elemento anterior y tomo su significado
                itCantidad--;
                res = itCantidad->second;
            }
            //Borro la clave y significados definidos previamente
            dtosDelItem->erase(cantidad); //O(log(cant))
        }
    }
    return res;
}

//O(log(I))
Nat Puesto::stock(Producto item) const{
    return  _items.at(item).stock;
}

//O(log(I))
Nat Puesto::precioItem(Producto item) const{
    return _items.at(item).precio;
}

//O(log(I))
void Puesto::cambiarStock(Producto item, Nat nuevoStock) {
    _items.find(item)->second.stock = nuevoStock;
}

//O(log(A))
void Puesto::cambiarGastoDePersona(Persona dni, Nat NuevoGasto) {
        _clientes[dni]= NuevoGasto;
}

//O(I)
list<Producto> Puesto ::devolverItems() {
    map<Producto, info_item> :: iterator it = _items.begin();
    list<Producto> res;
    while (it != _items.end()){
        res.push_back(it->first);
        it++;
    }
    return res;
}

//O(log(A))
void Puesto::eliminarAPersona(Persona dni){
    this->_clientes.erase(dni);
}
