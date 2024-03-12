package ScProcessor.IFU
import chisel3._
import chisel3.util._
import ScProcessor.topInfo._

class IFUBundleOut extends Bundle {
    val pc = Output(UInt(XLEN.W))
    val pc_next = Output(UInt(XLEN.W))
    val inst = Output(UInt(INS_LEN.W))
}

class IFUBundleIn extends Bundle {
    
}

class IFU extends Module {

}
