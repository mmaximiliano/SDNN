//Grupo Chip
//Integrante                LU      Correo electrónico
//Beren, Manuel Andrés      320/22  m.beren83@gmail.com
//Carrillo, Mariano         358/22  carr.mariano@gmail.com
//Roitman, Sofía            563/21  sofiroit@gmail.com
//Valentini, Justo Agustin  566/22  justovalentini@gmail.com

#ifndef TP_LOLLA_MINHEAP_H
#define TP_LOLLA_MINHEAP_H
#include "Puesto.h"
#include "vector"

using namespace std;


class minHeap {
public:
    minHeap();
    Puesto& proximo();
    void encolar(Puesto& p);
    void desencolar();
    int size();

private:
    vector<Puesto> _heap;


};


#endif //TP_LOLLA_MINHEAP_H

