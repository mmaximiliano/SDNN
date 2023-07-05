#include "../src/Puesto.h"
#include "gtest-1.8.1/gtest.h"

using  namespace std;

class tests_puesto : public testing::Test {
protected:
    Menu menu;
    set<Persona> personas1, personas2, personas3;
    Stock stock01, stock02, stock03, stock04;
    Promociones  descuentos01, descuentos02, descuentos03, descuentos04;

    void SetUp() {
        menu = {{3,500},{4,1000},{5,2500},{7,2000}};
        personas1 = {11,12,13};
        stock01 = {{3, 10}, {4, 5}, {5, 30}, {7, 10}};
        stock02 = {{3, 1}, {4, 1}, {5, 1}, {7, 40}};
        stock03 = {{3, 20}, {4, 20}, {5, 20}, {7, 20}};
        descuentos01 = {{7, {{3, 20}, {6, 25}}}};
        descuentos02 = {{4, {{4, 15}}}};
        descuentos03 = {{3, {{5, 30}}}, {7, {{2, 10}}}};
    }
};

TEST_F(tests_puesto, abrir_Puesto){

    Puesto p1(stock01,menu,descuentos01);

//                 PUESTO 1
// +----------+------+-------+-------------------+
// | Producto | Menu | Stock |    Descuentos     |
// +----------+------+-------+-------------------+
// |        3 |  500 |    10 | -                 |
// |        4 | 1000 |     5 | -                 |
// |        5 | 2500 |    30 | -                 |
// |        7 | 2000 |    10 | 3u. x 20 , 6u. 25 |
// +----------+------+-------+-------------------+

    for(auto m : menu){
        EXPECT_TRUE(p1.enMenu(m.first));
    }
    for(auto m : menu){
        EXPECT_EQ(p1.Precio(m.first),m.second);
    }
    for(auto s : stock01){
        EXPECT_EQ(p1.stockProducto(s.first),s.second);
    }
    EXPECT_EQ(p1.porcentajeDescuento(3,6),0);
    EXPECT_EQ(p1.porcentajeDescuento(4,3),0);
    EXPECT_EQ(p1.porcentajeDescuento(5,6),0);
    EXPECT_EQ(p1.porcentajeDescuento(7,3),20);
    EXPECT_EQ(p1.porcentajeDescuento(7,6),25);

    for(int per : personas1){
        EXPECT_EQ(p1.cuantoGastoPersona(per),0);
        for(auto i: menu){
            EXPECT_FALSE(p1.comproItemSinDescuento(per,i.first));
        }
    }
}

