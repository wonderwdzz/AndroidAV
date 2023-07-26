//
// Created by Administrator on 2021/3/13.
//

#include <sys/stat.h>
#include <asm/fcntl.h>
#include <fcntl.h>
#include <android/log.h>
#include "MMKV.h"
int32_t DEFAULT_MMAP_SIZE =getpagesize();
void MMKV::initializeMMKV(const char *path) {
//    创建文件
    g_rootDir = path;
//创建文件夹
// 777
    mkdir(g_rootDir.c_str(), 0777);
}

MMKV *MMKV::defaultMMKV() {
//    映射 数据
//创建MMKV  文件名  -----》 数据
    MMKV *kv = new MMKV(DEFAULT_MMAP_ID);


    return kv;
}

MMKV::MMKV(const char *mmapID) {
//    映射
    m_path = g_rootDir + "/" + mmapID;
//差生映射
    loadFromFile();
}

void MMKV::loadFromFile() {
    m_fd = open(m_path.c_str(), O_RDWR | O_CREAT, S_IRWXU);
//    获取文件 具体的大小     binder  mmap 调用者    驱动 binder
    struct stat st = {0};
    if (fstat(m_fd, &st) != -1) {
//        m_size  非  4k整数倍  可能 1  不可能2
        m_size = st.st_size;

    }
    //        纠正   4k整数倍   不然没办法映射    内存  磁盘  交互数据
//    1k  2k
    if(m_size<DEFAULT_MMAP_SIZE||(m_size % DEFAULT_MMAP_SIZE != 0)) {
//        1     不足4k     不是整数倍      有余数 余数不为0

        int32_t oldSize = m_size;
//新的4k整数倍
        m_size = ((m_size / DEFAULT_MMAP_SIZE) + 1) * DEFAULT_MMAP_SIZE;
        //修改文件大小
        if (ftruncate(m_fd, m_size) != 0) {
            m_size = st.st_size;
        }
        //如果文件大小被增加了，   0
        //如果文件大小被增加了， 让增加这些大小的内容变成空
        zeroFillFile(m_fd, oldSize, m_size - oldSize);

    }

//    m_fd   这个文件时4k整数倍
//结合了
    m_ptr= static_cast<int8_t *>(mmap(0, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd,
                                      0));
//     现在有管  需要 1  不需要2    mmkv   管里面
//先缓存到内存中      sp   hashmap
//m_ptr   ---虚拟地址 -----》  物理内存

//    括号里面
    //文件头4个字节写了数据有效区长度  读取 总厂
    memcpy(&m_actualSize, m_ptr, 4);
    __android_log_print(ANDROID_LOG_VERBOSE, "david", "m_actualSize=%d ", m_actualSize);
//m_actualSize   是实际长度
//数据长度  大于0  m_actualSiz 14  118762    1000个数据

//1000  name + i  i
//key长度    key内容    value 的长度    value的内容
//  1 长度      可以    200
//key    最短   int   4个字节
// 1   4个字节           1个字节
    if (m_actualSize > 0) {
//            总长度  后面读的是什么
        ProtoBuf inputBuffer(m_ptr + 4, m_actualSize);
//        清空hashmap
        m_dic.clear();
        //将文件内容解析为map
        while (!inputBuffer.isAtEnd()) {
//           开始解析   名 能够读 能够写         追加写
            std::string key = inputBuffer.readString();
            __android_log_print(ANDROID_LOG_VERBOSE, "david", "key=%s ", key.c_str());
//            解析已经有数据的文件     空文件
//物理内存的数据  搞到 map中
            if (key.length() > 0) {
//                value的长度  数据
                ProtoBuf *value = inputBuffer.readData();
                //数据有效则保存，否则删除key，因为我们是append的
                //数据有效   key的 唯一性
                if (value && value->length() > 0) {
                    m_dic.emplace(key, value);
                }
            }
        }
    }

//    为了 咱们后续动态扩容  需要保存一份原始数据
    m_output = new ProtoBuf(m_ptr + 4 + m_actualSize,
                            m_size - 4 - m_actualSize);

//    size  =4096
}
//写0
void MMKV::zeroFillFile(int fd, int32_t startPos, int32_t size) {
    if (lseek(fd, startPos, SEEK_SET) < 0) {
        return;
    }


    static const char zeros[4096] = {0};
    while (size >= sizeof(zeros)) {
        if (write(fd, zeros, sizeof(zeros)) < 0) {
            return;
        }
        size -= sizeof(zeros);
    }
    if (size > 0) {
        if (write(fd, zeros, size) < 0) {
            return;
        }
    }

}

