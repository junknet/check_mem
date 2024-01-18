function logd(data) {
    console.log(data);
}



var dbi_path = "/data/local/tmp/libqwe.so";
var qwe = Module.load(dbi_path);
logd(qwe.name)
var func_addr = qwe.findExportByName("find_module")
var func = new NativeFunction(func_addr, "void", ["pointer"])
logd(func)


setTimeout(() => {


    func(Memory.allocUtf8String("https://"))


}, 1000);
