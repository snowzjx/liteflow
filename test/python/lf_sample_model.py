import numpy as np

def quan_0_comp (input):
    output = []
    for i in input:
        output.append(i * 255 / 62 - 128)
    #return np.array(output, dtype=np.int8)
    return np.array(output)

def fc_1_comp (input):
    weights = [[-2,-87,117,-74,50,-90,49,-64],
              [33,71,36,-88,27,86,23,24],
              [68,91,-113,-29,-61,85,97,6],
              [7,-15,31,-21,24,-127,34,98],
              [219,254,255,255,58,255,255,255]]
              
    res = np.dot(np.append(input,1),weights)
    #return res.astype(np.int8)
    return res

def fc_2_comp(input):
    weights = [[70],[17],[122],[-50],[-54],[-127],[-62],[-14],[141]]
    res = np.dot(np.append(input,1),weights)
    #return res.astype(np.int8)
    return res

def dequan_3_comp(input):
    output = []
    for i in input:
        output.append((i+128) * 30 / 255)
    #return np.array(output,dtype=np.int8)
    return np.array(output)

def main():
    input = [10,20,30,40]
    output = quan_0_comp(input)
    print(output)
    output = fc_1_comp(output)
    print(output)
    output = fc_2_comp(output)
    print(output)
    output = dequan_3_comp(output)
    print(output)

main()