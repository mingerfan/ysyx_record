package ScProcessor
package IDU
import chisel3._
import chisel3.util._
import scala.collection.immutable
import scala.collection.mutable
import IDUInfo._
import IDUInsInfo._
import tools._
import basic._
import myutil._
import immGet._

object IDUInfo {
    val OP_BITS = (6, 0)
    val FUNCT3_BITS = (14, 12)
    val FUNCT7_BITS = (31, 25)
    // val INS_LEN = topInfo.INS_LEN
    // val XLEN = topInfo.XLEN
}

class DecOutIO extends Bundle {
    val imm = Output(UInt(topInfo.XLEN.W))
    val rd = Output(UInt(log2Ceil(topInfo.R_NUM).W))     // register destination index
    val rs1 = Output(UInt(log2Ceil(topInfo.R_NUM).W))    // register source 1 index
    val rs2 = Output(UInt(log2Ceil(topInfo.R_NUM).W))    // register source 2 index
}

class InsStruct(op_p: String, funct3_p: String, funct7_p: String, sp_p: String = "-1") {
    val op = Integer.parseInt(op_p, 2)
    val funct3 = Integer.parseInt(funct3_p, 2)
    val funct7 = Integer.parseInt(funct7_p, 2)
    val sp = Integer.parseInt(sp_p, 2)
    require(op > 0)
    require(funct3 >= -1)
    require(funct7 >= -1)
    require(sp >= -1)
}


class IDU extends Module {
    val inst = IO(Input(UInt(topInfo.INS_LEN.W)))
    val dataOut = IO(Output(new DecOutIO()))
    // dp aka data path
    val dpCtrl = IO(new Bundle {
        val aluOp = Output(UInt(EXU.ALUInfo.OPS_NUM.W)) // one-hot encoding
        val exuOp = Output(UInt(EXU.EXUInfo.OPS_NUM.W)) // one-hot encoding
        val ctrls_out = Output(UInt(ctrls.length.W))
        val pcOp = Output(UInt(PC.PCInfo.OPS_NUM.W))
        val rfOp = Output(UInt(RF.RFMInfo.OPS_NUM.W))
        val memOp = Output(UInt(MEMWR.MEMWRInfo.OPS_NUM.W))
    })
    val ebreak = IO(Output(Bool()))
    val inv_inst = IO(Output(Bool()))

    def WireLogic(l_arr: Array[String], 
    l_map: Map[String, Array[String]], b_map: mutable.Map[String, Bool]) = {
        val ports_num = l_arr.length
        val vec_logic = Wire(Vec(ports_num, Bool()))
        for (i <- 0 until ports_num) {
            val strs_opt = l_map.get(l_arr(i))
            require(strs_opt != None)
            val strs = strs_opt.get
            require(strs.length != 0)
            var start = false.B
            for (j <- strs) {
                start = start | b_map(j)
            }
            vec_logic(i) := start
        }
        vec_logic.asUInt
    }

    val dec_opl = Module(new BasicDecoder(4))
    val dec_oph = Module(new BasicDecoder(3))
    val dec_3 = Module(new BasicDecoder(3))
    val dec_7l = Module(new BasicDecoder(4))
    val dec_7h = Module(new BasicDecoder(3))
    
    dec_opl.sel     := inst(3, 0)
    dec_oph.sel     := inst(6, 4)
    dec_3.sel       := inst(14, 12)
    dec_7l.sel      := inst(28, 25)
    dec_7h.sel      := inst(31, 29)


    val insts = mutable.Map[String, Bool]()
    for ((key, value) <- instructions) {
        val ismatch = Wire(Bool())
        ismatch := dec_opl.out(BITS(value.op, 3, 0)) & dec_oph.out(BITS(value.op, 6, 4)) &
        (if (value.funct3 < 0) true.B else dec_3.out(value.funct3)) &
        (if (value.funct7 < 0) true.B else (dec_7l.out(BITS(value.funct7, 3, 0)) & dec_7h.out(BITS(value.funct7, 6, 4)))) &
        (if (value.sp < 0) true.B else inst(31, 26) === value.sp.U)
        insts += (key -> ismatch)
    }
    
    val instBools = VecInit(insts.values.toSeq)
    val inst_hit = instBools.reduceTree(_ | _)

    ebreak := WireDefault(inst === "b000000000001_00000_000_00000_1110011".U)
    inv_inst := !inst_hit && !WireDefault(inst === "b000000000001_00000_000_00000_1110011".U)
    // val ebreak_detect_ = Module(new ebreak_detect)
    // ebreak_detect_.io.ebreak := ebreak

    val immLogic = WireLogic(immSwitch, immSwitchMap, insts)
    val hit = U_HIT_CURRYING(immLogic, immSwitch)_
    dataOut.imm := Mux1H(Seq(
        hit("immI") -> immDecI(inst),
        hit("immU") -> immDecU(inst),
        hit("immJ") -> immDecJ(inst)
        // hit("immB") -> immDecB(inst),
        // hit("immS") -> immDecS(inst)
    ))
    dataOut.rd := inst(11, 7)
    dataOut.rs1 := inst(19, 15)
    dataOut.rs2 := inst(24, 20)


    dpCtrl.aluOp := WireLogic(aluOps, aluOpsMap, insts)
    dpCtrl.exuOp := WireLogic(exuOps, exuOpsMap, insts)
    dpCtrl.ctrls_out := WireLogic(ctrls, ctrlsMap, insts)
    dpCtrl.pcOp  := WireLogic(pcOps, pcOpsMap, insts)
    dpCtrl.rfOp  := WireLogic(rfOps, rfOpsMap, insts)
    dpCtrl.memOp := WireLogic(memOps, memOpsMap, insts)
}

object IDUMain extends App {
    val s = getVerilogString(new IDU)
    println(s)
}