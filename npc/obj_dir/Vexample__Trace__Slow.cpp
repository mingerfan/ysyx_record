// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vexample__Syms.h"


void Vexample___024root__traceInitSub0(Vexample___024root* vlSelf, VerilatedVcd* tracep) VL_ATTR_COLD;

void Vexample___024root__traceInitTop(Vexample___024root* vlSelf, VerilatedVcd* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vexample__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    {
        Vexample___024root__traceInitSub0(vlSelf, tracep);
    }
}

void Vexample___024root__traceInitSub0(Vexample___024root* vlSelf, VerilatedVcd* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vexample__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    const int c = vlSymsp->__Vm_baseCode;
    if (false && tracep && c) {}  // Prevent unused
}

void Vexample___024root__traceCleanup(void* voidSelf, VerilatedVcd* /*unused*/);

void Vexample___024root__traceRegister(Vexample___024root* vlSelf, VerilatedVcd* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vexample__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    {
        tracep->addCleanupCb(&Vexample___024root__traceCleanup, vlSelf);
    }
}
