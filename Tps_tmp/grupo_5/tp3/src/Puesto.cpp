#include "Puesto.h"

Puesto::Puesto(Stock s, Menu m, Promociones promos): stock(s), menu(m), gastoPorPersona(), comprasPorPersona(), comprasSinDescuentoPorPersona(){
    map<Producto, vector<Nat>> desc; // O(1)
    // Se itera sobre los productos definidos en promos, y para cada uno se crea el vector de descuentos y posteriormente se guarda en diccionario de descuentos.
    for(auto itProductosPromos = promos.begin(); itProductosPromos != promos.end(); ++itProductosPromos){
        // Busco el descuento con la mayor cantidad mínima. Ej: Si hay descuento para 2 o + (9%), 4 o + (12%) y 6 o + (18%), maxCantMinima sería 6.
        Nat maxCantMinima = 0;
        for(auto itPromos = itProductosPromos->second.begin(); itPromos != itProductosPromos->second.end(); ++itPromos){
            if(itPromos->first > maxCantMinima){
                maxCantMinima = itPromos->first;
            }
        }
        // Creo el vector donde se van a almacenar los descuentos.
        vector<Nat> vecDescItem;
        // Y los guardo. Siguiendo el ejemplo de más arriba, el vector debería quedar <0,0,9,9,12,12,6>.
        Nat descTmp = 0;
        for(int i = 0; i <= maxCantMinima; i++){
            // Si hay descuento para la cantidad i, actualizo descTmp.
            if(itProductosPromos->second.count(i) == 1){
                descTmp = itProductosPromos->second.at(i);
            }
            // Inserto el porcentaje en el vector.
            vecDescItem.push_back(descTmp);
        }
        // Por último, defino el par <producto, vecDescItem> en el diccionario desc.
        desc.insert({itProductosPromos->first, vecDescItem});
    }
    descuentos = desc;
}
// Complejidad: θ(I * (log(I) + D' * D)), donde I = #claves(m) = #claves(s), D = máxima cantidad de descuentos que tiene un ítem y D' = máxima cantidad mínima requerida para aplicar un descuento que tiene un ítem.


Nat Puesto::porcentajeDescuento(Producto item, Nat cant) const{
    // Si el puesto no ofrece descuentos para el producto, se devuelve 0.
    if(descuentos.count(item) == 0){ // θ(log(I))
        return 0;
    }else{
        // Caso contrario, se busca qué descuento aplicable hay para la cantidad cant.
        // El primer caso es que estemos preguntando por una cantidad mayor que la maxima cantidad mínima que tiene un descuento. En este caso se devuelve el descuento guardado en la última posición del vector.
        if(cant > descuentos.at(item).size() - 1){ // θ(log(I))
            return descuentos.at(item).back(); // θ(log(I))
        }else{
            // Si no, simplemente se accede a la posición en el vector.
            return descuentos.at(item)[cant]; // θ(log(I))
        }
    }
}
// Complejidad: θ(log(I)) con I = cantidad de productos que tienen descuento (en peor caso I = cantidad de productos vendidos por el puesto).

bool Puesto::enMenu(Producto item) const{
    return menu.count(item) == 1;
}
// Complejidad: θ(log(I)) con I = cantidad de productos vendidos por el puesto.

Nat Puesto::stockProducto(Producto item) const{
    return stock.at(item);
}
// Complejidad: θ(log(I)) con I = cantidad de productos vendidos por el puesto.

Nat Puesto::cuantoGastoPersona(Persona per) const{
    // Si la persona nunca hizo una compra en el puesto, se devuelve 0.
    if(gastoPorPersona.count(per) == 0){ // θ(log(A))
        return 0;
    }else{
        // Caso contrario, se devuelve el gasto almacenado en el diccionario.
        return gastoPorPersona.at(per); // θ(log(A))
    }
}
// Complejidad: θ(log(A)), con A = cantidad de personas que realizaron una compra en el puesto.

Nat Puesto::Precio(Producto item) const{
    return menu.at(item);
}
// Complejidad: θ(log(I)) con I = cantidad de productos vendidos por el puesto.

