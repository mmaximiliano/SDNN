#ifndef TP_LOLLA_LOLLAPATUZA_H
#define TP_LOLLA_LOLLAPATUZA_H
#include "tipos.h"
#include "Puesto.h"

class Lollapatuza {
public:
    Lollapatuza(const map<IdPuesto, Puesto> puestos, const set<Persona> personas);
    //Complejidad: O(P * I * log(I) + P * copia puesto + A * log(A))
    //copiar puesto = O(I * CantidadDeDescuentos + I * max(longitud vector) + A * longListaMaxVentas + A * I)

    Lollapatuza();
    //Descripcion: inicializa nuestro lolla como vacío.
    //Justificación: lo necesitabamos para algunas declaraciones en nuestros diccionarios y en el constructor de FachadaLollapatuza.
    //Complejidad: O(1).

    void registrarCompra(Producto item, Nat cantidad, Persona persona, IdPuesto idPuesto);
    //Aliasing: no cambia.
    //Complejidad: O(log(I) + log(A) + log(P) + log(Cant)).

    void hackear(Producto item, Persona persona);
    //Aliasing: no cambia.
    //Complejidad: O(log(I) + log(P) + log(A))

    Nat obtenerGastoTotalDeUnaPersona(Persona persona) const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un nat cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(log(A)).

    Persona personaQueMasGasto() const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un int cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(1).

    IdPuesto puestoConMenorStockDeItem(Producto item) const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un int cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(P * I)

    const set<Persona>& obtenerPersonas() const;
    //Aliasing: lo devolvemos por referencia no modificable.
    //Complejidad: O(1).

    const map<IdPuesto, Puesto>& obtenerPuestos() const;
    //Aliasing: lo devolvemos por referencia no modificable.
    //Complejidad: O(1).

    map<Producto, Nat> preciosLolla(const map<IdPuesto, Puesto>& puestos) const;
    //Aliasing: lo devolvemos por copia y no por referencia.
    //Complejidad: O(P * I * log(I)).

    Persona personaConMenorId(const set<Persona>& personas) const;
    //Aliasing: lo devolvemos por copia y no por referencia, como es un int cuesta O(1) y no aumenta complejidad.
    //Complejidad: O(A).

    map<Persona, Nat> gastosEnCero(const set<Persona>& personas);
    //Aliasing: lo devolvemos por copia y no por referencia.
    //Complejidad: O(A * log(A)).

    map<Nat, map<Persona, Nat>> crearPersonaPorGasto(const set<Persona>& personas);
    //Aliasing: lo devolvemos por copia y no por referencia.
    //Complejidad: O(A * log(A)).

    set<IdPuesto> ObtenerClavesPuesto() const;
    //Descripcion: devuelve las claves del diccionario Puestos.
    //Justificacion: agregamos esta operación para la fachada.
    //Aliasing: lo devolvemos por copia y no por referencia.
    //Complejidad: O(P).


private:
    map<Producto,Nat> Precios;
    map<IdPuesto, Puesto> Puestos;
    set<Persona> Personas;
    Persona PersonaQueMasGasto;
    map<Persona, Nat> GastoTotalPorPersona;
    map<Producto, map<Persona, map<IdPuesto, Puesto*>>> HackeosPosibles;
    map<Nat, map<Persona, Nat >> PersonaPorGasto;
};


#endif //TP_LOLLA_LOLLAPATUZA_H