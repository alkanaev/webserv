import random
res = [random.randrange(-500, 500, 1) for i in range(500)]
check = []
for i in res:
    if i not in check:
        check.append(i)
        print(i, end = " ")
