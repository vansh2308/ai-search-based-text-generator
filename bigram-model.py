import numpy as np

def display_dict(my_dict:dict):
    print("\n")
    for (key, val) in my_dict.items():
        print(key, " -> ", val);


text = open("./corpus.txt").readlines()

word_frq = {}
bigram_frq  = {}

for line in text:
    line = line.lower().strip().split(" ")
    for word in line:
        word_frq[word] = 1 if word not in word_frq else word_frq[word] +1

    for i in range(1, len(line)):
        bigram = line[i-1] + " " + line[i]
        bigram_frq[bigram] = 1 if bigram not in bigram_frq else bigram_frq[bigram]+1




L = len(word_frq.keys())-2
vocab = list(word_frq.keys())
vocab.remove("<sos>")
vocab.remove("<eos>")
vocab.append("<sos>")
vocab.append("<eos>")




probs = np.array([[0.0 for i in range(L+2)] for _ in range(L+2)])

# probs[i][j] => P(j|i) = P(j,i) / P(i)


def calc_cond_prob(i:int, j:int) -> float:
    bgram = vocab[i]  +  " " + vocab[j] 
    p_ij =  0 if bgram not in bigram_frq else bigram_frq[bgram]
    return round(float(p_ij)/word_frq[vocab[i]], 4)



for i in range(len(vocab)):
    for j in range(len(vocab)):
        if(i!=j):
            probs[i][j] = calc_cond_prob(i, j)




probs[-1] = probs[:, -1]
probs = probs[:, :-2]
np.savetxt("./trans2.txt", probs, fmt="%.4e")

with open('vocab2.txt', 'w') as f:
    for line in vocab[:-2]:
        f.write(f"{line}\n")

