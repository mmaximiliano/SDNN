#ifndef TP_LOLLA_PUESTO_H
#define TP_LOLLA_PUESTO_H
#include "tipos.h"


class Puesto{
public:
    Puesto(const map<Producto, Nat> precios, const Stock stock, const Promociones descuentos);
    //Complejidad: O(I * c + I * max(longitud del vector))

    Puesto();
    //Descripcion: inicializa nuestro puesto como vacío.
    //Justificación: lo necesitabamos para algunas declaraciones en nuestros diccionarios y en el constructor de FachadaLollapatuza.
    //Complejidad: O(1).

    Nat obtenerElStock(const Producto& item) const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un nat cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(log(I)).

    Nat obtenerDescuento(const Producto& item, Nat cantidad) const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un nat cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(log(I) + log(cant)).

    Nat obtenerGasto(Persona persona) const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un nat cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(log(A)).

    void vender(Persona persona, Producto item, Nat cantidad);
    //Aliasing: no cambia.
    //Complejidad: O(log(A) + log(I) + log(cant)).

    Nat obtenerPrecio(Producto item) const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un nat cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(log(I)).

    map<Producto, vector<Nat>> transformarACantidadesConDescuentos(const Promociones& descuentos) const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un vector la complejidad cambia.
    //Complejidad = O(I * c + I * max(longitud del vector)).

    Nat gastoDeVenta(Producto item, Nat cantidad) const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un nat cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(log(I) + log(cant)).

    void hackearPuesto(Persona persona, Producto item);
    //Aliasing: no cambia.
    //Complejidad: O(log(A) + log(I)).

    Nat cantidadConDescuento(Producto item, Nat cantidad) const;
    //Aclaracion: nos dimos cuenta que habiamos interpretado mal la especificacion.
    //Ahora se le aplica a todos los items el descuento que sea menor o igual a la cantidad comprada o no aplica dependiendo el caso.
    //Aliasing: lo devolvemos por copia y no por referencia, como es un nat cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(log(I) + log(cant)).

    map<Producto, Nat> obtenerPrecios() const;
    //Descripcion: devuelve el diccionario de precios de un puesto.
    //Justificacion: agregamos esta operación que en el diseño no estaba para poder acceder a la parte privada.
    //Aliasing: se devuelve por copia.
    //Complejidad: O(I).

    const map<Persona, map<Producto, list<pair<Producto, Nat>>::iterator>>& obtenerVentasSinDescuento() const;
    //Descripcion: devuelve el diccionario de ventas sin descuento de un puesto.
    //Justificacion: agregamos esta operación que en el diseño no estaba para poder acceder a la parte privada.
    //Aliasing: se devuelve por referencia no modificable.
    //Complejidad: O(1).

private:
    Stock StockPorItem;
    Promociones DescuentoPorCantidadPorItem;
    map<Producto, vector<Nat>> CantidadesConDescuentos;
    map<Persona, Nat> GastoPorPersona;
    map<Producto, Nat> Precios;
    map<Persona, list<pair<Producto, Nat>>> Ventas;
    map<Persona, map<Producto, list<pair<Producto, Nat>>::iterator>> VentasSinDescuento;

    };

#endif //TP_LOLLA_PUESTO_H