//
// Created by Admin on 26/6/2023.
//

#include "lollapatuza.h"

template<typename T, typename S>
bool estaDefinido(const map<T, S>& m, T clave){
    return m.count(clave) == 1;
}

//O(A*log(A) + P*(I*(log(P)+log(I))))
Lollapatuza::Lollapatuza(map<IdPuesto, Puesto>& puestos, set<Persona> dnis) : _puestos(puestos) {

   _conjPersonas = new set<Persona>(dnis); //O(A)
    //Agrega la info inicial de las personas (gastoTotal y las compras)
    this->AgregarPersonas(*_conjPersonas); //O(A*log(A))

    //Agrega los items de los puestos al lolla
    this->AgregarPuestos(_puestos); //O(P*(I*(log(P)+log(I))))

    //Pone a todas las personas en gasto 0
    pair<Nat, set<Persona>> gastoEnCero = make_pair(0, *_conjPersonas);
    _gastos.insert(gastoEnCero); //O(A)

    //Como todos tienen el mismo gasto, se desempata por menor dni
    map<Nat, set<Persona>>::iterator itMaximoGastador = _gastos.begin();
    _maxGastador.it = itMaximoGastador;
    Persona maxGastador = *itMaximoGastador->second.begin();
    _maxGastador.id = maxGastador;
}

//O(A*log(A))
void Lollapatuza::AgregarPersonas(set<Persona> &dnis) {
    set<Persona> :: iterator it;
    for(it = dnis.begin(); it != dnis.end(); it++){ //O(A)
        info_persona info;
        info.gastoTotal = 0;
        map<Producto, nodoCompras> comprasPersona;
        info.compras = comprasPersona;
        pair<Persona, info_persona> persona_a_agregar = make_pair(*it, info);
        _personas.insert(persona_a_agregar); //O(log(A))
    } //O(A*log(A))

}

//O(P*(I*(log(P)+log(I))))
void Lollapatuza::AgregarPuestos(map<IdPuesto, Puesto> &puestos) {
    //iterador que apunta a puestos dentro del diccionario _puestos (Puestos en el lolla)
    itPuestos itAPuesto;
    //Empiezo a iterar sobre el map de puestos
    for(itAPuesto = puestos.begin(); itAPuesto != puestos.end(); itAPuesto++){ //O(P)
        //Todos los items que vende el puesto (solo el nombre)
        list<Producto> items_del_puesto = itAPuesto->second.devolverItems(); //O(I)
        //Iterador que apunta a los items del puesto
        list<Producto> :: iterator itItems = items_del_puesto.begin();

        //Por cada uno itero sobre sus items
        while(itItems != items_del_puesto.end()){ //O(I)
            //Si el item ya estaba en el lolla, agregas a ese diccionario el puesto nuevo que lo vende
            if(estaDefinido(_items, *itItems)){ //O(log(I))
                pair<IdPuesto , itPuestos> puesto_con_el_item = make_pair(itAPuesto->first, itAPuesto);
                //Agrego el puesto a la lista de puestos que venden ese item
                _items.at(*itItems).insert(puesto_con_el_item); //O(log(P))
            } else{
                //Si nadie lo vende, lo agrego a la lista de items disponibles en el lolla
                pair<Producto, map<IdPuesto, itPuestos>> item_a_insertar;
                item_a_insertar.first = *itItems;
                pair<IdPuesto, itPuestos> puesto_a_insertar = make_pair(itAPuesto->first, itAPuesto);
                item_a_insertar.second.insert(puesto_a_insertar); //O(log(P))
                _items.insert(item_a_insertar); //O(log(I))
            }
            itItems++;
        } //O(I*(log(P)+log(I)))
    } //O(P*(I*(log(P)+log(I))))
}

//O(log(A) + log(P) + log(I) + log(cant))
void Lollapatuza::venta(Producto item, Persona dni, IdPuesto id, Nat cant) {
    //iterador al puesto donde se compro
    auto itPuestoVenta = _puestos.find(id); //O(log(P))


    /** Manipulacion del stock*/
    //calcula el stock del item despues de hacerse la compra
    Nat stockNuevo = itPuestoVenta->second.stock(item) - cant; //O(log(I))
    // Actualiza el stock del item
    itPuestoVenta->second.cambiarStock(item, stockNuevo); //O(log(I))


    /** Manipulacion de lo gastado en la transaccion**/
    //En esta linea se busca el dto del item
    Nat Dto = itPuestoVenta->second.descuentoItem(item, cant); //O(log(I)+log(cant))
    Nat precioItem = itPuestoVenta->second.precioItem(item); //O(log(I))
    Nat gastado = Nat(precioItem*float((100-float(Dto))/100)*cant);

    //Actualiza lo relacionado a maxGastador y agrega a gastos
    this->actualizarGastosVenta(dni, gastado, itPuestoVenta); //O(log(A))

    /**Cambia la cantidad hackeable del producto para esta persona de ser necesario**/
    if (Dto == 0) {
        //Actualiza lo relacionado a hackear a la persona
        this->actulizarHackeableVenta(_personas[dni], item, id, cant, itPuestoVenta); //O(log(A) + log(P) + log(I))
    }
}

