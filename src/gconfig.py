
gconfig = {
    "workdir": "",
    "download_threads": 8
}

def param(param, default):
    if param not in gconfig:
        return default
    else:
        return gconfig[param]
