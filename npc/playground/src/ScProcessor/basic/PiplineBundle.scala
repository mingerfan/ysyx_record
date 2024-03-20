package ScProcessor
package basic

import chisel3._
import chisel3.util._
import ScProcessor.RF.rfInfo

class IF_ID_Bundle extends Bundle {
  val ifu = new IFU.IFUBundleOut()

  def connect(ifu: DecoupledIO[IFU.IFUBundleOut]): DecoupledIO[IF_ID_Bundle] = {
    val if_id = Wire(DecoupledIO(new IF_ID_Bundle))
    if_id.valid := ifu.valid
    ifu.ready := if_id.ready
    if_id.bits.ifu <> ifu
    if_id
  }
}

class ID_EX_Bundle extends Bundle {
  val idu = new IDU.IDUWrapperBundleOut()
  val rf = new Bundle {
    val rdData1 = UInt(rfInfo.REGS_WIDTH.W)
    val rdData2 = UInt(rfInfo.REGS_WIDTH.W)
  }
}
