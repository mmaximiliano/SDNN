#include "Puesto.h"

Puesto::Puesto(const map<Producto, Nat> precios,const Stock stock,const Promociones descuentos): //O(I * c + I * max(longitud del vector))
        StockPorItem(stock), //O(I)
        DescuentoPorCantidadPorItem(descuentos), //O(I * c) donde c es la cantidad de descuentos definidos
        CantidadesConDescuentos(transformarACantidadesConDescuentos(descuentos)), //O(I * c + I * max(longitud del vector))
        GastoPorPersona(map<Persona, Nat>()), //O(1)
        Precios(precios), //O(I)
        Ventas(map<Persona, list<pair<Producto, Nat>>>()), //O(1)
        VentasSinDescuento(map<Persona, map<Producto, list<pair<Producto, Nat>>::iterator>>()) {} //O(1)


Nat Puesto::obtenerElStock(const Producto& item) const{ //O(log(I))
    return StockPorItem.at(item); //O(log(I))
}

Nat Puesto::obtenerDescuento(const Producto& item, Nat cantidad) const{ //O(log(I) + log(cant))
    Nat res = 0; //O(1)
    if (DescuentoPorCantidadPorItem.count(item) != 0) { //O(log(I))
        map<Nat, Nat> diccDescuento = DescuentoPorCantidadPorItem.at(item); //O(log(I) + log(cant))
        Nat cantConDescuento = cantidadConDescuento(item, cantidad); //O(log(I) + log(cant))
        if (cantConDescuento != 0){ //O(1)
            res = diccDescuento.at(cantConDescuento); //O(log(cant))
        }
    }
    return res;
}


Nat Puesto::obtenerGasto(Persona persona) const{ //O(log(A))
    Nat res = 0; //O(1)
    if (GastoPorPersona.count(persona) != 0){ //O(log(A))
        res = GastoPorPersona.at(persona); //O(log(A))
    }
    return res;
}

void Puesto::vender(Persona persona, Producto item, Nat cantidad){  //O(log(A) + log(I) + log(cant))
    StockPorItem[item] = obtenerElStock(item) - cantidad; //O(log(I))
    Nat gastoTotal = gastoDeVenta(item, cantidad); //O(log(I) + log(cant))
    GastoPorPersona[persona] = obtenerGasto(persona) + gastoTotal; //O(log(A))
    Nat cantConDescuento = cantidadConDescuento(item, cantidad); //O(log(I) + log(cant))
    if (Ventas.count(persona) == 0){ //O(log(A))
        Ventas[persona] = list<pair<Producto, Nat>>(); //O(log(A))
    }
    if (cantConDescuento != 0){ //O(1)
        Ventas[persona].push_back(make_pair(item, cantidad)); //O(log(A))
    } else {
        if (VentasSinDescuento.count(persona) != 0 && VentasSinDescuento.at(persona).count(item) != 0) { //O(log(A) + log(I))
            auto it_VentaSinDescuento = VentasSinDescuento.at(persona).at(item);  //O(log(A) + log(I))
            Nat cantidadAnterior = (*(it_VentaSinDescuento)).second; //O(1)
            (*(it_VentaSinDescuento)).second = cantidadAnterior + cantidad; //O(1)
        } else {
            Ventas[persona].push_back(make_pair(item, cantidad)); //O(log(A))
            auto it_VentaSinDescuento = Ventas.at(persona).end(); //O(log(A))
            it_VentaSinDescuento--; //O(1)
            if (VentasSinDescuento.count(persona) == 0) {  //O(log(A))
                VentasSinDescuento[persona] = map<Producto, list<pair<Producto, Nat>>::iterator>(); //O(log(A))
            }
            VentasSinDescuento.at(persona)[item] = it_VentaSinDescuento; //O(log(A) + log(I))
        }
    }
}


Nat Puesto::obtenerPrecio(Producto item) const{ //O(log(I))
    return Precios.at(item); //O(log(I))
}


map<Producto, vector<Nat>> Puesto::transformarACantidadesConDescuentos(const Promociones& descuentos) const{ //O(I * c + I * max(longitud del vector))
    map<Producto, vector<Nat>> res = map<Producto, vector<Nat>>(); //O(1)
    auto it_items = descuentos.begin(); //O(1)
    while (it_items != descuentos.end()) { //O(I)
        vector<Nat> vec(0); //O(1)
        map<Nat, Nat> diccCantConDesc = it_items->second; //O(1)
        auto it_cantidades = diccCantConDesc.begin(); //O(1)
        while (it_cantidades != diccCantConDesc.end()){ //O(c) donde c es la cantidad de descuentos definidos
            vec.push_back(it_cantidades->first); //O(1)
            it_cantidades++; //O(1)
        }
        res[it_items->first] = vec; //O(1)
        it_items++; //O(1)
    }
    return res; //O(I * max(longitud del vector)) como la longitud del vector no está acotada el costo máximo que habría que pagar seria el del vector mas largo
}



Nat Puesto::gastoDeVenta(Producto item, Nat cantidad) const{ //O(log(I) + log(cant))
    Nat descuentoPosible = obtenerDescuento(item, cantidad); //O(log(I) + log(cant))
    return cantidad*(obtenerPrecio(item) - ((obtenerPrecio(item)*descuentoPosible)/100)); //O(log(I))
}


void Puesto::hackearPuesto(Persona persona, Producto item){ // O(log(A) + log(I))
    map<Producto, list<pair<Producto, Nat>>::iterator> diccVentasSinDesc = VentasSinDescuento.at(persona); //O(log(A))
    auto it_VentasSinDesc = diccVentasSinDesc.at(item); //O(log(I))
    pair<Producto, Nat>& venta = *it_VentasSinDesc; //O(1)
    if (venta.second > 1){
        venta.second--;
    } else {
        Ventas.at(persona).erase(it_VentasSinDesc); //O(log(A))
        VentasSinDescuento.at(persona).erase(item); //O(log(A))
    }
    GastoPorPersona[persona] = obtenerGasto(persona) - obtenerPrecio(item); //O(log(A) + log(I))
    StockPorItem[item] = obtenerElStock(item) + 1; //O(log(I))
}

Nat Puesto::cantidadConDescuento(Producto item, Nat cantidad) const{//O(log(I) + log(cant))
    Nat res = 0; //O(1)
    if (DescuentoPorCantidadPorItem.count(item) != 0){ //O(log(I))
        vector<Nat> vecCant = CantidadesConDescuentos.at(item); //O(log(I))
        if (cantidad >= vecCant[0]){ //O(1)
            int ultimaPos = vecCant.size()-1; //O(1)
            int primeraPos = 0; //O(1)
            if (vecCant[ultimaPos] <= cantidad) { //O(1)
                res = vecCant[ultimaPos]; //O(1)
            }else{
                while (primeraPos + 1 < ultimaPos){ //O(log(cant))
                    int medio = (ultimaPos + primeraPos)/2; //O(1)
                    if (cantidad < vecCant[medio]){ //O(1)
                        ultimaPos = medio; //O(1)
                    } else{
                        primeraPos = medio;//O(1)
                    }
                }
                res = vecCant[primeraPos]; //O(1)
            }
        }
    }
    return res;
}

map<Producto, Nat>  Puesto::obtenerPrecios() const{ //O(I)
    return Precios; //O(I)
}

const map<Persona, map<Producto, list<pair<Producto, Nat>>::iterator>>& Puesto::obtenerVentasSinDescuento() const { //O(1)
    return VentasSinDescuento; //O(1)
}

Puesto::Puesto() {} //O(1)



