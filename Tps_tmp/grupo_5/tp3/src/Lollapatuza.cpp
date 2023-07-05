#include "Lollapatuza.h"

// Función para implementar la cola de prioridad de los gastos de las personas.
bool comparacionTuplasGasto(pair<Nat, Persona> A, pair<Nat, Persona> B) {
    return A.first > B.first or (A.first == B.first and B.second > A.second);
}

Lollapatuza::Lollapatuza(const Personas &pers, const Puestos &pues): personas(pers), puestos(), colaGastos(colaPriorTupla<Nat,Persona> (comparacionTuplasGasto)), gastoPorPersona(), IDpuestosDondeComproSinDescuento(){
    // Se crea cada puesto a partir del struct y se guarda el puesto y su id en el diccionario puestos.
    for(auto it = pues.begin(); it != pues.end(); ++it){
        puestos.insert({it->first, Puesto(it->second.stock, it->second.menu, it->second.promociones)});
    }
    // Se itera sobre el conjunto de personas, encolandolas en colaGastos y definiendolas en cuantoGastoCadaUna con gasto 0.
    for(Persona per: personas){
        gastoPorPersona[per] = 0;
        colaGastos.encolar(make_pair(0, per));
    }
}
// Complejidad: θ(∑p∈pues(crearPuesto(p)) + ∑i=1to#pers(log(i) + log(i)^2))

void Lollapatuza::registrarCompra(Persona per, IdPuesto id, Producto item, Nat cant) {
    // Se modifica el puesto realizando la venta.
    puestos.at(id).Vender(per, item, cant); // θ(log(P)) + θ(Vender) = θ(log(P)) + log(A') + log(I'))
    // Se actualiza la información del gasto de la persona y de la persona que más gastó.
    Nat descAplicable = puestos.at(id).porcentajeDescuento(item, cant); // θ(log(P)) + θ(porcentajeDescuento) = θ(log(P) + log(I'))
    Nat nuevoGasto = gastoPorPersona[per] + calcularDescuento(puestos.at(id).Precio(item), cant, descAplicable); // θ(log(A)) + θ(log(P)) + θ(Precio) = θ(log(A) + log(P) + log(I'))
    int pos = colaGastos.posicionEnCola({gastoPorPersona[per], per}); // θ(posicionEnCola) + θ(log(A)) = θ(log(A) + log(A)) = θ(log(A))
    colaGastos.eliminar(pos); // θ(eliminar) = θ(log(A)^2)
    gastoPorPersona[per] = nuevoGasto; // θ(log(A))
    colaGastos.encolar({nuevoGasto, per}); // θ(encolar) = θ(log(A)^2)
    // Si la compra es sin descuento, se actualiza IDpuestosDondeComproSinDescuento.
    if(descAplicable == 0){
        Puesto* p = &puestos.at(id); // θ(log(P))
        IDpuestosDondeComproSinDescuento[per][item][id] = p; // θ(log(A'') + log(I'') + log(P))
    }
}
// I' = cantidad de productos que vende el puesto, I'' = cantidad de productos que compró la persona sin que se le aplique un descuento, en peor caso I' = I'' = I = cantidad de productos que se venden en el festival.
// P' = cantidad de puestos donde la persona compró un producto sin descuento, en peor caso P' = P = cantidad de puestos en el festival.
// A' = cantidad de personas que compraron en el puesto, A'' = cantidad de personas que compraron sin descuento en el festival, en peor caso A' = A'' = A = cantidad de personas en el festival.
// Complejidad: θ(log(P) + log(I) + log(A)^2)

void Lollapatuza::Hackear(Persona per, Producto item){
    // Por defecto el iterador de la clase map recorre las claves en orden de menor a mayor, en base a la comparación < de las claves, que en este caso son enteros. Por lo tanto, si creo un iterador al diccionario <IdPuesto, Puesto*> de
    // map<Persona, map<Producto, map<IdPuesto, Puesto*>>>, voy a obtener el puesto a hackear (el de menor ID entre los que compró sin descuento).
    // Se actualiza la información del gasto de la persona y de la persona que más gastó.
    auto it = IDpuestosDondeComproSinDescuento[per][item].begin(); // θ(log(A) + log(I))
    Nat nuevoGasto = gastoPorPersona[per] - puestos.at(it->first).Precio(item); // θ(log(A)) + θ(log(P)) + θ(Precio) = θ(log(A) + log(P) + log(I))
    colaGastos.eliminar(colaGastos.posicionEnCola({gastoPorPersona[per], per})); // θ(eliminar) + θ(log(A)) = θ(log(A)^2 + log(A)) = θ(log(A)^2)
    gastoPorPersona[per] = nuevoGasto; // θ(log(A))
    colaGastos.encolar({nuevoGasto, per}); // θ(encolar) = θ(log(A)^2)
    // Se elimina el registro de la compra en el puesto.
    (*it->second).olvidarCompraSinDescuento(per, item); // θ(olvidarCompraSinDescuento) = θ(log(A) + log(I))
    // Y por último se revisa si el puesto deja de ser hackeable. En tal caso se elimina la clave en IDpuestosDondeComproSinDescuento.
    if(!(*it->second).comproItemSinDescuento(per, item)){ // θ(comproItemSinDescuento) = θ(log(A) + log(I))
        IDpuestosDondeComproSinDescuento[per][item].erase(it); // θ(log(P) + log(A) + log(I))
    }
}
// A = cantidad de personas en el festival
// I = cantidad de productos que se venden en el festival
// P = cantidad de puestos del festival
// Complejidad: θ(log(P) + log(A) + log(I) + log(A)^2)


