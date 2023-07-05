template<class T>
void ColaPrior<T>::_swap(vector<T>& v, int a, int b) {
    T temp = v[a];
    v[a] = v[b];
    v[b] = temp;
}

template<class T>
int ColaPrior<T>::_minIndex(const vector<T>& v, int a, int b) const {
    return v[a] < v[b] ? a : b;
}

template<class T>
void ColaPrior<T>::_siftUp(vector<T>& v, int pos) {
    while (pos > 0 && v[pos] < v[(pos-1)/2]) {
        _swap(v, pos, (pos-1)/2);
        pos = (pos-1)/2;
    }
}

template<class T>
void ColaPrior<T>::_siftDown(vector<T>& v, int pos) {
    //Arriba de el ultimo nivel y es menor que alguno de sus hijos.
    //Para el hijo de la derecha hay que chequear que exista.
    while (pos < v.size()/2 && (v[pos*2 + 1] < v[pos] || (pos*2 + 2 < v.size() && v[pos*2 + 2] < v[pos]))) {
        if (pos*2 + 2 == v.size() || v[pos*2 + 1] < v[pos*2 + 2]) {
            _swap(v, pos, pos*2 + 1);
            pos = pos*2 + 1;
        } else {
            _swap(v, pos, pos*2 + 2);
            pos = pos*2 + 2;
        }
    }
}

template<class T>
ColaPrior<T>::ColaPrior() : _cola() {}

template<class T>
int ColaPrior<T>::tam() const {
    return _cola.size();
}

template<class T>
void ColaPrior<T>::encolar(const T& elem) {
    _cola.push_back(elem);
    _siftUp(_cola, _cola.size() - 1);
//    int pos = _cola.size() - 1;
//    while (pos > 0 && _cola[(pos-1)/2] < _cola[pos]) {
//        _swap(_cola, pos, (pos-1)/2);
//        pos = (pos-1)/2;
//    }
}

template<class T>
const T& ColaPrior<T>::proximo() const {
    return _cola[0];
}

template<class T>
void ColaPrior<T>::desencolar() {
    _swap(_cola, 0, _cola.size()-1);
    _cola.pop_back();
    _siftDown(_cola, 0);
}

template<class T>
ColaPrior<T>::ColaPrior(const vector<T>& elems) : _cola(elems) {
    int pos = _cola.size()/2 - 1;
    while (pos >= 0) {
        _siftDown(_cola, pos);
        pos--;
    }
}

template<class T>
void ColaPrior<T>::siftUpTrack(int i, vector<int>& v, Nat canti){
    _cola[i].gasto += canti;
    while (i > 0 && _cola[i] < _cola[(i-1)/2]) {
        Persona actual = _cola[i].id;
        Persona old = _cola[(i-1)/2].id;
        v[old] = i;
        v[actual] = (i-1)/2;
        _swap(_cola, i, (i-1)/2);
        i = (i-1)/2;
    }
}

template<class T>
void ColaPrior<T>::siftDownTrack(int i, vector<int>& v, Nat canti){
    _cola[i].gasto -= canti;
    int izq = i*2 + 1;
    int der = i*2 + 2;
    while (i < _cola.size()/2) {
        Persona actual = _cola[i].id;
        if (der < _cola.size() && min(_cola[izq], _cola[der]) < _cola[i]) {
            int nuevaPos = _minIndex(_cola, izq, der);
            Persona old = _cola[nuevaPos].id;
            v[old] = i;
            v[actual] = nuevaPos;
            _swap(_cola, i, nuevaPos);
            i = nuevaPos;
            izq = i*2 + 1;
            der = i*2 + 2;
        } else if (_cola[izq] < _cola[i]) {
            Persona old = _cola[izq].id;
            v[old] = i;
            v[actual] = izq;
            _swap(_cola, i, izq);
            i = izq;
        } else {
            i = _cola.size()/2;
        }
    }
}