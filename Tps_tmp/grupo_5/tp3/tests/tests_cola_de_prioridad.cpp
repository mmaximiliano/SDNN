#include "gtest-1.8.1/gtest.h"
#include "../src/colaPriorTupla.hpp"

using namespace std;

bool cmp(pair<int, int> A, pair<int, int> B) {
    return A.first > B.first or (A.first == B.first and B.second > A.second);
}

TEST(test_cola_de_prioridad, cola_de_prioridad_vacio){
    colaPriorTupla<int, int> cola (cmp);
    EXPECT_TRUE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),0);
}

TEST(test_cola_de_prioridad, cola_de_prioridad_un_elem){
    colaPriorTupla<int, int> cola (cmp);
    pair<int, int> elem = make_pair(1, 2);
    cola.encolar(elem);
    ASSERT_FALSE(cola.esVacia());
    ASSERT_TRUE(cola.encolado(elem));
    EXPECT_EQ(cola.tamano(),1);
    EXPECT_EQ(cola.proximo(), elem);
    EXPECT_EQ(cola.posicionEnCola(elem),0);

}

TEST(test_cola_de_prioridad,cola_de_prioridad_proximo){
    colaPriorTupla<int, int> cola (cmp);

    pair<int,int> elem1 = make_pair(1,2);
    pair<int,int> elem2 = make_pair(3,4);
    pair<int,int> elem3 = make_pair(5,6);
    pair<int,int> elem4 = make_pair(7,8);
    pair<int,int> elem5 = make_pair(9,10);

    cola.encolar(elem1);
    ASSERT_EQ(cola.proximo(),elem1);
    cola.encolar(elem2);
    ASSERT_EQ(cola.proximo(), elem2);
    cola.encolar(elem3);
    ASSERT_EQ(cola.proximo(), elem3);
    cola.encolar(elem4);
    ASSERT_EQ(cola.proximo(), elem4);
    cola.encolar(elem5);
    ASSERT_EQ(cola.proximo(), elem5);
}

TEST(test_cola_de_prioridad, cola_de_prioridad_encolar){
    colaPriorTupla<int, int> cola (cmp);

    pair<int,int> elem1 = make_pair(1,2);
    pair<int,int> elem2 = make_pair(3,4);
    pair<int,int> elem3 = make_pair(5,6);
    pair<int,int> elem4 = make_pair(7,8);
    pair<int,int> elem5 = make_pair(9,10);

    /*
                   0
               { (1,2) }

                       (1,2) - 0

*/
    // CASO 1
    cola.encolar(elem1);
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),1);
    ASSERT_TRUE(cola.encolado(elem1));
    ASSERT_EQ(cola.posicionEnCola(elem1),0);

    /*
                   0      1
               { (3,4), (1,2) }

                       (3,4) - 0
                       /
                    (1,2) - 1

*/
    // CASO 2
    cola.encolar(elem2);
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),2);
    ASSERT_TRUE(cola.encolado(elem2));
    ASSERT_EQ(cola.posicionEnCola(elem2),0);
    ASSERT_EQ(cola.posicionEnCola(elem1),1);

    /*
                   0      1      2
               { (5,6), (1,2), (3,4) }

                        (5,6) - 0
                       /     \
                1 - (1,2)    (3,4) - 2
*/
    // CASO 3
    cola.encolar(elem3);
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),3);
    ASSERT_TRUE(cola.encolado(elem3));
    ASSERT_EQ(cola.posicionEnCola(elem3),0);
    ASSERT_EQ(cola.posicionEnCola(elem2),2);
    ASSERT_EQ(cola.posicionEnCola(elem1),1);


    /*
                   0      1      2      3
               { (7,8), (5,6), (3,4), (1,2) }

                        (7,8) - 0
                       /     \
                1 - (5,6)    (3,4) - 2
                   /
            3 - (1,2)
    */
    // CASO 4
    cola.encolar(elem4);
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),4);
    ASSERT_TRUE(cola.encolado(elem4));
    ASSERT_EQ(cola.posicionEnCola(elem4),0);
    ASSERT_EQ(cola.posicionEnCola(elem3),1);
    ASSERT_EQ(cola.posicionEnCola(elem2),2);
    ASSERT_EQ(cola.posicionEnCola(elem1),3);

    /*
                   0       1      2      3      4
               { (9,10), (7,8), (3,4), (1,2), (5,6) }

                        (9,10) - 0
                       /     \
                1 - (7,8)    (3,4) - 2
                   /    \
            3 - (1,2)   (5,6) - 4
*/
    // CASO 5
    cola.encolar(elem5);
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),5);
    ASSERT_TRUE(cola.encolado(elem5));
    ASSERT_EQ(cola.posicionEnCola(elem5),0);
    ASSERT_EQ(cola.posicionEnCola(elem4),1);
    ASSERT_EQ(cola.posicionEnCola(elem3),4);
    ASSERT_EQ(cola.posicionEnCola(elem2),2);
    ASSERT_EQ(cola.posicionEnCola(elem1),3);
}

