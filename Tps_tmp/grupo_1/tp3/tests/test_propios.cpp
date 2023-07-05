#include "../src/fachada_lollapatuza.h"
#include "gtest-1.8.1/gtest.h"
#include <set>

using namespace std;

class LollaTestPropios : public testing::Test {
protected:
    Menu menu;
    set<Persona> personas;
    set<IdPuesto> idsPuestos;
    Stock stock2, stock6, stock7;
    Promociones descuentos2, descuentos6, descuentos7;
    aed2_Puesto puesto2, puesto6, puesto7;
    map<IdPuesto, aed2_Puesto> puestos;

    void SetUp() {
        menu = {{3, 500}, {4, 1000}, {5, 2500}, {7, 2000}};
        personas = {2, 4, 8, 9};
        idsPuestos = {2, 6, 7};
        stock2 = {{3, 10}, {4, 5}, {5, 30}, {7, 10}};
        stock6 = {{3, 5}, {4, 40}, {5, 25}, {7, 5}};
        stock7 = {{3, 20}, {4, 20}, {5, 20}, {7, 20}};
        descuentos2 = {{7, {{3, 20}, {6, 25}}}};
        descuentos6 = {{4, {
                {3, 10}, {4, 15}, {5, 24}, {6, 25},
                {7, 28}, {9, 36}, {10, 40}, {11, 42},
                {12, 45}, {14, 47}, {17, 50}, {18, 55},
                {20, 60}, {22, 70}
        }}};
        descuentos7 = {{3, {{5, 30}}}, {7, {{2, 10}}}};
        puesto2 = {stock2, descuentos2, menu};
        puesto6 = {stock6, descuentos6, menu};
        puesto7 = {stock7, descuentos7, menu};
        puestos = {{2, puesto2}, {6, puesto6}, {7, puesto7}};
    }
};

TEST_F(LollaTestPropios, muchos_descuentos) {
    FachadaLollapatuza l(personas, puestos);

    l.registrarCompra(9, 4, 8, 6);

    EXPECT_EQ(l.gastoTotal(2), 0);
    EXPECT_EQ(l.gastoTotal(4), 0);
    EXPECT_EQ(l.gastoTotal(8), 0);
    EXPECT_EQ(l.gastoTotal(9), 5760);
    EXPECT_EQ(l.mayorGastador(), 9);
    EXPECT_EQ(l.menorStock(3), 6);
    EXPECT_EQ(l.menorStock(4), 2);
    EXPECT_EQ(l.menorStock(5), 7);
    EXPECT_EQ(l.menorStock(7), 6);
    EXPECT_EQ(l.personas(), personas);
    for (const pair<const Producto, Nat>& s : stock2) {
        EXPECT_EQ(l.stockEnPuesto(2, s.first), s.second);
    }
    EXPECT_EQ(l.stockEnPuesto(6, 3), 5);
    EXPECT_EQ(l.stockEnPuesto(6, 4), 32);
    EXPECT_EQ(l.stockEnPuesto(6, 5), 25);
    EXPECT_EQ(l.stockEnPuesto(6, 7), 5);
    for (const pair<const Producto, Nat>& s : stock7) {
        EXPECT_EQ(l.stockEnPuesto(7, s.first), s.second);
    }
    for (const Persona& p : personas) {
        EXPECT_EQ(l.gastoEnPuesto(2, p), 0);
    }
    EXPECT_EQ(l.gastoEnPuesto(6, 2), 0);
    EXPECT_EQ(l.gastoEnPuesto(6, 4), 0);
    EXPECT_EQ(l.gastoEnPuesto(6, 8), 0);
    EXPECT_EQ(l.gastoEnPuesto(6, 9), 5760);
    for (const Persona& p : personas) {
        EXPECT_EQ(l.gastoEnPuesto(7, p), 0);
    }
    EXPECT_EQ(l.descuentoEnPuesto(2, 7, 2), 0);
    EXPECT_EQ(l.descuentoEnPuesto(2, 7, 3), 20);
    EXPECT_EQ(l.descuentoEnPuesto(2, 7, 4), 20);
    EXPECT_EQ(l.descuentoEnPuesto(2, 7, 5), 20);
    EXPECT_EQ(l.descuentoEnPuesto(2, 7, 6), 25);
    EXPECT_EQ(l.descuentoEnPuesto(2, 7, 7), 25);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 2), 0);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 3), 10);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 5), 24);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 6), 25);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 8), 28);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 10), 40);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 11), 42);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 13), 45);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 16), 47);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 19), 55);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 21), 60);
    EXPECT_EQ(l.descuentoEnPuesto(6, 4, 24), 70);
    EXPECT_EQ(l.descuentoEnPuesto(7, 3, 4), 0);
    EXPECT_EQ(l.descuentoEnPuesto(7, 3, 5), 30);
    EXPECT_EQ(l.descuentoEnPuesto(7, 3, 6), 30);
    EXPECT_EQ(l.descuentoEnPuesto(7, 7, 1), 0);
    EXPECT_EQ(l.descuentoEnPuesto(7, 7, 2), 10);
    EXPECT_EQ(l.descuentoEnPuesto(7, 7, 3), 10);
    EXPECT_EQ(l.idsDePuestos(), idsPuestos);
}

