package tools
import scala.io.Source
import scala.collection.mutable
import scala.util.{Failure, Success, Try}

object kcondition{
    val kc_path = "./playground/include/config/auto.conf"
    val kc_str_itor = Source.fromFile(kc_path, "utf-8").getLines()
    val annotation_pattern = "\\s*#".r
    val kc_map = new mutable.HashMap[String, Tuple2[String, Int]]()
    val type_bool = 0
    val type_num = type_bool+1
    val type_str = type_num+1

    def kcInit() = {
        for (i <- kc_str_itor) {
            if (i != "" && (annotation_pattern findFirstIn i) == None) {
                if (("=\\s*y\\s*".r findFirstIn i) != None) {
                    kc_map.put(i.split("\\s*=\\s*")(0), ("true", type_bool))
                } else if (("=\\s*0x[abcdef\\d+]\\s*".r findFirstIn i) != None) {
                    val split = i.split("\\s*=0x\\s*")
                    kc_map.put(split(0), ("h"+split(1), type_num))
                } else if (("=\\s*\\d+\\s*".r findFirstIn i) != None) {
                    val split = i.split("\\s*=\\s*")
                    kc_map.put(split(0), ("h"+ Integer.toString(split(1).toInt, 16), type_num))
                } else if (("=\\s*\".*\"\\s*".r findFirstIn i) != None) {
                    val split = i.split("\\s*=\\s*")
                    kc_map.put(split(0), (split(1).replaceAll("\"", ""), type_str))
                } else {
                    throw new Exception(s"kcInit no match at $i")
                }
            }
        }
    }

    def kcIsEq(macro_key: String, value: String): Boolean = {
        val map_turple = kc_map.get(macro_key)
        if (map_turple == None) {
            return false
        }
        val map_val = map_turple.get._1
        val map_type = map_turple.get._2
        if (map_type == type_bool) {
            return true
        } else if (map_val == value) {
            return true
        } else if (map_type == type_num && ("[db]\\d+".r findFirstIn value)!=None) {
            // to handle number string
            if (value(0) == 'd') {
                Try(Integer.parseInt(value.split("d")(1), 10)) match {
                    case Success(tmp) => return ("h"+Integer.toString(tmp, 16))==map_val
                    case Failure(exception) => None
                    case _ => None
                }
            } else if (value(0) == 'b') {
                Try(Integer.parseInt(value.split("b")(1), 2)) match {
                    case Success(tmp) => return ("h"+Integer.toString(tmp, 16))==map_val
                    case Failure(exception) => None
                    case _ => None
                }
            }
        }
        false
    }

    def kcIsExist(macro_key: String): Boolean = {
        val map_turple = kc_map.get(macro_key)
        if (map_turple == None) {
            return false
        }
        true
    }

    def kcGetNum(macro_key: String): String = {
        val map_turple = kc_map.get(macro_key)
        if (map_turple == None) {
            throw new Exception(s"$macro_key doesn't exist")
        }
        val map_val = map_turple.get._1
        val map_type = map_turple.get._2
        if (map_type != type_num) {
            throw new Exception(s"$macro_key is not a num macro")
        }
        return map_val
    } 

    def kcGetStr(macro_key: String): String = {
        val map_turple = kc_map.get(macro_key)
        if (map_turple == None) {
            throw new Exception(s"$macro_key doesn't exist")
        }
        val map_val = map_turple.get._1
        val map_type = map_turple.get._2
        if (map_type != type_str) {
            throw new Exception(s"$macro_key is not a str macro")
        }
        return map_val
    }

    def printMap() = {
        kc_map.foreach {
            s => println(s._1 + " " + s._2)
        }
    }
    kcInit()
    // println(kcGetNum("CONFIG_AUDIO_CTL_MMIO"))
    // println(kcGetNum("CONFIG_TRACE_END"))
    // println(kcGetStr("CONFIG_DIFFTEST_REF_NAME"))
    // println(kcIsExist("CONFIG_ISA_riscv64"))
    // println()
    // println(kcIsEq("CONFIG_ISA_riscv64", "y1"))
    // println(kcIsEq("CONFIG_AUDIO_CTL_MMIO", "ha0000200"))
    // println(kcIsEq("CONFIG_DIFFTEST_REF_NAME", "none"))
    // println(kcIsEq("CONFIG_TRACE_END", "d10000"))
    // printMap()
}