//O(log(A))
void Lollapatuza::actualizarGastosVenta(Persona dni, Nat gastado, itPuestos itPuestoVenta) {
    /**cambia los gastos de la persona en el puesto donde compro**/
    Nat gastoTotalEnPuesto = itPuestoVenta->second.cuantoGasto(dni) + gastado; //O(log(A))
    Nat gastoPrevio = _personas.find(dni)->second.gastoTotal; //O(log(A))
    _personas.find(dni)->second.gastoTotal += gastado; //O(log(A))
    Nat gastoTotal = _personas.find(dni)->second.gastoTotal; //O(log(A))
    itPuestoVenta->second.cambiarGastoDePersona(dni, gastoTotalEnPuesto); //O(log(A))


    /**Agrega a la persona a Gastos y modifica a maximo gastador segun corresponde**/
    auto itMax = _maxGastador.it;
    Persona dniMaxGastador = _maxGastador.id;
    if (itMax->first == gastoTotal){
        /**Inserto el gasto en caso de que halla gastado lo mismo que el maximo gastador**/
        itMax->second.insert(dni); //O(log(A))
        if (dni < dniMaxGastador){
            //Actualizo el dni del maximo Gastador de ser necesario
            _maxGastador.id = dni;
        }
    }else if(itMax->first < gastoTotal){
        /**Insrto el gasto en caso de que halla gastado mas de lo que gasto el maximo gastador**/
        /**incerto el nuevo gasto**/
        //Creo un conjunto vacio para ponerle adentro el dni parametro y luego agregarlo como significado del dicciionario de gastos
        set<Persona> conjConDNIparametro;
        //Agrego al dni al conjunto antes mencionado
        conjConDNIparametro.insert(dni); //O(log(A))
        pair<Nat, set<Persona>> DefinicionGastos = make_pair(gastoTotal, conjConDNIparametro);
        //Guardo el iterador al nodo para actualizar el it del maxGastador
        auto itNuevoMax = _gastos.insert(DefinicionGastos); //O(log(A))


        /**Actualizo la info de maxGastador**/
        _maxGastador.id = dni;
        _maxGastador.it = (itNuevoMax.first);
    }else{
        /**Insrto el gasto en caso de que halla gastado menos de lo que gasto el maximo gastador**/
        if (estaDefinido(_gastos, gastoTotal)){ //O(log(A))
            //Agrego el dni al conjunto de personas que le corresponde
            _gastos[gastoTotal].insert(dni); //O(log(A))
        }else{
            //Creo un conjunto vacio para ponerle adentro el dni parametro y luego agregarlo como significado del dicciionario de gastos
            set<Persona> conjConDNIparametro;
            //Agrego al dni al conjunto antes mencionado
            conjConDNIparametro.insert(dni); //O(log(A))
            pair<Nat, set<Persona>> DefinicionGastos = make_pair(gastoTotal, conjConDNIparametro);
            _gastos.insert(DefinicionGastos); //O(log(A))
        }
    }

    /**Borrar a la persona del nodo que ocupaba anteriormente o de quedar vacio el nodo borrar el nodo completo [por nodo me refiero a clave y singnificado]**/
    if(_gastos[gastoPrevio].size() == 1){
        _gastos.erase(gastoPrevio); //Borra el nodo porque la unica persona con ese gasto era el mismo
    }else{
        _gastos[gastoPrevio].erase(dni); //Borra el dni de la persona del conjunto en la definicion del diccionario porque ya no tiene mas ese gasto como clave y mantiene el nodo debido a que aun hay personas con ese gasto
    }
}