TEST_F(LollaTestPropios, cambiar_puesto_hackeable) {
    FachadaLollapatuza l(personas, puestos);
    l.registrarCompra(8, 4, 6, 7);
    l.hackear(8, 4);

    EXPECT_EQ(l.gastoTotal(2), 0);
    EXPECT_EQ(l.gastoTotal(4), 0);
    EXPECT_EQ(l.gastoTotal(8), 5000);
    EXPECT_EQ(l.gastoTotal(9), 0);
    EXPECT_EQ(l.mayorGastador(), 8);
    EXPECT_EQ(l.menorStock(3), 6);
    EXPECT_EQ(l.menorStock(4), 2);
    EXPECT_EQ(l.menorStock(5), 7);
    EXPECT_EQ(l.menorStock(7), 6);
    EXPECT_EQ(l.personas(), personas);
    for (const pair<const Producto, Nat>& s : stock2) {
        EXPECT_EQ(l.stockEnPuesto(2, s.first), s.second);
    }
    for (const pair<const Producto, Nat>& s : stock6) {
        EXPECT_EQ(l.stockEnPuesto(6, s.first), s.second);
    }
    EXPECT_EQ(l.stockEnPuesto(7, 3), 20);
    EXPECT_EQ(l.stockEnPuesto(7, 4), 15);
    EXPECT_EQ(l.stockEnPuesto(7, 5), 20);
    EXPECT_EQ(l.stockEnPuesto(7, 7), 20);
    for (const Persona& p : personas) {
        EXPECT_EQ(l.gastoEnPuesto(6, p), 0);
        EXPECT_EQ(l.gastoEnPuesto(2, p), 0);
    }
    EXPECT_EQ(l.gastoEnPuesto(7, 2), 0);
    EXPECT_EQ(l.gastoEnPuesto(7, 4), 0);
    EXPECT_EQ(l.gastoEnPuesto(7, 8), 5000);
    EXPECT_EQ(l.gastoEnPuesto(7, 9), 0);
    EXPECT_EQ(l.idsDePuestos(), idsPuestos);

    l.registrarCompra(8, 4, 3, 2);
    l.hackear(8, 4);

    EXPECT_EQ(l.gastoTotal(2), 0);
    EXPECT_EQ(l.gastoTotal(4), 0);
    EXPECT_EQ(l.gastoTotal(8), 7000);
    EXPECT_EQ(l.gastoTotal(9), 0);
    EXPECT_EQ(l.mayorGastador(), 8);
    EXPECT_EQ(l.menorStock(3), 6);
    EXPECT_EQ(l.menorStock(4), 2);
    EXPECT_EQ(l.menorStock(5), 7);
    EXPECT_EQ(l.menorStock(7), 6);
    EXPECT_EQ(l.personas(), personas);
    EXPECT_EQ(l.stockEnPuesto(2, 3), 10);
    EXPECT_EQ(l.stockEnPuesto(2, 4), 3);
    EXPECT_EQ(l.stockEnPuesto(2, 5), 30);
    EXPECT_EQ(l.stockEnPuesto(2, 7), 10);
    for (const pair<const Producto, Nat>& s : stock6) {
        EXPECT_EQ(l.stockEnPuesto(6, s.first), s.second);
    }
    EXPECT_EQ(l.stockEnPuesto(7, 3), 20);
    EXPECT_EQ(l.stockEnPuesto(7, 4), 15);
    EXPECT_EQ(l.stockEnPuesto(7, 5), 20);
    EXPECT_EQ(l.stockEnPuesto(7, 7), 20);
    EXPECT_EQ(l.gastoEnPuesto(2, 2), 0);
    EXPECT_EQ(l.gastoEnPuesto(2, 4), 0);
    EXPECT_EQ(l.gastoEnPuesto(2, 8), 2000);
    EXPECT_EQ(l.gastoEnPuesto(2, 9), 0);
    for (const Persona& p : personas) {
        EXPECT_EQ(l.gastoEnPuesto(6, p), 0);
    }
    EXPECT_EQ(l.gastoEnPuesto(7, 2), 0);
    EXPECT_EQ(l.gastoEnPuesto(7, 4), 0);
    EXPECT_EQ(l.gastoEnPuesto(7, 8), 5000);
    EXPECT_EQ(l.gastoEnPuesto(7, 9), 0);
}

