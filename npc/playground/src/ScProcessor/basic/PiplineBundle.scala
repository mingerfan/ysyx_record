package ScProcessor
package basic

import chisel3._
import chisel3.util._
import ScProcessor.RF.rfInfo


class ID_EX_Bundle extends Bundle {
  val idu = new IDU.IDUWrapperBundleOut()
  val rf = new Bundle {
    val rdData1 = UInt(rfInfo.REGS_WIDTH.W)
    val rdData2 = UInt(rfInfo.REGS_WIDTH.W)
  }
  val ifu = new IFU.IFUBundleOut
}

class EX_MEM_Bundle extends Bundle {
  
}
