//Grupo Chip
//Integrante                LU      Correo electrónico
//Beren, Manuel Andrés      320/22  m.beren83@gmail.com
//Carrillo, Mariano         358/22  carr.mariano@gmail.com
//Roitman, Sofía            563/21  sofiroit@gmail.com
//Valentini, Justo Agustin  566/22  justovalentini@gmail.com

#ifndef TP_LOLLA_LOLLA_H
#define TP_LOLLA_LOLLA_H
#include <map>
#include <set>
#include "Puesto.h"
#include "minHeap.h"

using namespace std;


class Lolla {
public:
    Lolla(map<int, Puesto> puestos, const set<int> personas);
    void comprar(Puesto& puesto, int persona, int item, int cant);
    void hackear(int persona, int item);
    int gastoTotal(int persona);
    int maxConsumidor() const;
    int puestoIDMenorStock(int item);
    const set<int>& infoPersonas() const;
    map<int, Puesto>& infoPuestos();


private:
    map<int, set<int>> _maxConsumidor;
    set<int> _personas;
    map<int, Puesto> _idPuestos;
    map<int, int> _gastoTotal;
    map<int, map<int, minHeap>> _comprasHack;

};


#endif //TP_LOLLA_LOLLA_H
