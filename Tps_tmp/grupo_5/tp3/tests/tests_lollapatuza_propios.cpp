#include "../src/fachada_lollapatuza.h"
#include "gtest-1.8.1/gtest.h"
using namespace std;

class tests_lolla : public testing::Test {
protected:
    Menu menu;
    set<Persona> personas, personas1, personas2, personas3, personas330k; 
    set<IdPuesto> idsPuestos;
    Stock stock32, stock33, stock34, stock35, stock36, stock37, stock38;
    Promociones descuentos32, descuentos33, descuentos34, descuentosEmpty;
    aed2_Puesto puesto32, puesto33, puesto34, puesto35, puesto36, puesto37, puesto38;
    map<IdPuesto, aed2_Puesto> puestos, puestos2, puestos3;

    void SetUp() {
        menu = {{0, 650}, {10, 200}, {5, 1250}, {12, 300}, {7, 2000}};
        personas = {1, 2, 3};
        personas1 = {4, 5, 6, 7};
        personas2 = {9, 12, 18};
        idsPuestos = {32, 33, 34};
        stock32 = {{0, 100}, {10, 25}, {5, 30}, {12, 10}};
        stock33 = {{0, 50}, {10, 10}, {5, 25}, {12, 5}};
        stock34 = {{0, 30}, {10, 40}, {5, 20}, {12, 2}};
        stock35 = {{0, 5000}, {10, 5000}, {5, 5000}, {12, 5000}};
        stock36 = {{0, 2500}, {10, 2500}, {5, 2500}, {12, 2500}};
        stock37 = {{0, 1000}, {10, 1000}, {5, 1000}, {12, 1000}};
        stock38 = {{3, 10}, {4, 5}, {5, 30}, {7, 330000}};
        descuentos32 = {{0, {{50, 50}}}, {12, {{6, 30}}}};
        descuentos33 = {{5, {{8, 60}}}};
        descuentos34 = {{10, {{10, 30}}}, {7, {{4, 10}}}};
        descuentosEmpty = {};
        puesto32 = {stock32, descuentos32, menu};
        puesto33 = {stock33, descuentos33, menu};
        puesto34 = {stock34, descuentos34, menu};
        puesto35 = {stock35, descuentosEmpty, menu};
        puesto36 = {stock36, descuentosEmpty, menu};
        puesto37 = {stock37, descuentosEmpty, menu};
        puesto38 = {stock38, descuentosEmpty, menu};
        puestos = {{32, puesto32}, {33, puesto33}, {34, puesto34}};
        puestos2 = {{35, puesto35}, {36, puesto36}, {37, puesto37}};
        puestos3 = {{38, puesto38}};
        for(int i = 1; i <= 330000; i++){
            personas330k.insert(i);
        }
    }
};

