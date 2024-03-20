package ScProcessor
package basic

import chisel3._
import ScProcessor.RF.rfInfo

class IF_ID_Bundle {
  val ifu = new IFU.IFUBundleOut()
}

class ID_EX_Bundle {
  val idu = new IDU.IDUWrapperBundleOut()
  val rf = new Bundle {
    val rdData1 = UInt(rfInfo.REGS_WIDTH.W)
    val rdData2 = UInt(rfInfo.REGS_WIDTH.W)
  }
}