Nat Lollapatuza::cuantoGasto(Persona per) const{
    return gastoPorPersona.at(per);
}
// Complejidad: θ(log(A)), con A = cantidad de personas en el festival.

Nat Lollapatuza::stockProductoPuesto(Producto item, IdPuesto id) const{
    return puestos.at(id).stockProducto(item);
}
// Complejidad: θ(log(P)) + θ(stockProducto) = θ(log(P) + log(I)) con P = cantidad de puestos del festival
// e I = cantidad de productos que vende el puesto (en peor caso vende todos los productos distintos que hay en el festival).

Persona Lollapatuza::laPersonaQueMasGasto() const{
    return colaGastos.proximo().second;
}
// Complejidad: θ(proximo) = θ(1)

bool Lollapatuza::comproProductoSinDescuento(Persona per, Producto item) const {
    for(auto itPuestos = puestos.begin(); itPuestos != puestos.end(); ++itPuestos){
        if(itPuestos->second.comproItemSinDescuento(per, item)){
            return true;
        }
    }
    return false;
}
// Se iteran todos los puestos y por cada uno se revisa si la persona compró el ítem sin descuento con costo θ(log(A) + log(I)) en peor caso.
// Complejidad: θ(P * (log(A) + log(I))), con P = cantidad de puestos, A = cantidad de personas e I = cantidad de productos distintos que se venden.

IdPuesto Lollapatuza::puestoConMenorStock(Producto item) const{
    list<pair<IdPuesto, Nat>> puestosQueLoVenden; // θ(1)
    if(loVendeAlgunPuestoConStock(item, puestosQueLoVenden)){ // θ(loVendeAlgunPuesto) = θ(P * log(I))
        // Se guarda temporalmente un id y un minimo stock del producto.
        auto it = puestosQueLoVenden.begin(); // θ(1)
        IdPuesto res_id = (*it).first; // θ(1)
        Nat min_stock = puestos.at(res_id).stockProducto(item); // θ(log(P)) + θ(stockProducto) = θ(log(P) + log(I))
        // Ahora se itera sobre el resto de ids de puestos que venden el producto, buscando el de menor stock y en caso de empate el de menor id.
        ++it; // θ(1)
        while(it != puestosQueLoVenden.end()){ // P' iteraciones
            if((*it).second < min_stock or ((*it).second == min_stock and (*it).first < res_id)){ // θ(1)
                min_stock = (*it).second; // θ(1)
                res_id = (*it).first; // θ(1)
            }
            ++it; // θ(1)
        }
        return res_id;
    }else{
        // Si no lo vende ningún puesto, devuelvo el menor id.
        // Por defecto el iterador de la clase map recorre las claves en orden de menor a mayor, en base a la comparación < de las claves, que en este caso son enteros.
        return puestos.begin()->first; // θ(1)
    }
}
// P = cantidad de puestos del festival, P' = cantidad de puestos que venden el producto, I = cantidad de productos que se venden en el festival.
// Complejidad:  θ((P * log(I)) + log(P) + log(I) +  P') =  θ(P * log(I))


bool Lollapatuza::loVendeAlgunPuestoConStock(Producto item, list<pair<IdPuesto, Nat>> &ids) const{
    list<pair<IdPuesto, Nat>> loVenden;
    bool res = false;
    // Itero sobre los puestos.
    for(auto it = puestos.begin(); it != puestos.end(); ++it){
        // Si el puesto vende el producto, lo agrego a loVenden y actualizo el booleano.
        if(it->second.enMenu(item)){ // θ(enMenu) = θ(log(I))
            loVenden.push_back(make_pair(it->first, it->second.stockProducto(item))); // θ(stockProducto) = θ(log(I))
            res = true;
        }
    }
    ids = loVenden; // θ(P)
    return res;
}
// Complejidad: θ(P * log(I)), con P = cantidad de puestos e I = cantidad de productos distintos que se venden.


const map<IdPuesto, Puesto>& Lollapatuza::puestosConIDS() const{
    return puestos;
}
// Complejidad: θ(1)

const set<Persona>& Lollapatuza::personasParticipantes() const {
    return personas;
}
// Complejidad: θ(1)