TEST(test_cola_de_prioridad, cola_de_prioridad_desencolar){
    colaPriorTupla<int, int> cola (cmp);

    pair<int,int> elem1 = make_pair(1,2);
    pair<int,int> elem2 = make_pair(3,4);
    pair<int,int> elem3 = make_pair(5,6);

    ASSERT_TRUE(cola.esVacia());

    cola.encolar(elem1);
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),1);
    ASSERT_TRUE(cola.encolado(elem1));
    ASSERT_EQ(cola.posicionEnCola(elem1),0);

    cola.desencolar();
    ASSERT_TRUE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),0);

    /*
               0      1      2
           { (5,6), (1,2), (3,4) }

                    (5,6) - 0
                   /     \
            1 - (1,2)    (3,4) - 2
*/
    cola.encolar(elem1);
    cola.encolar(elem2);
    cola.encolar(elem3);
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),3);
    ASSERT_TRUE(cola.encolado(elem1));
    ASSERT_TRUE(cola.encolado(elem2));
    ASSERT_TRUE(cola.encolado(elem3));
    ASSERT_EQ(cola.posicionEnCola(elem3),0);
    ASSERT_EQ(cola.posicionEnCola(elem2),2);
    ASSERT_EQ(cola.posicionEnCola(elem1),1);


    /*
           0      1
       { (3,4), (1,2) }

                (3,4) - 0
               /
        1 - (1,2)
*/

    cola.desencolar();
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),2);
    ASSERT_FALSE(cola.encolado(elem3));
    ASSERT_TRUE(cola.encolado(elem1));
    ASSERT_TRUE(cola.encolado(elem2));
    ASSERT_EQ(cola.posicionEnCola(elem2),0);
    ASSERT_EQ(cola.posicionEnCola(elem1),1);

    pair<int,int> elem4 = make_pair(7,8);
    pair<int,int> elem5 = make_pair(9,10);

    /*
        0       1      2      3      4
    { (9,10), (7,8), (3,4), (1,2), (5,6) }

                    (9,10) - 0
                   /     \
            1 - (7,8)    (3,4) - 2
               /    \
        3 - (1,2)   (5,6) - 4
*/

    cola.encolar(elem3);
    cola.encolar(elem4);
    cola.encolar(elem5);
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),5);
    ASSERT_TRUE(cola.encolado(elem3));
    ASSERT_TRUE(cola.encolado(elem4));
    ASSERT_TRUE(cola.encolado(elem5));
    ASSERT_EQ(cola.posicionEnCola(elem5),0);
    ASSERT_EQ(cola.posicionEnCola(elem4),1);
    ASSERT_EQ(cola.posicionEnCola(elem3),4);
    ASSERT_EQ(cola.posicionEnCola(elem2),2);
    ASSERT_EQ(cola.posicionEnCola(elem1),3);

    /*
           0      1      2      3
       { (7,8), (3,4), (1,2), (5,6) }

                    (7,8) - 0
                    /     \
             1 - (5,6)    (3,4) - 2
                 /
          3 - (1,2)
*/
    cola.desencolar();
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),4);
    ASSERT_TRUE(cola.encolado(elem3));
    ASSERT_TRUE(cola.encolado(elem4));
    ASSERT_FALSE(cola.encolado(elem5));
    ASSERT_EQ(cola.posicionEnCola(elem4),0);
    ASSERT_EQ(cola.posicionEnCola(elem3),1);
    ASSERT_EQ(cola.posicionEnCola(elem2),2);
    ASSERT_EQ(cola.posicionEnCola(elem1),3);

}

