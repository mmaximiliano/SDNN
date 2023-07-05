//
// Created by magal on 6/25/2023.
//

#include <map>
#include <set>


#ifndef TP_LOLLA_SETLOG_H
#define TP_LOLLA_SETLOG_H

using namespace std;

template <class T>

class ConjLog{
public:

    ConjLog();
    ConjLog(const set<T>& c);
    void Agregar(const T a);
    bool esVacio() const;
    bool pertenece(const T& a) const;
    void eliminar(const T& a);
    const T min() const;
    const T max() const;

private:
    map<T, bool> _map;
};




// O(1)
template <class T>
ConjLog<T>::ConjLog() {
    _map = map<T,bool>();
}

// O(n log(n))
template <class T>
ConjLog<T>::ConjLog(const set<T>& c) {
    _map = map<T,bool>();
    for(const T& e: c){
        _map[e] = false;
    }
}


// O(log(n))
template <class T>
void ConjLog<T>::Agregar(const T a){
    _map[a] = false;
}


// O(1)
template <class T>
bool ConjLog<T>::esVacio() const {
    return _map.empty();
}

// O(log n)
template <class T>
bool ConjLog<T>::pertenece(const T &a)  const {
    typename map<T,bool>::const_iterator it = _map.find(a);
    return it != _map.end();
}

// O(log n)
template <class T>
void ConjLog<T>::eliminar(const T &a) {
    typename map<T,bool>::iterator it = _map.find(a);
    if(it != _map.end())
        _map.erase(it);
}


// O(1)
template <class T>
const T ConjLog<T>::min() const{
    typename map<T,bool>::const_iterator itMinimo = _map.begin();
    const pair<const T,bool> claveSignificadoMinimo = *(itMinimo);
    return claveSignificadoMinimo.first;
}

template <class T>
const T ConjLog<T>::max() const{
    typename map<T,bool>::const_iterator itMaximo = _map.end();
    itMaximo--;
    const pair<const T,bool> claveSignificadoMaximo = *(itMaximo);
    return claveSignificadoMaximo.first;
}




#endif //TP_LOLLA_SETLOG_H