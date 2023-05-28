package tools
import sys.process._
import scala.language.postfixOps
import scala.io.Source
import java.io._
import kcondition._

class token(str1: String, t_type1: Int) {
    val str = str1
    val t_type = t_type1
}

object expression_calc {
    val token_space = 0
    val token_isEq = token_space + 1
    val token_isExist = token_isEq + 1
    val token_notEq = token_isExist + 1
    val token_and = token_notEq + 1
    val token_or = token_and + 1

    val pattern = Array(
        new token("\\s+", token_space),
        new token("kcIsEq\\(\\s*.*,\\s*\".*\"\\)", token_isEq),
        new token("kcIsExist\\(\\s*.*\\)", token_isExist),
        new token("kcNotEq\\(\\s*.*,\\s*\".*\"\\)", token_notEq),
        new token("&&", token_and),
        new token("\\|\\|", token_or)
    )
    
    // receive expression without #
    def get_tokens(str1: String) = {
        val tokens = new Array[token](30)
        var tokens_idx = 0
        var substr = str1
        while (substr != "") {
            var is_match = false
            for (i <- pattern if !is_match) {
                val temp = i.str.r findFirstMatchIn substr
                if (temp != None) {
                    val res = temp.get.toString
                    val idx = temp.map(_.start).get
                    if (idx==0) {
                        is_match = true
                        substr = substr.substring(res.length())
                        if (res != " ") {
                            tokens(tokens_idx) = new token(res, i.t_type)
                            tokens_idx += 1
                        }
                    }
                }
            }
            if (!is_match) {
                throw new Exception(s"tokens no match at:$substr")
            }
        }
        tokens
    }
}

object precompiler extends App {
    val prj_dir = kcGetStr("CONFIG_PRJ_DIR")
    "rm -rf playground/src/temp/" !;
    "mkdir -p playground/src/temp" !;

    def subdirs2(dir: File): Array[File] = {
        val d: Array[File] = dir.listFiles.filter(_.isDirectory)
        val f: Array[File] = dir.listFiles.filter(_.isFile)
        f ++ d.iterator.flatMap(subdirs2)
    }
    
    val file_list = subdirs2(new File(prj_dir))
    var cnt = 0
    for (i <- file_list) {
        if ((".*\\.scalam".r findFirstIn i.toString) != None) {
            val writer = new PrintWriter(new File("playground/src/temp/temp"+cnt+".scala"))
            cnt += 1
            val reader = Source.fromFile(i, "utf-8").getLines()
            var isTrue = false
            var contain_state = true
            val stack_ = new Array[Boolean](15)
            var stack_cnt = 0
            println(i)
            for (j <- reader) {
                // if (("#kcIsEq(\\s*.*,\\s*\".*\")".r findFirstIn j) != None) {
                //     val lineRegex = "#kcIsEq\\(\\s*(.*),\\s*\"(.*)\"\\)".r
                //     val lineRegex(s1, s2) = j
                //     contain_state = kcIsEq(s1, s2)&&contain_state
                //     stack_(stack_cnt) = contain_state
                // } else if (("#kcIsExist(\\s*.*)".r findFirstIn j) != None) {
                //     val lineRegex = s"#kcIsExist\\(\\s*(.*)\\)".r
                //     val lineRegex(s1) = j
                //     contain_state = kcIsExist(s1)&&contain_state
                //     stack_(stack_cnt) = contain_state
                // } else 
                if (("#endif".r findFirstIn j) != None) {
                    if (stack_cnt != 0) {
                        contain_state = stack_(stack_cnt)
                        stack_cnt -= 1
                    } else {
                        contain_state = true
                    }
                    
                } else if (("#".r findFirstIn j) != None) {
                    if (("\\S+".r findFirstIn j.split("#")(0)) == None) {
                        val reg_str = j.split("#").drop(1).mkString("")
                        val tokens = expression_calc.get_tokens(reg_str)
                        println("\nList:")
                        for (i <- tokens if i!=null) {
                            println(i.str)
                        }
                    }
                } 
                else {
                    if (contain_state == true) {
                        writer.write(j + "\n")
                    }
                }
            }
            writer.close()
        }
    }
}