TEST(test_cola_de_prioridad, cola_de_prioridad_eliminar){
    colaPriorTupla<int, int> cola (cmp);

    pair<int,int> elem1 = make_pair(1,2);
    pair<int,int> elem2 = make_pair(3,4);
    pair<int,int> elem3 = make_pair(5,6);
    pair<int,int> elem4 = make_pair(7,8);
    pair<int,int> elem5 = make_pair(9,10);
    pair<int,int> elem6 = make_pair(11,6);
    pair<int,int> elem7 = make_pair(12,8);
    pair<int,int> elem8 = make_pair(4,10);

    cola.encolar(elem1);
    cola.encolar(elem2);
    cola.encolar(elem3);
    cola.encolar(elem4);
    cola.encolar(elem5);
    cola.encolar(elem6);
    cola.encolar(elem7);
    cola.encolar(elem8);

    /*
        0      1      2       3       4       5      6       7
   { (12,8), (7,8), (11,6), (4,10), (5,6) , (3,4), (9,10), (1,2) }

                         (12,8) - 0
                       /            \
              1 - (7,8)              (11,6) - 2
                /      \                /    \
        3 -  (4,10)   (5,6) - 4   5 - (3,4)   (9,10) - 6
             /
      7 - (1,2)
*/
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),8);
    ASSERT_TRUE(cola.encolado(elem1));
    ASSERT_TRUE(cola.encolado(elem2));
    ASSERT_TRUE(cola.encolado(elem3));
    ASSERT_TRUE(cola.encolado(elem4));
    ASSERT_TRUE(cola.encolado(elem5));
    ASSERT_TRUE(cola.encolado(elem6));
    ASSERT_TRUE(cola.encolado(elem7));
    ASSERT_TRUE(cola.encolado(elem8));
    ASSERT_EQ(cola.posicionEnCola(elem1),7);
    ASSERT_EQ(cola.posicionEnCola(elem2),5);
    ASSERT_EQ(cola.posicionEnCola(elem3),4);
    ASSERT_EQ(cola.posicionEnCola(elem4),1);
    ASSERT_EQ(cola.posicionEnCola(elem5),6);
    ASSERT_EQ(cola.posicionEnCola(elem6),2);
    ASSERT_EQ(cola.posicionEnCola(elem7),0);
    ASSERT_EQ(cola.posicionEnCola(elem8),3);

    cola.eliminar(7);
    /*
    0       1      2       3       4       5      6
{ (12,8), (7,8), (11,6), (4,10), (5,6) , (3,4), (9,10)}

                    (12,8) - 0
                   /       \
         1 - (7,8)          (11,6) - 2
          /    \                   /   \
    3 - (4,10)   (5,6) - 4    5 - (3,4)   (9,10) - 6
*/
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),7);
    ASSERT_FALSE(cola.encolado(elem1));
    ASSERT_TRUE(cola.encolado(elem2));
    ASSERT_TRUE(cola.encolado(elem3));
    ASSERT_TRUE(cola.encolado(elem4));
    ASSERT_TRUE(cola.encolado(elem5));
    ASSERT_TRUE(cola.encolado(elem6));
    ASSERT_TRUE(cola.encolado(elem7));
    ASSERT_TRUE(cola.encolado(elem8));
    ASSERT_EQ(cola.posicionEnCola(elem2),5);
    ASSERT_EQ(cola.posicionEnCola(elem3),4);
    ASSERT_EQ(cola.posicionEnCola(elem4),1);
    ASSERT_EQ(cola.posicionEnCola(elem5),6);
    ASSERT_EQ(cola.posicionEnCola(elem6),2);
    ASSERT_EQ(cola.posicionEnCola(elem7),0);
    ASSERT_EQ(cola.posicionEnCola(elem8),3);

    cola.eliminar(5);
    cola.eliminar(4);
    /*
            0       1      2       3       4
        { (12,8), (9,10), (11,6), (4,10), (7,8)}

                        (12,8) - 0
                       /       \
               1 - (9,10)       (11,6) - 2
                 /      \
        3 - (4,10)     (7,8) - 4
*/
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),5);
    ASSERT_FALSE(cola.encolado(elem1));
    ASSERT_FALSE(cola.encolado(elem2));
    ASSERT_FALSE(cola.encolado(elem3));
    ASSERT_TRUE(cola.encolado(elem4));
    ASSERT_TRUE(cola.encolado(elem5));
    ASSERT_TRUE(cola.encolado(elem6));
    ASSERT_TRUE(cola.encolado(elem7));
    ASSERT_TRUE(cola.encolado(elem8));
    ASSERT_EQ(cola.posicionEnCola(elem4),4);
    ASSERT_EQ(cola.posicionEnCola(elem5),1);
    ASSERT_EQ(cola.posicionEnCola(elem6),2);
    ASSERT_EQ(cola.posicionEnCola(elem7),0);
    ASSERT_EQ(cola.posicionEnCola(elem8),3);

    cola.eliminar(1);
    /*
    0       1      2       3
{ (12,8), (9,10), (11,6), (4,10)}

                (12,8) - 0
               /       \
       1 - (7,8)      (11,6) - 2
         /
3 - (4,10)
*/
    ASSERT_FALSE(cola.esVacia());
    EXPECT_EQ(cola.tamano(),4);
    ASSERT_FALSE(cola.encolado(elem1));
    ASSERT_FALSE(cola.encolado(elem2));
    ASSERT_FALSE(cola.encolado(elem3));
    ASSERT_TRUE(cola.encolado(elem4));
    ASSERT_FALSE(cola.encolado(elem5));
    ASSERT_TRUE(cola.encolado(elem6));
    ASSERT_TRUE(cola.encolado(elem7));
    ASSERT_TRUE(cola.encolado(elem8));
    ASSERT_EQ(cola.posicionEnCola(elem4),1);
    ASSERT_EQ(cola.posicionEnCola(elem6),2);
    ASSERT_EQ(cola.posicionEnCola(elem7),0);
    ASSERT_EQ(cola.posicionEnCola(elem8),3);
}