TEST_F(tests_lolla, check_stock_laquemasgasto_y_gastoEnPuestos_con_hackeos){
    FachadaLollapatuza l(personas2, puestos);
    // La persona 9 compra 6 unidades del producto 10 en el puesto 34.
    // Gasto de 9: 1200
    l.registrarCompra(9, 10, 6, 34);
    // La persona 18 compra 3 unidades del producto 10 en el puesto 33.
    // Gasto de 18: 600
    l.registrarCompra(18, 10, 3 , 33);
    ASSERT_EQ(l.mayorGastador(), 9);
    ASSERT_EQ(l.gastoTotal(9), 1200);
    ASSERT_EQ(l.gastoTotal(18), 600);
    // La persona 12 compra 3 unidades del producto 5 en el puesto 32 y pasa a ser la que más gastó, con un gasto de 3750.
    l.registrarCompra(12, 5, 3, 32);
    ASSERT_EQ(l.mayorGastador(), 12);
    ASSERT_EQ(l.gastoTotal(12), 3750);
    // La persona 9 compra 4 unidades del producto 0 en el puesto 34 y pasa a ser la que más gastó, con un gasto de 3800.
    l.registrarCompra(9, 0, 4, 34);
    ASSERT_EQ(l.mayorGastador(), 9);
    ASSERT_EQ(l.gastoTotal(9), 3800);
    // Se hackea a la persona 9 una compra del producto 0, haciendo que deje de ser la que mas gasto y pase a ser la persona 12 de nuevo.
    l.hackear(9, 0);
    ASSERT_EQ(l.mayorGastador(), 12);
    ASSERT_EQ(l.gastoTotal(9), 3150);
    // La persona 18 compra 8 unidades del producto 5 en el puesto 33 (con descuento) y pasa a ser la que más gastó, con un gasto de 4600.
    l.registrarCompra(18, 5, 8, 33);
    ASSERT_EQ(l.mayorGastador(), 18);
    ASSERT_EQ(l.gastoTotal(18), 4600);
   // Se hackean todas las compras producto 10 hechas por la persona 18, haciendo que su gasto quede en 4000.
    l.hackear(18, 10);
    ASSERT_EQ(l.mayorGastador(), 18);
    l.hackear(18, 10);
    ASSERT_EQ(l.mayorGastador(), 18);
    l.hackear(18, 10);
    ASSERT_EQ(l.mayorGastador(), 18);
    ASSERT_EQ(l.gastoTotal(18), 4000);
    // La persona 9 compra todo el stock del producto 12 (10 unidades) en el puesto 32 (con descuento), quedando con un gasto de 5250 y pasando a ser la que más gastó.
    l.registrarCompra(9, 12, l.stockEnPuesto(32, 12), 32);
    ASSERT_EQ(l.mayorGastador(), 9);
    ASSERT_EQ(l.gastoTotal(9), 5250);

    // Chequeo de los stocks.
    // Puesto 32:
    ASSERT_EQ(l.stockEnPuesto(32, 0), 100);
    ASSERT_EQ(l.stockEnPuesto(32, 5), 27);
    ASSERT_EQ(l.stockEnPuesto(32, 10), 25);
    ASSERT_EQ(l.stockEnPuesto(32, 12), 0);
    // Puesto 33:
    ASSERT_EQ(l.stockEnPuesto(33, 0), 50);
    ASSERT_EQ(l.stockEnPuesto(33, 5), 17);
    ASSERT_EQ(l.stockEnPuesto(33, 10), 10);
    ASSERT_EQ(l.stockEnPuesto(33, 12), 5);
    // Puesto 34:
    ASSERT_EQ(l.stockEnPuesto(34, 0), 27);
    ASSERT_EQ(l.stockEnPuesto(34, 5), 20);
    ASSERT_EQ(l.stockEnPuesto(34, 10), 34);
    ASSERT_EQ(l.stockEnPuesto(34, 12), 2);

    ASSERT_EQ(l.menorStock(0), 34);
    ASSERT_EQ(l.menorStock(5), 33);
    ASSERT_EQ(l.menorStock(10), 33);
    ASSERT_EQ(l.menorStock(12), 32);

    // Chequeo del gasto de cada persona en cada puesto.
    // Persona 9:
    ASSERT_EQ(l.gastoEnPuesto(32, 9), 2100);
    ASSERT_EQ(l.gastoEnPuesto(33, 9), 0);
    ASSERT_EQ(l.gastoEnPuesto(34, 9), 3150);
    // Persona 12:
    ASSERT_EQ(l.gastoEnPuesto(32, 12), 3750);
    ASSERT_EQ(l.gastoEnPuesto(33, 12), 0);
    ASSERT_EQ(l.gastoEnPuesto(34, 12), 0);
    // Persona 18:
    ASSERT_EQ(l.gastoEnPuesto(32, 18), 0);
    ASSERT_EQ(l.gastoEnPuesto(33, 18), 4000);
    ASSERT_EQ(l.gastoEnPuesto(34, 18), 0);
}

TEST_F(tests_lolla, desempate_laquemasgasto){
    FachadaLollapatuza l(personas1, puestos);
    l.registrarCompra(5, 0, 1, 34); // 650
    l.registrarCompra(5, 10, 3, 33); // 650 + 600 = 1250
    l.registrarCompra(6, 5, 1, 32); // 1250
    EXPECT_EQ(l.gastoTotal(5), l.gastoTotal(6));
    // 5 y 6 tienen mismo gasto, pero como se desempata por menor id, mayorGastador debería devolver 5.
    EXPECT_EQ(l.mayorGastador(), 5);
    l.registrarCompra(7, 0, 1, 33); // 650
    l.registrarCompra(7, 12, 2, 34); // 650 + 600 = 1250
    EXPECT_EQ(l.gastoTotal(7), l.gastoTotal(5));
    // No se modifica la persona que mas gasto
    EXPECT_EQ(l.mayorGastador(), 5);
    l.registrarCompra(4, 5, 1, 33); // 1250
    EXPECT_EQ(l.gastoTotal(4), l.gastoTotal(5));
    // Como gasto(5) = gasto(4) pero 4 < 5, en este caso si cambia la persona que más gastó.
    EXPECT_EQ(l.mayorGastador(), 4);
}

