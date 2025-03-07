# python3

import re
import os
import shutil

'''(EXPORT_PARA_GROUP typedef union {
    __packed struct {_grp_paras};
    u16 GROUP[GROUP_SIZE * _grp_cnt];
} _grp_name;)'''

# _grp_paras: 组成员
# _grp_cnt: 组数
# _grp_name: 组名

'''_elem_type _elem_name;  ///< _elem_index _elem_mark'''
# _elem_type: 参数类型
# _elem_name: 参数名
# _elem_index: 参数索引
# _elem_comment: 参数注释


__block = r'''(EXPORT_PARA_GROUP typedef union \{
    __packed struct \{(.*?)\};
    u16 GROUP\[GROUP_SIZE \* \d{1,}\];.*?
\} (\w+_u);)'''

__line = r'''((\w+) \w+(\[(\d{1,})\])?;) {0,}///< ?(P\d{0,}\.\d{0,})? ?(.*?\n)'''


filesrc = "./tbl.h"  # [config]
filedst = filesrc  # [config]


def backup(filesrc):
    # 文件备份
    index = 1
    filedst = ""
    while True:
        filedst = filesrc + "." + str(index) + ".bak"
        if not os.path.exists(filedst):
            break
        index += 1
    shutil.copyfile(filesrc, filedst)


def add_prefix(number, prefix, length):
    # 通过添加前缀的方式将文本补全至指定长度
    result = str(number)
    delta = length - len(result)
    if delta > 0:
        result = prefix * delta + result
    return result


def generate_para_index(group, offset):
    # 生成参数索引
    return " ///< P" + add_prefix(group, "0", 2) + "." + add_prefix(offset, "0", 3) + " "


if os.path.exists(filesrc):

    file_ctx = ""

    if filedst == filesrc:  # backup
        backup(filesrc)

    with open(filesrc, "r", encoding="utf-8") as fsrc:
        file_ctx = ''.join(fsrc.readlines())

    with open(filedst, "w", encoding="utf-8") as fdst:

        # regex
        re_blocks = re.compile(__block, re.S)
        re_lines = re.compile(__line)

        # groups
        _grp_offset = 0
        _grp_bufsize = 100  # [config] 缓冲块大小
        for grp_idx, block in enumerate(re_blocks.findall(file_ctx)):
            _grp_bufcnt = 1  # 缓冲块数量
            _grp_ctx = block[0]
            _grp_paras = block[1]  # 参数
            _grp_name = block[2]  # 组名

            # params
            para_elem_offset = 0

            # 允许的数据类型及所占的字数(word)
            para_elem_type = {  # [config]
                "s16": 1, "s32": 2, "s64": 4,
                "u16": 1, "u32": 2, "u64": 4,
                "f32": 2, "f64": 4,
            }

            grp_paras = [""]

            paras = re_lines.findall(_grp_paras)
            for index, line in enumerate(paras):

                _para_ctx_left = line[0]  # 参数定义(类型+名称)
                _para_ctx_right = line[5]  # 参数注释

                _para_elem_type = line[1]  # 参数类型
                _para_array_size = line[3]  # 参数数组大小

                para_new_line = (_para_ctx_left + generate_para_index(grp_idx + _grp_offset, para_elem_offset) + _para_ctx_right)
                grp_paras.append(para_new_line)
                # print(para_new_line)

                # 检查数据类型, 计算下一个参数的地址索引
                try:
                    para_array_size = 1 if _para_array_size == '' else int(_para_array_size)
                    para_elem_offset += para_elem_type[_para_elem_type] * para_array_size
                except:
                    raise f"error type: {_para_elem_type}"

                # 检查是否超过单组容量
                if (_grp_bufsize <= para_elem_offset) and (index < (len(paras)-1)):
                    para_elem_offset -= _grp_bufsize
                    _grp_offset += 1
                    _grp_bufcnt += 1

            # 输出当前组结构体
            grp_paras_ctx = ""
            grp_paras_ctx += "EXPORT_PARA_GROUP typedef union {\n"
            grp_paras_ctx += "    __packed struct {\n"
            grp_paras_ctx += "        ".join(grp_paras)
            grp_paras_ctx += "    };\n"
            grp_paras_ctx += "    u16 GROUP[GROUP_SIZE * " + str(_grp_bufcnt) + "];" + generate_para_index(grp_idx + _grp_offset, para_elem_offset-1) + "\n"
            grp_paras_ctx += "} " + _grp_name + ";"

            file_ctx = file_ctx.replace(_grp_ctx, grp_paras_ctx)

        fdst.write(file_ctx)


# grp_idx = 0
# grp_elem_offset = 0
# grp_name = "arr_a"


# print(ctx)
