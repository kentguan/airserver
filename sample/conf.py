import ConfigParser

def getConf():
    ip = "10.1.1.182"
    port = 4408
    work_num = 1
    so_name = "test.so"

    print ip, port, work_num, so_name
    return (ip,port,work_num,so_name)

getConf()
