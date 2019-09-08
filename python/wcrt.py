import matplotlib as mpl 
import matplotlib.pyplot as plt 

mpl.use('agg')

list = [] 
taskCount = 20;
startU = 30;
stepU = 5;

for i in range (0,11):
    if i == 9:
        continue
        
    for k in range (0,2): 
        curList = []        
        summe = 0;
        count = 0;
        fNameA = ""
        
        for n in range (0,100):  
            numbers = []  
            if k == 0:
                f = open("rtLFPT"+ str(startU+i*stepU) + str(taskCount) + "-" + str(n) + ".txt", "r")
                fNameA = "avgFPT.txt"
            else:
                f = open("rtLFPM"+ str(startU+i*stepU) + str(taskCount) + "-" + str(n) + ".txt", "r")
                fNameA = "avgFPM.txt"
                
            for value in f:
                number = value.replace('\n', "")
                numbers.append(int(number))
            f.close()  

            numbers.sort(reverse = True)
            curList.append(numbers[0])
            count += 1
            summe += numbers[0]
            
        list.append(curList) 
        f = open(fNameA, "a+")
        avg = summe / count
        f.write(str(avg) + "\n")
        f.close()
        
data_to_plot = list

fig, ax1 = plt.subplots(figsize=(20, 10))
fig.canvas.set_window_title('A Boxplot Example')
fig.suptitle('20 tasks', fontsize=27, fontweight='bold')

bp = ax1.boxplot(data_to_plot)
ax1.yaxis.grid(True, linestyle='-', which='major', color='lightgrey',
               alpha=0.5)

ax1.set_axisbelow(True)
ax1.set_ylabel('WCRT in ns', fontsize=25)
xtickNames = plt.setp(ax1, xticklabels=["FPT30", "FPM30", "FPT35", "FPM35", "FPT40", "FPM40", "FPT45", "FPM45", "FPT50", "FPM50", "FPT55", "FPM55", "FPT60", "FPM60", "FPT65", "FPM65", "FPT70", "FPM70", "FPT80", "FPM80"])
plt.setp(xtickNames)
plt.rc('ytick',labelsize=18)
plt.tick_params(labelsize=18)

fig.savefig('fig1.png', bbox_inches='tight')
#plt.show()