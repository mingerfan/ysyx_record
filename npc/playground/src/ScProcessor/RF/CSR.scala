package ScProcessor
package RF
import chisel3._
import chisel3.util._
import rfInfo._
import topInfo._
import csrInfo._


object csrInfo {
    val CSR_WIDTH = topInfo.XLEN
    val CSR_ADDR = Map (
        "mepc"      -> 0x341,
        "mcause"    -> 0x342,
        "mtvec"     -> 0x305,
        "mstatus"   -> 0x300
    )
    val CSR_NUM = CSR_ADDR.size
}

class CSR() extends Module {
    val io = new Bundle {
        val rdAddr1 = Input(UInt(log2Ceil(CSR_NUM).W))
        val rdData1 = Output(UInt(CSR_WIDTH.W))
        val wrEn = Input(Bool())
        val wrAddr = Input(UInt(log2Ceil(CSR_NUM).W))
        val wrData = Input(UInt(CSR_WIDTH.W))
    }

    def csrhit(s: String) = {
        CSR_ADDR.get(s) match {
            case Some(x) => (x.U === io.rdAddr1)
            case None => throw new RuntimeException("No match csr addr")
        }
    }

    def write(s: String, d: UInt) = {
        Mux(csrhit(s), io.wrData, d)
    }

    val mepc = RegEnable(0.U(CSR_WIDTH.W), io.wrEn)
    val mcause = RegEnable(0.U(CSR_WIDTH.W), io.wrEn)
    val mtvec = RegEnable(0.U(CSR_WIDTH.W), io.wrEn)
    val mstatus = RegEnable("ha00001800".U(CSR_WIDTH.W), io.wrEn)

    mepc    := write("mepc", mepc)
    mcause  := write("mcause", mcause)
    mtvec   := write("mtvec", mtvec)
    mstatus := write("mstatus", mstatus)

    io.rdData1 := Mux1H(Seq(
        csrhit("mepc")      -> mepc,
        csrhit("mcause")    -> mcause,
        csrhit("mtvec")     -> mtvec,
        csrhit("mstatus")   -> mstatus
    ))
}