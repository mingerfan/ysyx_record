package HomeWorkHDL
import chisel3._
import chisel3.util._

class ClockTimer extends Module {
    val io = IO(new Bundle {
        val clk1Hz = Input(Bool())
        val modeSel = Input(Bool()) // false for count down
        val loadVec = Input(Vec(3, Bool()))
        val holdD = Input(Bool())
        val timeUp = Output(Bool())
        val IncDec = Input(Vec(3, Bool()))  // false for dec, true for inc
        val timeVec = Output(Vec(3, UInt(8.W))) // SS:MM:HH
    })

    val secCnt = Module(new ReCntRising10(6, 0))
    val minCnt = Module(new ReCntRising10(6, 0))
    val horCnt = Module(new ReCntRising10(2, 4))

    val secCntD = Module(new ReCntDecreasing10(6, 0))
    val minCntD = Module(new ReCntDecreasing10(6, 0))
    val horCntD = Module(new ReCntDecreasing10(2, 4))

    secCnt.io.reClk := io.clk1Hz
    minCnt.io.reClk := secCnt.io.tick
    horCnt.io.reClk := minCnt.io.tick

    secCntD.io.reClk := io.clk1Hz
    minCntD.io.reClk := secCntD.io.tick
    horCntD.io.reClk := minCntD.io.tick

    secCnt.io.load := Mux(io.modeSel, io.loadVec(0), false.B)
    minCnt.io.load := Mux(io.modeSel, io.loadVec(1), false.B)
    horCnt.io.load := Mux(io.modeSel, io.loadVec(2), false.B)

    secCntD.io.load := Mux(io.modeSel, false.B, io.loadVec(0))
    minCntD.io.load := Mux(io.modeSel, false.B, io.loadVec(1))
    horCntD.io.load := Mux(io.modeSel, false.B, io.loadVec(2))

    secCnt.io.loadNum := Mux(io.IncDec(0), secCnt.io.cntOut + 1.U, secCnt.io.cntOut - 1.U)
    minCnt.io.loadNum := Mux(io.IncDec(1), minCnt.io.cntOut + 1.U, minCnt.io.cntOut - 1.U)
    horCnt.io.loadNum := Mux(io.IncDec(2), horCnt.io.cntOut + 1.U, horCnt.io.cntOut - 1.U)

    secCntD.io.loadNum := Mux(io.IncDec(0), secCntD.io.cntOut + 1.U, secCntD.io.cntOut - 1.U)
    minCntD.io.loadNum := Mux(io.IncDec(1), minCntD.io.cntOut + 1.U, minCntD.io.cntOut - 1.U)
    horCntD.io.loadNum := Mux(io.IncDec(2), horCntD.io.cntOut + 1.U, horCntD.io.cntOut - 1.U)

    when (io.holdD) {
        secCntD.io.load := true.B
        minCntD.io.load := true.B
        horCntD.io.load := true.B

        secCntD.io.loadNum := secCntD.io.cntOut
        minCntD.io.loadNum := minCntD.io.cntOut
        horCntD.io.loadNum := horCntD.io.cntOut
    }

    io.timeUp := (secCntD.io.cntOut === 0.U) && (minCntD.io.cntOut === 0.U) && (horCntD.io.cntOut === 0.U)

    io.timeVec(0) := Mux(io.modeSel, secCnt.io.cntOut, secCntD.io.cntOut)
    io.timeVec(1) := Mux(io.modeSel, minCnt.io.cntOut, minCntD.io.cntOut)
    io.timeVec(2) := Mux(io.modeSel, horCnt.io.cntOut, horCntD.io.cntOut)
}