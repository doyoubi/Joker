src = [83,445,438,431,107,107,111,110,435,430,437,215,221,432,434,436,108,111,108,107]
select_point = [2, 9, 14]
out = list()
moveToTime = 900

accumulate = 0
for i in range(0, len(src)):
    accumulate = accumulate + src[i]
    if i in select_point:
        out.append(accumulate)
        accumulate = 0

out[0] = out[0] - moveToTime

print out