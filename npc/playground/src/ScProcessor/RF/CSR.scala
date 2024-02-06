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

    def csr_addr(s: String) = {
        CSR_ADDR.get(s) match {
            case Some(value) => value
            case None => throw new RuntimeException("No match csr addr")
        }
    }
}

trait HasCSRConst {
    // to avoid hardware field when mixed with module, use def
    def MODE_M = 0x3.U
    def MODE_H = 0x2.U
    def MODE_S = 0x1.U
    def MODE_U = 0x0.U
}


// mret riscv privileged P21
class MstatusBundle extends Bundle with HasCSRConst {
    // Bundle似乎不能包含0.U之类的UInt实例化的内容
    val sd      = UInt(1.W)
    val pad5    = UInt(25.W)
    val mbe     = UInt(1.W)
    val sbe     = UInt(1.W)
    val sxl     = UInt(2.W)  // S-mode is not support, zero(man P22)
    val uxl     = UInt(2.W)  // U-mode is not support, zero(man P22)
    val pad4    = UInt(9.W)
    val tsr     = UInt(1.W)
    val tw      = UInt(1.W)
    val tvm     = UInt(1.W)
    val mxr     = UInt(1.W)
    val sum     = UInt(1.W)
    val mprv    = UInt(1.W)
    val xs      = UInt(2.W) // read only, no additional user ext, zero(man P25, P26)
    val fs      = UInt(2.W) // F ext and S ext is not support, zero(man P25)
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

class CSR() extends Module with HasCSRConst {
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

    val mstatus_wmask =  ~(ZeroExt((
        GenMask(XLEN - 2, 38) | // WPRI(pad5)
        GenMask(31, 23) |   // WPRI(pad4)
        GenMask(16, 15) |   // XS, read only
        GenMask(10, 9) |    // WPRI(pad3)
        GenMask(4)  | // WPRI(pad2)
        GenMask(2)  | // WPRI(pad1)
        GenMask(0)  // WPRI(pad0)
    ), 64))

    val mstatus_rmask = ~(ZeroExt((
            GenMask(XLEN - 2, 38) |
            GenMask(31, 23) |
            GenMask(10, 9) |
            GenMask(4)  |
            GenMask(2)  |
            GenMask(0)
    ), 64))

    dbg_csrs := mepc ## mcause ## mtvec ## mstatus

    val csr_data = Mux1H(Seq(
        csrhit("mepc")      -> mepc,
        csrhit("mcause")    -> mcause,
        csrhit("mtvec")     -> mtvec,
        csrhit("mstatus")   -> (mstatus & mstatus_rmask),
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

    val mstatus_old = WireInit(mstatus.asTypeOf(new MstatusBundle))
    val mstatus_new = WireInit(mstatus.asTypeOf(new MstatusBundle))

    val mret_status = (mstatus(63, 13) ## 0.U(2.W) ## mstatus(10, 8)
    ## 1.U(1.W) ## mstatus(6, 4) ## mstatus(7) ## mstatus(2, 0))

    mepc    := Mux(hit("ecall"), io.pc, write("mepc", mepc))
    mcause  := Mux(hit("ecall"), "hb".U, write("mcause", mcause))
    mtvec   := write("mtvec", mtvec)
    when(hit("mret")) {
        mstatus_new.mpp     := MODE_U   // 因为没有实现U-mode，所以man P127的某些内容不适用，参考P21(然而这样过不了difftest，所以不用mode_m)
        mstatus_new.mie     := mstatus_old.mpie
        mstatus_new.mpie    := 1.U
        mstatus             := mstatus_new.asUInt
    } .elsewhen(hit("ecall")) {
        mstatus_new.mpp     := MODE_M
        mstatus_new.mpie    := mstatus_old.mie
        mstatus_new.mie     := 0.U
        mstatus             := mstatus_new.asUInt
    } .elsewhen (io.wrEn && !hit("mret") && csrhit("mstatus")) {
        mstatus := (wrData & mstatus_wmask) | mstatus
    }


    io.rdData := Mux1H(Seq(
        (hit("csrrw") | hit("csrrs"))
             -> csr_data,
        hit("ecall") -> (mtvec & ~("b11".U(CSR_WIDTH.W))),
        hit("mret")  -> mepc,
    ))
}