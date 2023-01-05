package HomeWorkHDL
import chisel3._
import chisel3.util._

class ControlLogic extends Module {
    val io = IO(new Bundle {
        // 0 mode select - switch 
        // 1 up/down select - switch
        // 2 second change - button
        // 3 minute change - button
        // 4 hour change - button
        // 5 hold down - switch
        val inVec = Input(Vec(6, Bool()))
        val timeUp = Input(Bool())
        val modeSel = Output(Bool())
        val loadVec = Output(Vec(3, Bool()))
        val holdD = Output(Bool())
        val IncDec = Output(Vec(3, Bool()))
    })

    def rising(s: Bool) = s & !RegNext(s)

    io.modeSel := io.inVec(0)
    for (i <- 0 until 3) {
        io.IncDec(i) := io.inVec(1)
        io.loadVec(i) := rising(io.inVec(2+i))
    }
    io.holdD := (io.inVec(5) | io.timeUp) & !io.inVec(2) & !io.inVec(3) & !io.inVec(4)
}