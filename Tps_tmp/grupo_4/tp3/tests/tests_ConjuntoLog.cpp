//
// Created by santiago on 25/06/23.
//


#include "../src/ConjuntoLog.hpp"
#include "gtest-1.8.1/gtest.h"

TEST(ConjuntoLogaritmico, vacio){
    ConjLog<int> c = ConjLog<int>();

    EXPECT_EQ(c.esVacio(), true);
    EXPECT_EQ(c.pertenece(1), false);
}

TEST(ConjuntoLogaritmico, agregarElementos){
    ConjLog<int> c = ConjLog<int>();

    c.Agregar(1);
    c.Agregar(1);
    c.Agregar(3);
    c.Agregar(4);
    c.Agregar(6);

    EXPECT_EQ(c.esVacio(), false);
    EXPECT_EQ(c.pertenece(0), false);
    EXPECT_EQ(c.pertenece(1), true);
    EXPECT_EQ(c.pertenece(2), false);
    EXPECT_EQ(c.pertenece(3), true);
    EXPECT_EQ(c.pertenece(4), true);
    EXPECT_EQ(c.pertenece(5), false);
    EXPECT_EQ(c.pertenece(6), true);
    EXPECT_EQ(c.pertenece(7), false);
}

TEST(ConjuntoLogaritmico, eliminarElementos){
    ConjLog<int> c = ConjLog<int>();

    c.Agregar(1);
    c.Agregar(3);
    c.Agregar(4);
    c.Agregar(6);

    EXPECT_EQ(c.pertenece(1), true);
    c.eliminar(1);
    EXPECT_EQ(c.pertenece(1), false);
    EXPECT_EQ(c.esVacio(), false);

    EXPECT_EQ(c.pertenece(2), false);
    c.eliminar(2);
    EXPECT_EQ(c.pertenece(2), false);
    EXPECT_EQ(c.esVacio(), false);


    EXPECT_EQ(c.pertenece(3), true);
    c.eliminar(3);
    EXPECT_EQ(c.pertenece(3), false);
    EXPECT_EQ(c.esVacio(), false);

    EXPECT_EQ(c.pertenece(4), true);
    c.eliminar(4);
    EXPECT_EQ(c.pertenece(4), false);
    EXPECT_EQ(c.esVacio(), false);

    EXPECT_EQ(c.pertenece(6), true);
    c.eliminar(6);
    EXPECT_EQ(c.pertenece(6), false);
    EXPECT_EQ(c.esVacio(), true);
}

TEST(ConjuntoLogaritmico, minMax){
    ConjLog<int> c = ConjLog<int>();

    c.Agregar(1);
    c.Agregar(4);
    c.Agregar(3);
    c.Agregar(10);
    c.Agregar(-4);
    c.Agregar(6);
    EXPECT_EQ(c.min(), -4);
    EXPECT_EQ(c.max(), 10);

    c.eliminar(-4);
    c.eliminar(10);
    EXPECT_EQ(c.min(), 1);
    EXPECT_EQ(c.max(), 6);

    c.Agregar(20);
    c.Agregar(-10);
    EXPECT_EQ(c.min(), -10);
    EXPECT_EQ(c.max(), 20);
}

TEST(ConjuntoLogaritmico, crearAPartirDeSet) {
    set<int> cLineal1 = {10, 20, -7 , 1, 0, 14};
    ConjLog<int> c1 = ConjLog<int>(cLineal1);

    EXPECT_EQ(c1.esVacio(), false);
    EXPECT_EQ(c1.pertenece(10), true);
    EXPECT_EQ(c1.pertenece(20), true);
    EXPECT_EQ(c1.pertenece(-7), true);
    EXPECT_EQ(c1.pertenece(1), true);
    EXPECT_EQ(c1.pertenece(0), true);
    EXPECT_EQ(c1.pertenece(14), true);
    EXPECT_EQ(c1.pertenece(-1), false);
}