//O(Log(I) + Log(P) + Log(A))
void Lollapatuza::actulizarHackeableVenta(Lollapatuza::info_persona &persona, Producto item, IdPuesto idPuesto,Nat cantidad, itPuestos itPuestoVenta) {
    if (estaDefinido(persona.compras, item)){ //O(log(I)
        /**Se cambia o guarda la cantidad comprada a la cantidad a hackear del producto**/
        auto itCompra = persona.compras.find(item); //O(log(I))
        if (estaDefinido(itCompra->second.puestosAHackear, idPuesto)){//O(log(P))
            /**Se cambia la cantidad a hackear**/
            auto HackDelPuesto = itCompra->second.puestosAHackear.find(idPuesto); //O(log(P))
            HackDelPuesto->second.cantHackeable += cantidad;
        }else{
            /**Se agrega el puesto pues es la primera ves que se compra en este este producto sin descuento**/
            Hack significadoPuestosAHackear = Hack(itPuestoVenta, cantidad);
            auto itPosibleNuevoMinimoId = itCompra->second.puestosAHackear.insert(make_pair(idPuesto, significadoPuestosAHackear)); //O(log(P))
            IdPuesto idPuestoAHackear = itCompra->second.it_puestosAHackear->first;
            if(idPuestoAHackear > idPuesto){
                /**Se actualiza el iterador al puestoAHackear pues este ya no es el de menor id**/
                itCompra->second.it_puestosAHackear = itPosibleNuevoMinimoId.first;
            }
        }
    }else{
        /**se crea el struct Hack que se usa para poder generar el struct nodoCompras que esta en la siguiente linea, en este nodo se halla la cantidad de items que compro y se pueden hackear en este local y un iterador al local en cuestion**/
        Hack significadoPuestosAHackear = Hack(itPuestoVenta, cantidad);
        /**Se inserta en la info_persona del cliente el item con su cantidad disponible para ser hackeada dentro**/
        persona.compras.insert(make_pair(item, nodoCompras(idPuesto,significadoPuestosAHackear))); //O(log(I))
    }
}

//O(log(I) + log(A))
void Lollapatuza::hackear(Producto item, Persona dni) {
    // me devuelve un iterador apuntando a la clave/significado
    map<Persona,info_persona>::iterator iter= _personas.find(dni);  //O(log(A))
    //persona es un puntero a la informacion de la persona que voy a hackear
    info_persona* persona = &(iter->second); //O(1)
    //gastoPrevio es el gasto de la persona antes de ser hackeada
    Nat gastoPrevio = persona->gastoTotal;//O(1)
    //iterador a las compras de la persona a hackear
    map<Producto, nodoCompras>::iterator it_comprasDePersona = persona->compras.find(item);  //O(log(I))

    /*puestosHackeables es el diccionario de puestos en los que se haya comprado el item sin descuento y
    un iterador al puesto con menor id entre ellos*/
    nodoCompras* puestosHackeables = &(it_comprasDePersona->second); //O(1)
    //baja la cantidad de items comprados sin descuento en el puesto a hackear
    puestosHackeables->it_puestosAHackear->second.cantHackeable--; //O(1)
    //iterador al puesto que se esta hackeando
    itPuestos it_puestoHackeado = puestosHackeables->it_puestosAHackear->second.it_puesto; //O(1)

    //actualiza aquello relacionado a gasto y stock, y devuelve el iterador al nodo de gastos donde estaba la persona antes de ser hackeada
    map<Nat, set<Persona>>::iterator  nodoGastoPrevio = hackearGastosyStock(*persona, item, dni, it_puestoHackeado); // O(Log(A) + Log(I))

    if(dni == _maxGastador.id){
        hackearMaximoGastador(nodoGastoPrevio); //O(1)
    }
    // si la cantidad hackeable pasa a ser 0, tengo que eliminarlo de puesto a hackear
    if ((puestosHackeables->it_puestosAHackear)->second.cantHackeable == 0){   
        puestosHackeables->puestosAHackear.erase(puestosHackeables->it_puestosAHackear); //O(1) porque el puesto ya estaba apuntado por un iterador
        puestosHackeables->it_puestosAHackear = puestosHackeables->puestosAHackear.begin(); //O(1)
    }

    //si el nodo anterior en gastos solo tenia a la persona a hackear, se lo borra, sino se borra nada mas a la persona del nodo
    if (nodoGastoPrevio->second.size() == 1){
        _gastos.erase(gastoPrevio);// O(log(|claves(_gastos)|) acotado por log(A) porque tengo que buscar entre todas las claves el gastoprevio
    }
    else {
        nodoGastoPrevio->second.erase(dni); //O(1)
    }
}