TEST_F(tests_puesto,vender_sin_descuento){

// COMPRA DE UN SOLO ITEM DE UNA PERSONA

    Puesto p1(stock01,menu,descuentos01);
    p1.Vender(11,3,4);

//                 PUESTO 1
// +----------+------+-------+-------------------+
// | Producto | Menu | Stock |    Descuentos     |
// +----------+------+-------+-------------------+
// |        3 |  500 |     6 | -                 |
// |        4 | 1000 |     5 | -                 |
// |        5 | 2500 |    30 | -                 |
// |        7 | 2000 |    10 | 3u. x 20 , 6u. 25 |
// +----------+------+-------+-------------------+

//               PERSONA 11
// +------+----------+-------+---------------+
// | Item | Cantidad | Gasto | ConDescuento? |
// +------+----------+-------+---------------+
// |    3 |        4 |  2000 | false         |
// +------+----------+-------+---------------+

    EXPECT_EQ(p1.cuantoGastoPersona(11),2000);
    EXPECT_EQ(p1.cuantoGastoPersona(12),0);
    EXPECT_EQ(p1.cuantoGastoPersona(13),0);
    EXPECT_TRUE(p1.comproItemSinDescuento(11,3));
    EXPECT_EQ(p1.stockProducto(3),6);
    while(p1.cuantoGastoPersona(11) != 0){
        p1.olvidarCompraSinDescuento(11,3);
    }
    for(auto i: menu){
        EXPECT_FALSE(p1.comproItemSinDescuento(11,i.first));
    }
    EXPECT_EQ(p1.cuantoGastoPersona(11),0);
    EXPECT_EQ(p1.stockProducto(3),10);


// COMPRAS SOBRE EL MISMO ITEM PERO CON DISTINTAS PERSONAS

    p1.Vender(11,3,1);
    p1.Vender(12,3,3);
    p1.Vender(13,3,6);

    //             PUESTO 1
// +----------+------+-------+-------------------+
// | Producto | Menu | Stock |    Descuentos     |
// +----------+------+-------+-------------------+
// |        3 |  500 |     0 | -                 |
// |        4 | 1000 |     5 | -                 |
// |        5 | 2500 |    30 | -                 |
// |        7 | 2000 |    10 | 3u. x 20 , 6u. 25 |
// +----------+------+-------+-------------------+

//                     COMPRAS
// +----------+------+----------+-------+---------------+
// | Personas | Item | Cantidad | Gasto | ConDescuento? |
// +----------+------+----------+-------+---------------+
// |       11 |    3 |        1 |   500 | false         |
// |       12 |    3 |        3 |  4500 | false         |
// |       13 |    3 |        6 |  3000 | false         |
// +----------+------+----------+-------+---------------+

    EXPECT_EQ(p1.cuantoGastoPersona(11),500);
    EXPECT_EQ(p1.cuantoGastoPersona(12),1500);
    EXPECT_EQ(p1.cuantoGastoPersona(13),3000);
    for(int per: personas1){
        EXPECT_TRUE(p1.comproItemSinDescuento(per,3));
    }
    EXPECT_EQ(p1.stockProducto(3),0);
    while(p1.cuantoGastoPersona(11) != 0){
        p1.olvidarCompraSinDescuento(11,3);
    }
    EXPECT_EQ(p1.stockProducto(3),1);
    while(p1.cuantoGastoPersona(12) != 0){
        p1.olvidarCompraSinDescuento(12,3);
    }
    EXPECT_EQ(p1.stockProducto(3),4);
    while(p1.cuantoGastoPersona(13) != 0){
        p1.olvidarCompraSinDescuento(13,3);
    }

    EXPECT_EQ(p1.cuantoGastoPersona(11),0);
    EXPECT_EQ(p1.cuantoGastoPersona(12),0);
    EXPECT_EQ(p1.cuantoGastoPersona(13),0);
    EXPECT_EQ(p1.stockProducto(3),10);

// COMPRA DE DISTINTOS ITEMS POR UNA SOLA PERSONA

    p1.Vender(11,3,1);
    p1.Vender(11,4,2);
    p1.Vender(11,5,3);

//                PUESTO 1
// +----------+------+-------+-------------------+
// | Producto | Menu | Stock |    Descuentos     |
// +----------+------+-------+-------------------+
// |        3 |  500 |     9 | -                 |
// |        4 | 1000 |     3 | -                 |
// |        5 | 2500 |    27 | -                 |
// |        7 | 2000 |    10 | 3u. x 20 , 6u. 25 |
// +----------+------+-------+-------------------+

//                     COMPRAS
// +----------+------+----------+-------+---------------+
// | Personas | Item | Cantidad | Gasto | ConDescuento? |
// +----------+------+----------+-------+---------------+
// |       11 |    3 |        1 |   500 | false         |
// |       11 |    4 |        2 |  2000 | false         |
// |       11 |    5 |        3 |  7500 | false         |
// +----------+------+----------+-------+---------------+

    EXPECT_EQ(p1.cuantoGastoPersona(11),10000);
    EXPECT_EQ(p1.cuantoGastoPersona(12),0);
    EXPECT_EQ(p1.cuantoGastoPersona(13),0);
    EXPECT_EQ(p1.stockProducto(3),9);
    EXPECT_EQ(p1.stockProducto(4),3);
    EXPECT_EQ(p1.stockProducto(5),27);
    while(p1.cuantoGastoPersona(11) != 9500){
        p1.olvidarCompraSinDescuento(11,3);
    }
    while(p1.cuantoGastoPersona(11) != 7500){
        p1.olvidarCompraSinDescuento(11,4);
    }
    while(p1.cuantoGastoPersona(11) != 0){
        p1.olvidarCompraSinDescuento(11,5);
    }
    EXPECT_EQ(p1.stockProducto(3),10);
    EXPECT_EQ(p1.stockProducto(4),5);
    EXPECT_EQ(p1.stockProducto(5),30);
}