void Puesto::Vender(Persona per, Producto item, Nat cant){
    // Primero, se calcula el nuevo gasto de la persona.
    Nat desc = porcentajeDescuento(item, cant); // θ(porcentajeDescuento) =  θ(log(I))
    Nat precio = Precio(item); //  θ(Precio) =  θ(log(I))
    Nat nuevoGasto = cuantoGastoPersona(per) + calcularDescuento(precio, cant, desc); // θ(cuantoGastoPersona) + θ(1) = θ(log(A))
    // Se actualiza el gasto.
    gastoPorPersona[per] = nuevoGasto; // θ(log(A))
    // Agrego la compra a la lista de compras de la persona. En caso que haya sido sin descuento, guardo el iterador a la compra en la cola de compras sin descuento de la persona.
    // Aclaración: El operador [] define la clave cuando no lo estaba, o devuelve una referencia al significado si estaba definida. Aprovechando esto, y que la lista o cola se definen vacías si es la primera vez que se define la clave,
    // no hace falta separar en casos (por ejemplo si la persona ya hizo una compra en el puesto, si no, si hizo una compra pero no sin descuento, etc) pues el código funciona bien.
    auto itCompra = comprasPorPersona[per][item].insert(comprasPorPersona[per][item].end(), cant); // θ(log(A) + log(I'))
    if(desc == 0){
        comprasSinDescuentoPorPersona[per][item].push(itCompra); // θ(log(A') + log(I'')).
    }
    // Por último, se actualiza el stock del puesto.
    stock[item]-= cant; // θ(log(I))
}
// I = cantidad de productos que vende el puesto.
// I' = cantidad de productos que compró per en el puesto.
// I'' = cantidad de productos que compró per en el puesto sin que se le haya aplicado descuento.
// A = cantidad de personas que realizaron alguna compra en el puesto.
// A' = cantidad de personas que realizaron alguna compra en el puesto en la cual no se le aplicó un descuento.
// En peor caso A = A' y I = I' = I''.
// Complejidad: θ(log(A) + log(I))


bool Puesto::comproItemSinDescuento(Persona per, Producto item) const{
    // Si la persona no está definida en comprasSinDescuentoPorPersona, o está definida pero no está definido el producto o el producto está definido pero la cola de iteradores a compras sin descuento es vacía, se devuelve false.
    if(comprasSinDescuentoPorPersona.count(per) == 0 or comprasSinDescuentoPorPersona.at(per).count(item) == 0 or comprasSinDescuentoPorPersona.at(per).at(item).empty()){ // θ(log(A') + log(I''))
        return false;
    }else{
        // Caso contrario: la persona sí hizo alguna compra sin descuento de producto, por lo que se devuelve true.
        return true;
    }
}
// A' = cantidad de personas que realizaron alguna compra en el puesto en la cual no se le aplicó un descuento.
// I'' = cantidad de productos que compró per en el puesto sin que se le haya aplicado descuento.
// En peor caso A' = A = cantidad de personas que hicieron alguna compra en el puesto e I'' = I = cantidad de productos que vende el puesto.
// Complejidad: θ(log(A) + log(I))

void Puesto::olvidarCompraSinDescuento(Persona per, Producto item){
    // Se modifica el gasto de la persona.
    gastoPorPersona[per] -= Precio(item); // θ(log(A)) + θ(Precio) = θ(log(A)) + θ(log(I)) = θ(log(A) + log(I))
    // Se modifica el stock.
    stock[item]++; // θ(log(I))
    // Se accede al primer elemento de la cola de iteradores y se revisa la cantidad comprada a la que apunta el iterador, si es mayor que 1 simplemente se modifica restándole 1,
    // caso contrario se elimina esa compra de la lista y se desencola el iterador.
    if(*(comprasSinDescuentoPorPersona[per][item].front()) > 1){ // θ(log(A')) + θ(log(I''))
        *(comprasSinDescuentoPorPersona[per][item].front()) = *(comprasSinDescuentoPorPersona[per][item].front()) - 1; // θ(log(A')) + θ(log(I''))
    }else{
        comprasPorPersona[per][item].erase(comprasSinDescuentoPorPersona[per][item].front(), comprasSinDescuentoPorPersona[per][item].front()); // θ(log(A)) + θ(log(I')) + θ(log(A')) + θ(log(I''))
        comprasSinDescuentoPorPersona[per][item].pop(); // θ(log(A')) + θ(log(I''))
    }
}
// I = cantidad de productos que vende el puesto.
// I' = cantidad de productos que compró per en el puesto.
// I'' = cantidad de productos que compró per en el puesto sin que se le haya aplicado descuento.
// A = cantidad de personas que realizaron alguna compra en el puesto.
// A' = cantidad de personas que realizaron alguna compra en el puesto en la cual no se le aplicó un descuento.
// En peor caso A = A' y I = I' = I''.
// Complejidad: θ(log(A) + log(I))

