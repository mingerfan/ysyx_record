package HomeWorkHDL
import chisel3._
import chisel3.util._
import Chap26._

class top extends Module {
    val io = IO(new Bundle {
        // 0 mode select - switch 
        // 1 up/down select - switch
        // 2 second change - button
        // 3 minute change - button
        // 4 hour change - button
        // 5 hold down - switch
        val inVec = Input(Vec(6, Bool()))
        val outVec = Output(Vec(6, UInt(7.W)))
        val beepOut = Output(Bool())
    })

    val srcClk = 50000000
    // val srcClk = 150    // Only for test

    val clk1Hz = Module(new ClockGen(srcClk, 1))
    val clockTimer = Module(new ClockTimer)
    val cLogic = Module(new ControlLogic)

    val DecVec = new Array[Decoder2](3)
    val FilterVec = new Array[BJQModule](3)

    val beep = Module(new Beep(srcClk))
    io.beepOut := beep.io.beepOut & clockTimer.io.timeUp

    for (i <- 0 until 3) {
        DecVec(i) = Module(new Decoder2)
    }

    for (i <- 0 until 3) {
        FilterVec(i) = Module(new BJQModule(10))
        FilterVec(i).io.signalIn := ~io.inVec(i+2)
        cLogic.io.inVec(i+2) := FilterVec(i).io.signalOut
    }

    cLogic.io.inVec(0) := io.inVec(0)
    cLogic.io.inVec(1) := io.inVec(1)
    cLogic.io.inVec(5) := io.inVec(5)
    cLogic.io.timeUp := clockTimer.io.timeUp

    clockTimer.io.clk1Hz := clk1Hz.io.outClk
    clockTimer.io.modeSel := cLogic.io.modeSel
    clockTimer.io.holdD := cLogic.io.holdD
    
    for (i <- 0 until 3) {
        DecVec(i).io.inNum := clockTimer.io.timeVec(i)
        clockTimer.io.IncDec(i) := cLogic.io.IncDec(i)
        clockTimer.io.loadVec(i) := cLogic.io.loadVec(i)

        io.outVec(i * 2) := DecVec(i).io.out1
        io.outVec(i * 2 + 1) := DecVec(i).io.out2
    }
}

object TopMain extends App {
    emitVerilog(new top, Array("--target-dir", "generated"))
}