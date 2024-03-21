package ScProcessor
package basic

import chisel3._
import chisel3.util._
import ScProcessor.RF.rfInfo

class RfDataBundle extends Bundle {
  val rdData1 = UInt(rfInfo.REGS_WIDTH.W)
  val rdData2 = UInt(rfInfo.REGS_WIDTH.W)
}

class ID_EX_Bundle extends Bundle {
  val idu = new IDU.IDUWrapperBundleOut
  val rf = new RfDataBundle
  val ifu = new IFU.IFUBundleOut
  val csr = UInt(RF.CSRInfo.CSR_WIDTH.W)
}

class EX_MEM_Bundle extends Bundle {
  val idu = new IDU.IDUWrapperBundleOut
  val rf = new RfDataBundle
  val ifu = new IFU.IFUBundleOut
  val exu = UInt(topInfo.XLEN.W)
  val csr = UInt(RF.CSRInfo.CSR_WIDTH.W)

  def idu_apply(idu: IDU.IDUWrapperBundleOut) = {
    this.idu <> idu
  }

  def rf_apply(rdData1: UInt, rdData2: UInt) = {
    rf.rdData1 := rdData1
    rf.rdData2 := rdData2
  }

  def ifu_apply(ifu: IFU.IFUBundleOut) = {
    this.ifu <> ifu
  }

  def exu_apply(exu: UInt) = {
    this.exu := exu
  }

  def csr_apply(csr: UInt) = {
    this.csr := csr
  }
}

class MEM_WB_Bundle extends EX_MEM_Bundle {
  val mem = UInt(topInfo.XLEN.W)

  def mem_apply(mem: UInt) = {
    this.mem := mem
  }

  def ex_mem_apply(ex_mem: EX_MEM_Bundle) = {
    idu := ex_mem.idu
    rf := ex_mem.rf
    ifu := ex_mem.ifu
    exu := ex_mem.exu
    csr := ex_mem.csr
  }
}