TEST_F(tests_puesto,vender_con_descuento) {

    Puesto p1(stock03, menu, descuentos01);
    p1.Vender(11, 7, 3);

    //                 PUESTO 1
// +----------+------+-------+-------------------+
// | Producto | Menu | Stock |    Descuentos     |
// +----------+------+-------+-------------------+
// |        3 |  500 |    20 | -                 |
// |        4 | 1000 |    20 | -                 |
// |        5 | 2500 |    20 | -                 |
// |        7 | 2000 |    17 | 3u. x 20 , 6u. 25 |
// +----------+------+-------+-------------------+

//                            GASTOS
// +----------+------+----------+--------------------------+---------------+
// | Personas | Item | Cantidad |          Gasto           | ConDescuento? |
// +----------+------+----------+--------------------------+---------------+
// |       11 |    7 |        3 | 4800 = (6000-(6000*0.2)) | true          |
// +----------+------+----------+--------------------------+---------------+

    EXPECT_EQ(p1.stockProducto(7),17);
    EXPECT_EQ(p1.cuantoGastoPersona(11),4800);
    EXPECT_FALSE(p1.comproItemSinDescuento(11,7));

// MULTIPLES COMPRAS CON DESCUENTO

    p1.Vender(11, 7, 3);
    p1.Vender(11, 7, 3);

    //                          GASTOS
// +----------+------+----------+--------------------------+----------------+--------+
// | Personas | Item | Cantidad |          Gasto           | ConDescuento?  | Total  |
// +----------+------+----------+--------------------------+----------------+--------+
// |       11 |    7 |        3 | 4800 = (6000-(6000*0.2)) | true           |   4800 |
// |       11 |    7 |        3 | 4800 = (6000-(6000*0.2)) | true           |   9600 |
// |       11 |    7 |        3 | 4800 = (6000-(6000*0.2)) | true           |  14400 |
// +----------+------+----------+--------------------------+----------------+--------+

    EXPECT_EQ(p1.stockProducto(7),11);
    EXPECT_EQ(p1.cuantoGastoPersona(11),14400);
    EXPECT_FALSE(p1.comproItemSinDescuento(11,7));

//   COMPRA DEL MISMO ITEM CON DESCUENTO POR ENCIMA DEL MAYOR DESCUENTO

    Puesto p2(stock02, menu, descuentos01);
    p2.Vender(11,7,13);

//                 PUESTO 2
// +----------+------+-------+-------------------+
// | Producto | Menu | Stock |    Descuentos     |
// +----------+------+-------+-------------------+
// |        3 |  500 |     5 | -                 |
// |        4 | 1000 |     5 | -                 |
// |        5 | 2500 |     5 | -                 |
// |        7 | 2000 |    27 | 3u. x 20 , 6u. 25 |
// +----------+------+-------+-------------------+

//                              GASTOS
// +----------+------+----------+---------------------------+---------------+
// | Personas | Item | Cantidad |           Gasto           | ConDescuento? |
// +----------+------+----------+---------------------------+---------------+
// |       11 |    7 |       13 | 19500= 26000-(26000*0.25) | true          |
// +----------+------+----------+---------------------------+---------------+

    EXPECT_EQ(p2.stockProducto(7),27);
    EXPECT_EQ(p2.cuantoGastoPersona(11),19500);
    EXPECT_FALSE(p2.comproItemSinDescuento(11,7));

}