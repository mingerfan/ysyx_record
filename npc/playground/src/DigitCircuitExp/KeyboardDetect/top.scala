package KeyboardDetect
import chisel3._
import chisel3.util._
import ShiftReg.Decoder
import HomeWorkHDL.ClockGen

class KeyBoard extends Module {
    val io = IO(new Bundle {
        val ps2_clk = Input(Bool())
        val ps2_data = Input(Bool())
        val segs = Output(Vec(6, UInt(7.W)))
        val ascii = Output(UInt(8.W))
        val clkout = Output(Bool())
        val backspace = Output(Bool())
    })

    val boardInputScan = Module(new BoardInputScan)
    val regRecord = Module(new RegRecord)

    val clockGen = Module(new ClockGen(10000000, 1))
    val risingEdge = WireDefault(!RegNext(clockGen.io.outClk) & clockGen.io.outClk)
    val timesReg = RegInit(0.U(8.W))
    val lastAscii = RegInit(0.U(8.W))

    io.clkout := clockGen.io.clkout
    io.ascii := regRecord.io.ascii

    when (risingEdge && regRecord.io.ascii =/= 0.U) {
        lastAscii := regRecord.io.ascii
        when (lastAscii === regRecord.io.ascii) {
            timesReg := timesReg + 1.U
        } .otherwise {
            timesReg := 0.U
        }
    } .elsewhen (regRecord.io.ascii === 0.U) {
        timesReg := 0.U
    }

    val segsArr = new Array[Decoder](6)
    for (i <- 0 until 6) {
        segsArr(i) = Module(new Decoder)
        io.segs(i) := segsArr(i).io.out
    }

    boardInputScan.io.ps2_clk := io.ps2_clk
    boardInputScan.io.ps2_data := io.ps2_data
    
    regRecord.io.fifo <> boardInputScan.io.cmdOut

    segsArr(0).io.inNum := regRecord.io.keyCode(3,0)
    segsArr(1).io.inNum := regRecord.io.keyCode(7,4)

    segsArr(2).io.inNum := regRecord.io.ascii(3,0)
    segsArr(3).io.inNum := regRecord.io.ascii(7,4)

    segsArr(4).io.inNum := timesReg(3,0)
    segsArr(5).io.inNum := timesReg(7,4)

    io.backspace := regRecord.io.backspace
}