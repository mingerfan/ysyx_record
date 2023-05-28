package ScProcessor
package RF
import chisel3._
import chisel3.util._
import rfInfo._
import topInfo._

object rfInfo {
    val REGS_NUM = topInfo.R_NUM
    val REGS_WIDTH = topInfo.XLEN 
}

class DualPortsIO(num: Int, width: Int) extends Bundle {
    val rdAddr1 = Input(UInt(log2Ceil(num).W))
    val rdData1 = Output(UInt(width.W))
    val rdAddr2 = Input(UInt(log2Ceil(num).W))
    val rdData2 = Output(UInt(width.W))
    val wrEn = Input(Bool())
    val wrAddr = Input(UInt(log2Ceil(num).W))
    val wrData = Input(UInt(width.W))
}

class Regs(num: Int, width: Int) extends Module {
    val io = IO(new DualPortsIO(num, width))
    val dbg_regs_w = if (NPC_SIM) num*width else 0
    val dbg_regs = IO(Output(UInt(dbg_regs_w.W)))
    val reg_file = Reg(Vec(num, UInt(width.W)))
    io.rdData1 := reg_file(io.rdAddr1)
    io.rdData2 := reg_file(io.rdAddr2)
    // reg_file(io.wrAddr) := Mux(io.wrEn, io.wrData, reg_file(io.wrAddr))
    when (io.wrEn) {
        reg_file(io.wrAddr) := io.wrData
    }

    dbg_regs := 0.U
    if (NPC_SIM) {
        val regs_wire = Wire(Vec(num, UInt(width.W)))
        for (i <- 0 until num) {
            if (i != 0) {
                regs_wire(i) := reg_file(i)
            } else {
                regs_wire(0) := 0.U
            }
        }
        dbg_regs := regs_wire.asUInt
    }
}

class RegFile extends Module {
    val io = IO(new DualPortsIO(REGS_NUM, REGS_WIDTH))
    val regs = Module(new Regs(REGS_NUM, REGS_WIDTH))
    val dbg_regs_w = if (NPC_SIM) REGS_NUM*REGS_WIDTH else 0
    val dbg_regs = IO(Output(UInt(dbg_regs_w.W)))

    dbg_regs := 0.U
    if (NPC_SIM) {
        dbg_regs := regs.dbg_regs
    }

    regs.io.rdAddr1 := io.rdAddr1
    regs.io.rdAddr2 := io.rdAddr2
    regs.io.wrEn := Mux(reset.asBool(), true.B, io.wrEn)
    regs.io.wrAddr := io.wrAddr
    regs.io.wrData := Mux(reset.asBool(), 0.U, io.wrData)
    io.rdData1 := Mux(io.rdAddr1 === 0.U, 0.U, regs.io.rdData1)
    io.rdData2 := Mux(io.rdAddr2 === 0.U, 0.U, regs.io.rdData2)
}

object RFApp extends App {
    // emitVerilog(new RegFile(), Array("--target-dir", "./build"))
    val s = getVerilogString(new RegFile())
    println(s)
}