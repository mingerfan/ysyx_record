package ScProcessor.IFU
import chisel3._
import chisel3.util._
import ScProcessor.topInfo._

class INST_READ extends BlackBox {
    val io = IO(new Bundle {
        val clk   = Input(Bool())
        val rst   = Input(Bool())
        val raddr = Input(UInt(XLEN.W))
        val rdata = Output(UInt(XLEN.W))
    })
}

class IFUBundleOut extends Bundle {
    val pc = UInt(XLEN.W)
    val inst = UInt(INS_LEN.W)
}

class IFUBundleIn extends Bundle {
    val pc = Input(UInt(XLEN.W))
}

class IFU extends Module {
    val out = IO(DecoupledIO(new IFUBundleOut()))
    val in = IO(Flipped(DecoupledIO(new IFUBundleIn())))


    val inst_in = Module(new INST_READ)
    inst_in.io.clk := clock.asBool
    inst_in.io.rst := reset

    inst_in.io.raddr := in.bits.pc
    out.bits.pc := in.bits.pc
    out.bits.inst := inst_in.io.rdata

    in.ready := true.B
    out.valid := true.B
}
