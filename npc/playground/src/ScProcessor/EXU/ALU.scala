package ScProcessor
package EXU
import chisel3._
import chisel3.util._
import ALUInfo._
import tools._

object ALUInfo {
    val OPS_NUM = IDU.IDUInsInfo.aluOps.length
}

class ALU extends Module {
    val io = IO(new Bundle {
        val in1 = Input(UInt(topInfo.XLEN.W))
        val in2 = Input(UInt(topInfo.XLEN.W))
        val out = Output(UInt(topInfo.XLEN.W))
        val aluOp = Input(UInt(OPS_NUM.W))
    })
    
    val aluOps = IDU.IDUInsInfo.aluOps
    val hit = U_HIT_CURRYING(io.aluOp, aluOps)_

    val in1pin2 = io.in1 + io.in2
    val in1sin2 = io.in1 - io.in2
    val in1xorin2 = io.in1 ^ io.in2
    val in1andin2 = io.in1 & io.in2
    val in1orin2 = io.in1 | io.in2
    val in1mlin2_32 = io.in1(31, 0)*io.in2(31, 0)
    val in1DIVWin2 = io.in1(31, 0).asSInt/io.in2(31, 0).asSInt
    val in1REMWin2 = io.in1(31, 0).asSInt%io.in2(31, 0).asSInt

    io.out := Mux1H(Seq(
        hit("SUM") -> (in1pin2),
        hit("UCMP")-> (io.in1 < io.in2),
        hit("SCMP")-> (io.in1.asSInt < io.in2.asSInt),
        hit("WSUM")-> U_SEXT64(in1pin2(31, 0), 32),
        hit("WSUB")-> U_SEXT64(in1sin2(31, 0), 32),
        hit("SUB") -> (in1sin2),
        hit("XOR") -> (in1xorin2),
        hit("AND") -> (in1andin2),
        hit("OR")  -> (in1orin2),
        hit("ULSW")-> (U_SEXT64(io.in1(31, 0)<<(io.in2(4, 0)), 32)),
        hit("URSW")-> (U_SEXT64(io.in1(31, 0)>>(io.in2(4, 0)), 32)),
        hit("SRSW")-> (U_SEXT64((io.in1(31, 0).asSInt>>(io.in2(4, 0))).asUInt, 32)),
        hit("SRS") -> (io.in1.asSInt >> io.in2(5, 0)).asUInt,
        hit("ULS") -> (io.in1 << io.in2(5, 0)),
        hit("URS") -> (io.in1 >> io.in2(5, 0)),
        hit("MULW")-> (U_SEXT64(in1mlin2_32(31, 0), 32)),
        hit("DIVW")-> (U_SEXT64(in1DIVWin2(31, 0), 32)),
        hit("REMW")-> (U_SEXT64(in1REMWin2(31, 0), 32))
    ))
}

object ALUMain extends App {
    val s = getVerilogString(new ALU)
    println(s)
}