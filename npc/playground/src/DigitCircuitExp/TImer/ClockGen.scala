package HomeWorkHDL
import chisel3._
import chisel3.util._

// dstFrq is less than srcFrq
class ClockGen(srcFrq: Int, dstFrq: Int) extends Module {
    var cnt = 0
    if (dstFrq >= srcFrq) {
        cnt = 0
    } else {
        cnt = (srcFrq/dstFrq)/2 - 1
    }
    val io = IO(new Bundle {
        val outClk = Output(Bool())
    })

    val cntReg = RegInit(cnt.U)
    val outReg = RegInit(0.U)

    cntReg := cntReg - 1.U
    when (cntReg === 0.U) {
        cntReg := cnt.U
    }

    when (cntReg === 0.U) {
        outReg := ~outReg
    }

    io.outClk := outReg
}