TEST_F(tests_lolla, test_gastos_con_hackeos){
    FachadaLollapatuza l(personas, puestos2);
    map<IdPuesto, map<Producto, Nat>> stockOriginal;
    for(IdPuesto id: l.idsDePuestos()){
        for(Nat n: {0,5,10,12}){
            stockOriginal[id][n] = l.stockEnPuesto(id, n);
        }
    }
    for(Persona per: l.personas()){
        EXPECT_EQ(l.gastoTotal(per), 0);
    }
    int c = 1;
    while(l.stockEnPuesto(35, 0) > 0){
        l.registrarCompra(1, 0, 1, 35);
        l.registrarCompra(1, 5, 1, 35);
        l.registrarCompra(1, 10, 1, 35);
        l.registrarCompra(1, 12, 1, 35);
        EXPECT_EQ(l.gastoTotal(1), 2400 * c);
        c++;
    }
    c = 1;
    while(l.stockEnPuesto(36, 0) > 0){
        l.registrarCompra(2, 0, 1, 36);
        l.registrarCompra(2, 5, 1, 36);
        l.registrarCompra(2, 10, 1, 36);
        l.registrarCompra(2, 12, 1, 36);
        EXPECT_EQ(l.gastoTotal(2), 2400 * c);
        c++;
    }
    c = 1;
    while(l.stockEnPuesto(37, 0) > 0){
        l.registrarCompra(3, 0, 1, 37);
        l.registrarCompra(3, 5, 1, 37);
        l.registrarCompra(3, 10, 1, 37);
        l.registrarCompra(3, 12, 1, 37);
        EXPECT_EQ(l.gastoTotal(3), 2400 * c);
        c++;
    }
    for(int i: {0, 5, 10, 12}){
        for(IdPuesto j: l.idsDePuestos()){
            EXPECT_EQ(l.stockEnPuesto(j, i), 0);
        }
        EXPECT_EQ(l.menorStock(i), 35);
    }
    EXPECT_EQ(l.mayorGastador(), 1);
    for(Persona per: l.personas()){
        c = 1;
        Nat gasto = l.gastoTotal(per);
        if(per == 1){
            while(l.stockEnPuesto(35, 0) < 5000){
                l.hackear(1, 0);
                l.hackear(1, 5);
                l.hackear(1, 10);
                l.hackear(1, 12);
                EXPECT_EQ(l.gastoTotal(1), gasto - (2400 * c) );
                c++;
            }
            EXPECT_EQ(l.gastoTotal(1), 0);
        }else if(per == 2){
            while(l.stockEnPuesto(36, 0) < 2500){
                l.hackear(2, 0);
                l.hackear(2, 5);
                l.hackear(2, 10);
                l.hackear(2, 12);
                EXPECT_EQ(l.gastoTotal(2), gasto - (2400 * c) );
                c++;
            }
            EXPECT_EQ(l.gastoTotal(2), 0);
        }else{
            while(l.stockEnPuesto(37, 0) < 1000){
                l.hackear(3, 0);
                l.hackear(3, 5);
                l.hackear(3, 10);
                l.hackear(3, 12);
                EXPECT_EQ(l.gastoTotal(3), gasto - (2400 * c) );
                c++;
            }
            EXPECT_EQ(l.gastoTotal(3), 0);
        }
    }
    for(IdPuesto id: l.idsDePuestos()){
        for(Nat n: {0,5,10,12}){
            EXPECT_EQ(l.stockEnPuesto(id, n), stockOriginal[id][n]);
        }
    }

}

TEST_F(tests_lolla, lolla_con_personas_2023) {
    FachadaLollapatuza l(personas330k, puestos3);
    for(int i=1; i <= 330000; i++){
        l.registrarCompra(i, 7, 1, 38);
        EXPECT_EQ(l.gastoTotal(i), 2000);
        EXPECT_EQ(l.stockEnPuesto(38,7), 330000 - i);
    }
    EXPECT_EQ(l.mayorGastador(),1);
    for(int i=1; i< 330000; i++){
        l.hackear(i, 7); 
        EXPECT_EQ(l.gastoTotal(i),0);
        EXPECT_EQ(l.stockEnPuesto (38,7),i);
        EXPECT_EQ(l.mayorGastador(), i + 1);
    }
    EXPECT_EQ(l.mayorGastador(), 330000);
}
