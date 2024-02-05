package ScProcessor
package IDU
import chisel3._
import chisel3.util._
import utils.tools._

object immGet {
    def immDecI(inst: UInt) = {
        Fill(topInfo.XLEN - 11, inst(31)) ## inst(30, 20) 
    }

    def immDecS(inst: UInt) = {
        Fill(topInfo.XLEN - 11, inst(31)) ## inst(30, 25) ## 
        inst(11, 8) ## inst(7)
    }

    def immDecB(inst: UInt) = {
        Fill(topInfo.XLEN - 12, inst(31)) ## inst(7) ##
        inst(30, 25) ## inst(11, 8) ## 0.U(1.W)
    }

    def immDecU(inst: UInt) = {
        Fill(topInfo.XLEN - 31, inst(31)) ## inst(30, 12) ##
        Fill(12, 0.U(1.W))
    }

    def immDecJ(inst: UInt) = {
        Fill(topInfo.XLEN - 20, inst(31)) ## inst(19, 12) ##
        inst(20) ## inst(30, 25) ##
        inst(24, 21)## 0.U(1.W)
    }

    def immDecCsr(inst: UInt) = {
        inst(19, 15)
    }
}