TEST_F(LollaTestPropios, eliminar_puesto_hackeable) {
    FachadaLollapatuza l(personas, puestos);

    l.registrarCompra(8, 4, 2, 2);
    l.registrarCompra(8, 4, 6, 7);
    l.hackear(8, 4);
    l.hackear(8, 4);

    EXPECT_EQ(l.gastoTotal(2), 0);
    EXPECT_EQ(l.gastoTotal(4), 0);
    EXPECT_EQ(l.gastoTotal(8), 6000);
    EXPECT_EQ(l.gastoTotal(9), 0);
    EXPECT_EQ(l.mayorGastador(), 8);
    EXPECT_EQ(l.menorStock(3), 6);
    EXPECT_EQ(l.menorStock(4), 2);
    EXPECT_EQ(l.menorStock(5), 7);
    EXPECT_EQ(l.menorStock(7), 6);
    EXPECT_EQ(l.personas(), personas);
    EXPECT_EQ(l.stockEnPuesto(2, 3), 10);
    EXPECT_EQ(l.stockEnPuesto(2, 4), 5);
    EXPECT_EQ(l.stockEnPuesto(2, 5), 30);
    EXPECT_EQ(l.stockEnPuesto(2, 7), 10);
    for (const pair<const Producto, Nat>& s : stock6) {
        EXPECT_EQ(l.stockEnPuesto(6, s.first), s.second);
    }
    EXPECT_EQ(l.stockEnPuesto(7, 3), 20);
    EXPECT_EQ(l.stockEnPuesto(7, 4), 14);
    EXPECT_EQ(l.stockEnPuesto(7, 5), 20);
    EXPECT_EQ(l.stockEnPuesto(7, 7), 20);
    EXPECT_EQ(l.gastoEnPuesto(2, 2), 0);
    EXPECT_EQ(l.gastoEnPuesto(2, 4), 0);
    EXPECT_EQ(l.gastoEnPuesto(2, 8), 0);
    EXPECT_EQ(l.gastoEnPuesto(2, 9), 0);
    for (const Persona& p : personas) {
        EXPECT_EQ(l.gastoEnPuesto(6, p), 0);
    }
    EXPECT_EQ(l.gastoEnPuesto(7, 2), 0);
    EXPECT_EQ(l.gastoEnPuesto(7, 4), 0);
    EXPECT_EQ(l.gastoEnPuesto(7, 8), 6000);
    EXPECT_EQ(l.gastoEnPuesto(7, 9), 0);
    EXPECT_EQ(l.idsDePuestos(), idsPuestos);

    l.hackear(8, 4);

    EXPECT_EQ(l.gastoTotal(2), 0);
    EXPECT_EQ(l.gastoTotal(4), 0);
    EXPECT_EQ(l.gastoTotal(8), 5000);
    EXPECT_EQ(l.gastoTotal(9), 0);
    EXPECT_EQ(l.mayorGastador(), 8);
    EXPECT_EQ(l.menorStock(3), 6);
    EXPECT_EQ(l.menorStock(4), 2);
    EXPECT_EQ(l.menorStock(5), 7);
    EXPECT_EQ(l.menorStock(7), 6);
    EXPECT_EQ(l.personas(), personas);
    EXPECT_EQ(l.stockEnPuesto(2, 3), 10);
    EXPECT_EQ(l.stockEnPuesto(2, 4), 5);
    EXPECT_EQ(l.stockEnPuesto(2, 5), 30);
    EXPECT_EQ(l.stockEnPuesto(2, 7), 10);
    for (const pair<const Producto, Nat>& s : stock6) {
        EXPECT_EQ(l.stockEnPuesto(6, s.first), s.second);
    }
    EXPECT_EQ(l.stockEnPuesto(7, 3), 20);
    EXPECT_EQ(l.stockEnPuesto(7, 4), 15);
    EXPECT_EQ(l.stockEnPuesto(7, 5), 20);
    EXPECT_EQ(l.stockEnPuesto(7, 7), 20);
    EXPECT_EQ(l.gastoEnPuesto(2, 2), 0);
    EXPECT_EQ(l.gastoEnPuesto(2, 4), 0);
    EXPECT_EQ(l.gastoEnPuesto(2, 8), 0);
    EXPECT_EQ(l.gastoEnPuesto(2, 9), 0);
    for (const Persona& p : personas) {
        EXPECT_EQ(l.gastoEnPuesto(6, p), 0);
    }
    EXPECT_EQ(l.gastoEnPuesto(7, 2), 0);
    EXPECT_EQ(l.gastoEnPuesto(7, 4), 0);
    EXPECT_EQ(l.gastoEnPuesto(7, 8), 5000);
    EXPECT_EQ(l.gastoEnPuesto(7, 9), 0);
}