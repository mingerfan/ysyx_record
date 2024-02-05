package ScProcessor.RF

import chisel3._
import chisel3.util._

abstract trait rwMask {
    def wmask(): UInt
    def rmask(): UInt
}

abstract trait rwFun {
    def read(addr: UInt, rmask: UInt): UInt
    def write(addr: UInt, wmask: UInt, wen: UInt): UInt
}
