class Cat:
    def __init__(self,name):
        self.name = name
def ask_ok(prompt, retire=4,complaint='Yes or no, please!'):
    while True:
        ok = input(prompt)
        if ok in ('y','ye','yes'):
            return True
        if ok in ('n','no','nop','nope'):
            return False
        retire = retire - 1
        if retire < 0:
            return OSError('uncooperative user')
        print(complaint)

jojo = Cat("jojo")
print(jojo.name)
ask_ok('OK  to overwrite the file?',2,'Do you really want to quit?')
