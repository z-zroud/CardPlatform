

# define case check result
S_OK = 1
S_INFO = 2
S_WARN = 3
S_ERROR = 4

def S_Result(result):
    if S_ERROR:
        return "Error"
    elif S_OK:
        return "OK"
    elif S_WARN:
        return "Warning"
    elif S_INFO:
        return "Information"