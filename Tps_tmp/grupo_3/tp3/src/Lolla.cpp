//Grupo Chip
//Integrante                LU      Correo electrónico
//Beren, Manuel Andrés      320/22  m.beren83@gmail.com
//Carrillo, Mariano         358/22  carr.mariano@gmail.com
//Roitman, Sofía            563/21  sofiroit@gmail.com
//Valentini, Justo Agustin  566/22  justovalentini@gmail.com

#include "Lolla.h"
#include "minHeap.h"

Lolla::Lolla(map<int, Puesto> puestos, const set<int> personas) : _idPuestos(puestos), _personas(personas){   //O(A*log(A) + copy(map) + copy(set)); no nos hizo falta recorrer el map de puestos como en el tp2.
    map<int, set<int>> maxCons;
    maxCons.insert(make_pair(0, personas));
    _maxConsumidor = maxCons;
    _gastoTotal = map<int, int>();
    for(int persona: personas){
        _gastoTotal.insert(make_pair(persona, 0));
    }
    _comprasHack = map<int, map<int, minHeap>>();
}
//C++ ordena los conjuntos automáticamente, asi que no hace falta usar un diccionario que apunte a algo que no importa
//Usamos directamente un set, que tiene los elementos ordenados.


void Lolla::comprar(Puesto& puesto, int persona, int item, int cant){    //O(log(P) + log(I) + log(cantMax) + log(A))
    puesto.stock()[item] = puesto.stock()[item] - cant; //cambia el stock
    float descuento = float(puesto.obtenerDescuento(item, cant))/float(100); //Tuvimos que usar float porque si no redondeaba a 0 o a 1
    int gastoCompra = puesto.precios()[item]*cant*(1-descuento);
    int gastoActualizado = _gastoTotal[persona] + gastoCompra; //calcula el gasto nuevo de la persona
    int gastoDesactualizado = _gastoTotal[persona]; //guarda el gasto viejo, sirve para no tener que buscarlo cada vez que sea necesario
    _gastoTotal[persona] = gastoActualizado; //Actualiza el gasto total de la persona en el lolla
    puesto.obtenerGasto(persona) = puesto.obtenerGasto(persona) + gastoCompra; //Actualiza el gasto de la persona en el puesto
    _maxConsumidor[gastoDesactualizado].erase(persona); //borra a la persona del gasto viejo
    if(_maxConsumidor[gastoDesactualizado].empty()){ //Si era la unica persona con el gasto viejo, borra el valor del dicc maxConsumidor
        _maxConsumidor.erase(gastoDesactualizado);
    }
    if(_maxConsumidor.count(gastoActualizado)==1){ //Si el gasto esta definido en el dicc, agrego a la persona
        _maxConsumidor[gastoActualizado].insert(persona);
    } else { //si no, defino el gasto y despues agrego
        set<int> personas;
        personas.insert(persona);
        _maxConsumidor.insert(make_pair(gastoActualizado, personas));
    }

    if(descuento==0) { //Caso sin descuento, no hace falta hacer un caso con descuento porque cambiamos la estructura
        if (_comprasHack.count(persona) == 0) { //Si la persona no hizo ninguna compra sin descuento, la agrego a comprasHack
            _comprasHack.insert(make_pair(persona, map<int, minHeap>()));
        }
        if (_comprasHack[persona].count(item) == 0) { //Si la persona nunca compro el item sin descuento, defino el item
            _comprasHack[persona].insert(make_pair(item, minHeap()));
        }
        if (puesto.sinDesc().count(persona) == 0) { //Si la persona no habia hecho ninguna compra sin descuento en el puesto, la defino
            puesto.sinDesc().insert(make_pair(persona, map<int, int>()));
        }
        if ((puesto.sinDesc())[persona].count(item) == 1) { //Si la persona ya habia comprado el item sin descuento en ese puesto, sumo la cantidad
            (puesto.sinDesc())[persona][item] = (puesto.sinDesc())[persona][item] + cant;
        } else { //Si no, defino el item con la cantidad correspondiente
            puesto.sinDesc()[persona].insert(make_pair(item, cant));
            _comprasHack[persona][item].encolar(puesto);

            //Agrego el puesto al heap acá porque como el item no estaba definido en las compras sinDesc de la persona en el puesto,
            //entonces el puesto tampoco va a estar en el heap

            //Si la persona ya habia comprado en ese puesto ese item sin descuento, entonces el puesto ya estaba en el heap y al
            //agregarlo se repetiria y no cumpliria con la precondicion
        }
    }
}