void MMKV::putInt(const std::string &key, int32_t value) {
//    值    vlue 长度 需要几个字节   value 内容
    int32_t size = ProtoBuf::computeInt32Size(value);
    ProtoBuf *buf = new ProtoBuf(size);
    buf->writeRawInt(value);
//    暂时 的   代写如 物理内存      hashmap
    m_dic.emplace(key, buf);
//物理内存
    appendDataWithKey(key, buf);

}
void MMKV::appendDataWithKey(std::string key, ProtoBuf *value) {
//    待写入的数据大小   size  value
    int32_t itemSize = ProtoBuf::computeItemSize(key, value);
//     最优  GoogleIO存储 更优秀
    if (itemSize > m_output->spaceLeft()) {
//        计算我需要总大小  m_dic  hashmap  去重之后的
        int32_t needSize = ProtoBuf::computeMapSize(m_dic);
//        实际数据   内存中
        needSize += 4;

//        扩容的大小

        //计算每个item的平均长度    计算  咱们  该扩容
        int32_t avgItemSize = needSize / std::max<int32_t>(1, m_dic.size());
//        1    2     3   4       1  在  重新全量更新
        int32_t futureUsage = avgItemSize * std::max<int32_t>(8, (m_dic.size() + 1) / 2);
//   needSize +为了大于
        if (needSize + futureUsage >= m_size) {
            //为了防止将来使用大小不够导致频繁重写，扩充一倍
            int32_t oldSize = m_size;
            do {
                //扩充一倍
                m_size *= 2;

            } while (needSize + futureUsage >= m_size); //如果在需要的与将来可能增加的加起来比扩容后还要大，继续扩容
            //重新设定文件大小
            ftruncate(m_fd, m_size);
            //清空文件
            zeroFillFile(m_fd, oldSize, m_size-oldSize);
            //解除映射
            munmap(m_ptr, oldSize);
            //重新映射
            m_ptr = (int8_t *) mmap(m_ptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
        }

//        扩容之后

//         全量写入
        // 写入数据大小 总长度
        m_actualSize = needSize - 4;
        memcpy(m_ptr, &m_actualSize, 4);
//数据 要遍历
        __android_log_print(ANDROID_LOG_VERBOSE,"david","extending  full write");
        delete m_output;
        //创建输出  全局缓存
        m_output = new ProtoBuf(m_ptr + 4,
                                m_size - 4);
//        m_dic
        auto iter = m_dic.begin();
        for (; iter != m_dic.end(); iter++) {
//
            auto k = iter->first;
//
            auto v = iter->second;
//            物理 内存  1    不能理解2
            m_output->writeString(k);
            m_output->writeData(v);
        }
    } else{
        /**
       *  足够，直接append加入
       */
        //写入4个字节总长度
        m_actualSize += itemSize;
        memcpy(m_ptr, &m_actualSize, 4);

        //写入key
        m_output->writeString(key);
        //写入value
        m_output->writeData(value);
    }
}

int32_t MMKV::getInt(std::string key, int32_t defaultValue) {
//    物理内存的内容
    auto itr = m_dic.find(key);
    if (itr != m_dic.end()) {
//        hashmap
        ProtoBuf *buf = itr->second;
        int32_t returnValue = buf->readInt();
        //多次读取，将position还原为0
        buf->restore();
        return returnValue;
    }



    return defaultValue;

}
