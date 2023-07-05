//Grupo Chip
//Integrante                LU      Correo electrónico
//Beren, Manuel Andrés      320/22  m.beren83@gmail.com
//Carrillo, Mariano         358/22  carr.mariano@gmail.com
//Roitman, Sofía            563/21  sofiroit@gmail.com
//Valentini, Justo Agustin  566/22  justovalentini@gmail.com

#include "minHeap.h"

//puesto1 < puesto2 sii puesto1.id < puesto2.id

minHeap::minHeap(){ //O(1)
    _heap = vector<Puesto>();
}

Puesto& minHeap::proximo(){ //O(1)
    return _heap[0];
}

void minHeap::encolar(Puesto& p){ //O(log n)
        _heap.push_back(p);
        for (int i = _heap.size() - 1; _heap[(i - 1) / 2].id() > _heap[i].id() && i != 0; i = (i - 1) / 2) {
            _heap[i] = _heap[(i - 1) / 2];
            _heap[(i - 1) / 2] = p;
    }
}



void minHeap::desencolar(){ //O(log n)
    Puesto& p = _heap[_heap.size()-1];
    int i = 0;
    _heap[0] = p;
    _heap.pop_back();
    while((2*i+2<_heap.size()) && //Caso en el que 2*i+1 y 2*i+2 estan definidos
            (_heap[i].id()>_heap[2*i+1].id() || _heap[i].id()>_heap[2*i+2].id())){
        if(_heap[2*i+1].id()<_heap[2*i+2].id()){
            _heap[i] = _heap[2*i+1];
            _heap[2*i+1] = p;
            i = 2*i+1;
        } else {
            _heap[i] = _heap[2*i+2];
            _heap[2*i+2] = p;
            i = 2*i+2;
        }
    }
    if(2*i+1<_heap.size() && _heap[i].id()>_heap[2*i+1].id()){
        _heap[i] = _heap[2*i+1];
        _heap[2*i+1] = p;
    }
}

int minHeap::size(){ //O(1)
    return _heap.size();
}