package ScProcessor
package RF
import chisel3._
import chisel3.util._
import rfInfo._
import topInfo._
import CSRInfo._
import utils.tools._
import utils.bitUtils._


object CSRInfo {
    val CSR_WIDTH = topInfo.XLEN
    val CSR_ADDR = Map (
        "mepc"      -> 0x341,
        "mcause"    -> 0x342,
        "mtvec"     -> 0x305,
        "mstatus"   -> 0x300
    )
    val CSR_NUM = CSR_ADDR.size
    val CSR_ADDR_WIDTH = 12 // inst(31, 20)
    val OPS_NUM = IDU.IDUInsInfo.csrOps.length
    val DBG_CSR_W = if (NPC_SIM) CSR_NUM*CSR_WIDTH else 0
}

class mstatusBundle extends Bundle{
    val sd      = UInt(1.W)
    val pad5    = UInt(25.W)
    val mbe     = UInt(1.W)
    val sbe     = UInt(1.W)
    val sxl     = UInt(2.W)
    val uxl     = UInt(2.W)
    val pad4    = UInt(9.W)
    val tsr     = UInt(1.W)
    val tw      = UInt(1.W)
    val tvm     = UInt(1.W)
    val mxr     = UInt(1.W)
    val sum     = UInt(1.W)
    val mprv    = UInt(1.W)
    val xs      = UInt(2.W)
    val fs      = UInt(2.W)
    val mpp     = UInt(2.W)
    val pad3    = UInt(2.W)
    val spp     = UInt(1.W)
    val mpie    = UInt(1.W)
    val ube     = UInt(1.W)
    val spie    = UInt(1.W)
    val pad2    = UInt(1.W)
    val mie     = UInt(1.W)
    val pad1    = UInt(1.W)
    val sie     = UInt(1.W)
    val pad0    = UInt(1.W)
}

class CSR() extends Module {
    val io = IO(new Bundle {
        val rdAddr  = Input(UInt(CSR_ADDR_WIDTH.W))
        val rdData  = Output(UInt(CSR_WIDTH.W))
        val wrEn    = Input(Bool())
        val wrAddr  = Input(UInt(CSR_ADDR_WIDTH.W))
        val rsIn    = Input(UInt(REGS_WIDTH.W))
        val immIn   = Input(UInt(5.W))
        val pc      = Input(UInt(XLEN.W))
        val csrOps  = Input(UInt(OPS_NUM.W))
    })
    val dbg_csrs = IO(Output(UInt(DBG_CSR_W.W)))

    val hit = U_HIT_CURRYING(io.csrOps, IDU.IDUInsInfo.csrOps)_

    val mepc = RegInit(0.U(CSR_WIDTH.W))
    val mcause = RegInit(0.U(CSR_WIDTH.W))
    val mtvec = RegInit(0.U(CSR_WIDTH.W))
    val mstatus = RegInit("ha00001800".U(CSR_WIDTH.W))

    dbg_csrs := mepc ## mcause ## mtvec ## mstatus

    val csr_data = Mux1H(Seq(
        csrhit("mepc")      -> mepc,
        csrhit("mcause")    -> mcause,
        csrhit("mtvec")     -> mtvec,
        csrhit("mstatus")   -> mstatus
    ))

    val wrData = Mux1H(Seq(
        hit("csrrw") -> io.rsIn,
        hit("csrrs") -> (csr_data | io.rsIn),
    ))

    def csrhit(s: String) = {
        CSR_ADDR.get(s) match {
            case Some(x) => (x.U === io.rdAddr)
            case None => throw new RuntimeException("No match csr addr")
        }
    }

    def write(s: String, d: UInt) = {
        Mux(csrhit(s) & io.wrEn, wrData, d)
    }

    val mret_status = (mstatus(63, 13) ## 0.U(2.W) ## mstatus(10, 8)
    ## 1.U(1.W) ## mstatus(6, 4) ## mstatus(7) ## mstatus(2, 0))

    mepc    := Mux(hit("ecall"), io.pc, write("mepc", mepc))
    mcause  := Mux(hit("ecall"), "hb".U, write("mcause", mcause))
    mtvec   := write("mtvec", mtvec)
    mstatus := Mux(hit("mret"), mret_status, write("mstatus", mstatus))


    io.rdData := Mux1H(Seq(
        (hit("csrrw") | hit("csrrs"))
             -> csr_data,
        hit("ecall") -> (mtvec & ~("b11".U(CSR_WIDTH.W))),
        hit("mret")  -> mepc,
    ))
}