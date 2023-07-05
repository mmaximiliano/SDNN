#ifndef TP_LOLLA_COLAPRIOR_H
#define TP_LOLLA_COLAPRIOR_H

#include <vector>
#include "tipos.h"

using namespace std;

/* La clase T debe tener definido un operator<
 * que implemente una relación de orden total. */
template<class T>
class ColaPrior {
    public:
        ColaPrior();

        int tam() const;

        void encolar(const T& elem);

        const T& proximo() const;

        void desencolar();

        //Estas estan hechas solo para PerGasto, no deberian usarse con otro tipo
        void siftUpTrack(int i, vector<int>& v, Nat canti);
        void siftDownTrack(int i, vector<int>& v, Nat canti);

        //Heapify
        ColaPrior(const vector<T>& elems);

        //No hace falta destructor porque no hicimos nada con memoria
        //Tampoco hace falta constructor "=" o "Lollapatuza(Lollapatuza l)" porque los que vienen por defecto con c++ copian las
        //variables de private y en nuestro caso eso es suficiente.

    private:
        void _swap(vector<T>& v, int a, int b);
        int _minIndex(const vector<T>& v, int a, int b) const;
        void _siftUp(vector<T>& v, int pos);
        void _siftDown(vector<T>& v, int pos);
        vector<T> _cola;
};

#include "colaprior.hpp"

#endif //TP_LOLLA_COLAPRIOR_H
