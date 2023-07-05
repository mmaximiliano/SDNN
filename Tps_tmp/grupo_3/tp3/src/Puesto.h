//Grupo Chip
//Integrante                LU      Correo electrónico
//Beren, Manuel Andrés      320/22  m.beren83@gmail.com
//Carrillo, Mariano         358/22  carr.mariano@gmail.com
//Roitman, Sofía            563/21  sofiroit@gmail.com
//Valentini, Justo Agustin  566/22  justovalentini@gmail.com

#ifndef TP_LOLLA_PUESTO_H
#define TP_LOLLA_PUESTO_H
#include <map>
#include <set>

using namespace std;


class Puesto {
public:
    Puesto(map<int, int>& precios, map<int, int>& stock, map<int, map<int, int>>& descuentos, int& id);
    map<int, int>& stock();
    int obtenerDescuento(int& item, int& cant);
    int& obtenerGasto(int& persona);
    const int id();
    map<int, int> precios();
    map<int, map<int, int>>& sinDesc();
    map<int, int>& ventas();
    Puesto& operator=(const Puesto& p);

private:
    int _id;
    map<int, int> _stock;
    map<int, map<int, int>> _descuentos;
    map<int, int> _precios;
    map<int, int> _ventas;
    //Decidimos cambiar la estructura porque no es necesario registrar las compras hechas con descuento.
    map<int, map<int, int>> _sinDesc; // Esto es diccLog(persona, diccLog(item, cant))
};


#endif //TP_LOLLA_PUESTO_H