void Lolla::hackear(int persona, int item){    //O(log(A) + log(I) + log(P))
    Puesto& puestoHack = _idPuestos.at(_comprasHack[persona][item].proximo().id()); //Busca el puesto a hackear
    int precioItem = puestoHack.precios()[item]; //Busca el precio del item
    puestoHack.ventas()[persona] = puestoHack.ventas()[persona] - precioItem; //Resta el precio del gasto de la persona en el puesto
    int gastoDesactualizado = _gastoTotal[persona]; //Guarda el gasto anterior
    int gastoActualizado = _gastoTotal[persona] - precioItem; //Guarda el gasto actualizado
    _gastoTotal[persona] = gastoActualizado; //Actualiza el gasto de la persona en el lolla
    puestoHack.stock()[item]++; //Devuelve el stock al puesto
    puestoHack.sinDesc()[persona][item]--;//Le resta uno a la cantidad de veces que se puede hackear a la persona
    if(puestoHack.sinDesc()[persona][item]==0){ //Si el puesto deja de ser hackeable, lo desencolo
        _comprasHack[persona][item].desencolar();
    }
    if(_comprasHack[persona][item].size()==0){ //Si la persona se queda sin compras del item, elimino el item del dicc
        _comprasHack[persona].erase(item);
    }
    if(_comprasHack[persona].size()==0){ //Si la persona se queda sin compras sin descuento, la borro del dicc
        _comprasHack.erase(persona);
    }
    _maxConsumidor[gastoDesactualizado].erase(persona); //Borra la persona del gasto desactualizado
    if(_maxConsumidor[gastoDesactualizado].size() == 0){ //Si el conjunto del gasto desactualizado queda vacio, borra la clave
        _maxConsumidor.erase(gastoDesactualizado);
    }
    if(_maxConsumidor.count(gastoActualizado)==1){ //Si el gasto esta definido en el dicc maxConsumidor, agrego la persona al conjunto
        _maxConsumidor[gastoActualizado].insert(persona);
    } else { //Si no esta definido el gasto, lo defino y agrego a la persona
        set<int> agregar;
        agregar.insert(persona);
        _maxConsumidor.insert(make_pair(gastoActualizado, agregar));
    }
}

int Lolla::gastoTotal(int persona) {  //O(log(A))
    return _gastoTotal[persona];
}

int Lolla::maxConsumidor() const { //O(1); crear los it al final es en O(1)
    map<int, set<int>>::const_iterator itGastos = _maxConsumidor.end(); //Inicializa un iterador al final de los gastos
    itGastos--; //Pone el iterador sobre el gasto maximo
    set<int>::const_iterator itPersonas = itGastos->second.end(); //Inicializa un iterador al final de las personas
    itPersonas--; //Pone el iterador sobre la persona con maximo id en el maximo de los gastos
    return *itPersonas;
}

int Lolla::puestoIDMenorStock(int item){   //O(P*log(I))
    map<int, Puesto>::iterator it = _idPuestos.begin(); //Inicializa un iterador
    int res = it->first; //Hace que res sea el puesto de menor ID
    int min = -1;
    if(it->second.stock().count(item)==1){ //Chequea si esta definido el item, si lo esta, cambia el minimo
        min = it->second.stock()[item];
    }
    for(pair<int, Puesto> p : _idPuestos){
        if(p.second.stock().count(item)==1){ //Si el elemento esta definido en algun puesto
            if(p.second.stock()[item]<min || min==(-1)){ //y el stock es menor o es el primer puesto que tiene el item, cambia el minimo y el id.
                res = p.first;
                min = p.second.stock()[item];
            }
        }
    }
    return res;
    //Como idPuestos esta ordenado por id de menor a mayor, si ningun puesto tiene el item
    //Nunca entra a los if y el puesto que devuelve es el de menor id
}

const set<int>& Lolla::infoPersonas() const { //O(1)
    return _personas;
}

map<int, Puesto>& Lolla::infoPuestos() {  //O(1)
    return _idPuestos;
}