// O(Log(I) + O(Log(A))
map<Nat, set<Persona>>::iterator  Lollapatuza::hackearGastosyStock(Lollapatuza::info_persona &persona, Producto item, Persona dni, itPuestos it_puestoHackeado) {
    //repone el stock en el puesto hackeado
    Nat stockCambiado = it_puestoHackeado->second.stock(item) + 1; //O(Log(I))
    it_puestoHackeado->second.cambiarStock(item, stockCambiado); //O(Log(I))

    //cambia el gasto de la persona en el puesto hackeado
    Nat precioItem = it_puestoHackeado->second.precioItem(item); // O(Log(I))
    Nat nuevoGasto = it_puestoHackeado->second.cuantoGasto(dni) - precioItem; // O(Log(A))
    it_puestoHackeado->second.cambiarGastoDePersona(dni, nuevoGasto); // O(Log(A))

    //si la persona ahora no gasto nada, la elimina de los clientes del puesto
    if (nuevoGasto == 0) {
        it_puestoHackeado->second.eliminarAPersona(dni); // (Log(A))
    }
    //Guarda el gastoTotal de la persona antes de ser hackeada y luego lo actualiza
    Nat gastoTotalPrevio = persona.gastoTotal; //O(1)
    persona.gastoTotal -= precioItem; //O(1)

    //Agrega el gasto total de la persona, despues de ser hackeado, al diccionario de gastos del lolla
    Nat nuevoGastoTotal = persona.gastoTotal; // O(1)
    if (estaDefinido(_gastos, nuevoGastoTotal)) { //O(Log(|Claves(_gastos)|)), acotado por log(A) porque en el peor caso todos gastaron distintos y habria un nodo para cada persona
        map<Nat, set<Persona>>::iterator gasto = _gastos.find(nuevoGastoTotal);//log(A)
        gasto->second.insert(dni); // (Log(A))
    } else {
        _gastos[nuevoGastoTotal].insert(dni); // Log(A) es asi porque el conjunto donde se agrega a la persona esta vacio
    }

    map<Nat, set<Persona>>::iterator  nodoGastoPrevio = _gastos.find(gastoTotalPrevio); // O(Log(A))
    return nodoGastoPrevio; //O(1)
}

//O(1)
void Lollapatuza::hackearMaximoGastador(map<Nat, set<Persona>>::iterator nodoGastoPrevio) {
    //si no habia nadie mas con el max gasto, entonces se busca el siguiente gasto mayor y se desempata por menor dni
    //sino, se busca el maximo gastador en el mismo nodo en el que estaba antes la persona hackeada
    if (nodoGastoPrevio->second.size() == 1) {
        nodoGastoPrevio--; //O(1)
        _maxGastador.id = *((nodoGastoPrevio->second).begin()); //O(1)
        _maxGastador.it = nodoGastoPrevio; // O(1)
    } else {
        auto MismoGtoPers = nodoGastoPrevio->second.begin(); //O(1)
        MismoGtoPers++; //O(1)
        _maxGastador.id = *MismoGtoPers; //O(1)
    }

}

//O(log(A))
Nat Lollapatuza::gastoDePersona(Persona dni) const{
    return _personas.at(dni).gastoTotal;
}

//O(1)
Persona Lollapatuza::mayorGastador() const{
    return _maxGastador.id;
}

//O(P+log(I))
IdPuesto Lollapatuza::puestoConMenorStock(Producto item) const{

    const map<IdPuesto, itPuestos>& puestosConItem = _items.at(item); //O(log(I))
    auto itPuestoActual = puestosConItem.begin();

    //Variables del ciclo
    auto itMenosStock = itPuestoActual;
    auto puestoMenosStock = itMenosStock->second;

    //Recorro el diccionario completo de puestos para encontrar al que tiene el menor stock del item recibido por copia
    while(itPuestoActual != puestosConItem.end()){ //O(P)
        auto puestoActual = itPuestoActual->second;

        //Comparo el stock del puesto actual con el menor hasta el momento y, en caso del primero ser menor, lo reasigno
        Nat stockActual = puestoActual->second.stock(item);

        Nat stockMenor = puestoMenosStock->second.stock(item);

        if (stockActual < stockMenor){
            itMenosStock = itPuestoActual;
            puestoMenosStock = itMenosStock->second;
        }

        itPuestoActual++;
    }
    //itPuestos es iterador de  map<IdPuesto,Puesto>
    return itMenosStock->first;
}

//O(1)
set<Persona>* Lollapatuza::personas() const{
    return _conjPersonas;
}

//O(1)
const map<IdPuesto, Puesto>* Lollapatuza::puestos() const{
    const map<IdPuesto, Puesto>* p  = &_puestos;
    return p;
}