TEST(ConjuntoLogaritmico, structSimple) {
    struct testStruct{
        int n1;
        int n2;
        int n3;

        testStruct(const int n1, const int n2, const int n3){
            this->n1 = n1;
            this->n2 = n2;
            this->n3 = n3;
        }
        bool operator <(const testStruct& otro) const{
            return this->n2 < otro.n2 || (this->n2 == otro.n2 && this->n1 < otro.n1);
        }
        bool operator ==(const testStruct& otro) const{
            return this->n1 == otro.n1 && this->n2 == otro.n2 && this->n3 == otro.n3;
        }
    };

    ConjLog<testStruct> c = ConjLog<testStruct>();
    EXPECT_EQ(c.esVacio(), true);

    c.Agregar(testStruct(-1,2,5));
    c.Agregar(testStruct(1,3,1));
    c.Agregar(testStruct(2,3,2));
    c.Agregar(testStruct(2,1,-1));
    c.Agregar(testStruct(3,1,1));

    EXPECT_EQ(c.esVacio(), false);
    EXPECT_EQ(c.pertenece(testStruct(3,1,1)), true);
    EXPECT_EQ(c.pertenece(testStruct(3,3,3)), false);
    EXPECT_EQ(c.min(), testStruct(2,1,-1));
    EXPECT_EQ(c.max(), testStruct(2,3,2));

    c.eliminar(testStruct(2,1,-1));
    EXPECT_EQ(c.pertenece(testStruct(2,1,-1)), false);
    EXPECT_EQ(c.min(), testStruct(3,1,1));

    c.eliminar(testStruct(2,3,2));
    EXPECT_EQ(c.pertenece(testStruct(2,3,2)), false);
    EXPECT_EQ(c.max(), testStruct(1,3,1));
}



TEST(ConjuntoLogaritmico, structConIterador){
    struct testStruct{
        int n;
        set<int>::iterator it;

        testStruct(const int n, const set<int>::iterator& it){
            this->n = n;
            this->it = it;
        }
        bool operator<(const testStruct& otro) const{
            return this->n < otro.n;
        }
        bool operator==(const testStruct& otro) const{
            return this->n == otro.n && this->it == otro.it;
        }
    };


    set<int> cLineal = {10, 13, 4, 9, -17, -1, 2};
    set<int>::iterator it1 = cLineal.begin();
    set<int>::iterator it2 = cLineal.begin() ++ ++;
    set<int>::iterator it3 = cLineal.begin() ++ ++ ++ ;
    set<int>::iterator it4 = cLineal.end() --;
    set<int>::iterator it5 = cLineal.end() -- -- --;


    ConjLog<testStruct> c = ConjLog<testStruct>();
    EXPECT_EQ(c.esVacio(), true);

    c.Agregar(testStruct(3, it1));
    c.Agregar(testStruct(-31, it2));
    c.Agregar(testStruct(110, it3));
    c.Agregar(testStruct(64, it4));
    c.Agregar(testStruct(14, it4));
    c.Agregar(testStruct(-28, it5));

    EXPECT_EQ(c.esVacio(), false);
    EXPECT_EQ(c.pertenece(testStruct(64,it4)), true);
    EXPECT_EQ(c.pertenece(testStruct(63,it4)), false);
    EXPECT_EQ(c.pertenece(testStruct(1,it1)), false);
    EXPECT_EQ(c.min(), testStruct(-31, it2));
    EXPECT_EQ(c.max(), testStruct(110, it3));


    c.eliminar(testStruct(-31, it2));
    EXPECT_EQ(c.pertenece(testStruct(-31, it2)), false);
    EXPECT_EQ(c.min(), testStruct(-28, it5));

    c.eliminar(testStruct(110, it3));
    EXPECT_EQ(c.pertenece(testStruct(110, it3)), false);
    EXPECT_EQ(c.max(), testStruct(64, it4));
};


