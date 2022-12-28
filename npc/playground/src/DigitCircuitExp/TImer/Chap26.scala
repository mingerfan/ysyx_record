package Chap26
import chisel3._
import chisel3.util._

class BJQModule(tfac: Int) extends Module {
    val io = IO(new Bundle {
        val signalIn = Input(Bool())
        val signalOut = Output(Bool())
    })

    def sync(in: Bool) = RegNext(RegNext(in))

    def rising(in: Bool) = in & !RegNext(in)

    def tickGen(fac: Int) = {
        val reg = RegInit(0.U(log2Up(fac).W))
        reg := reg + 1.U
        when(reg === (fac - 1).U) {
            reg := 0.U
        } // 用mux也可以的
        reg === (fac - 1).U
    }

    def filter(in: Bool, tick: Bool) = {
        val reg = RegInit(0.U(3.W))
        when (tick) {
            reg := reg(1, 0) ## in
        }
        (reg(2) & reg(1)) | (reg(2) & reg(0)) | (reg(1) & reg(0))
    }

    val tick = tickGen(tfac)
    val SigSap = RegInit(false.B)

    when (tick) {
        SigSap := sync(io.signalIn)
    }

    val filterOut = filter(SigSap, tick)
    io.signalOut := rising(filterOut)
}

object BJQMain extends App {
    val s = getVerilogString(new BJQModule(10))
    println(s)
}