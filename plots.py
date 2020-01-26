import csv
import matplotlib.pyplot as plt

x1 = []
y1 = []
x2 = []
y2 = []
x3 = []
y3 = []
x4 = []
y4 = []

filename1 = 'randomActionSelection.csv'
filename2 = 'qLearning-EGreedy.csv'
filename3 = 'qLearning-OptimisticInitialValues.csv'
filename4 = 'RLearning-EGreedy.csv'
filename5 = 'RLearning-OptimisticInitialValues.csv'
filename6 = 'sarsa-EGreedy.csv'
filename7 = 'sarsa-OptimisticInitialValues.csv'


with open(filename1) as f1:
  reader = csv.reader(f1,quoting=csv.QUOTE_NONNUMERIC)  #line 1
  header_row = next(reader)  #line 2
  for row in reader:
      x1.append(row[0])
      y1.append(row[1])

with open(filename2) as f2:
  reader = csv.reader(f2,quoting=csv.QUOTE_NONNUMERIC)  #line 1
  header_row = next(reader)  #line 2
  for row in reader:
      x2.append(row[0])
      y2.append(row[1])

with open(filename4) as f4:
  reader = csv.reader(f4,quoting=csv.QUOTE_NONNUMERIC)  #line 1
  header_row = next(reader)  #line 2
  for row in reader:
      x3.append(row[0])
      y3.append(row[1])

with open(filename6) as f6:
  reader = csv.reader(f6,quoting=csv.QUOTE_NONNUMERIC)  #line 1
  header_row = next(reader)  #line 2
  for row in reader:
      x4.append(row[0])
      y4.append(row[1])

# # E-greedy low traffic
# plt.title("Traffic light control, \u03B5-greedy ,  P(newCar)=0.2")
# plt.plot(x1, y1, color='red', linewidth=1, label='Random Action Selection')
# plt.plot(x2, y2, color='blue', linewidth=1, label='Q-learning(\u03B5=0.01, \u03B1=0.58, \u03B3=0.08)')
# plt.plot(x3, y3, color='green', linewidth=1, label='R-Learning(\u03B5=0.01, \u03B1=0.002, \u03B2=0.04 )')
# plt.plot(x4, y4, color='black', linewidth=1, label='Sarsa(\u03B5=0.04, \u03B1=0.30, \u03B3=0.25)')

# # E-greedy high traffic
# plt.title("Traffic light control, \u03B5-greedy ,  P(newCar)=0.4")
# plt.plot(x1, y1, color='red', linewidth=1, label='Random Action Selection')
# plt.plot(x2, y2, color='blue', linewidth=1, label='Q-learning(\u03B5=0.01, \u03B1=0.30, \u03B3=0.20)')
# plt.plot(x3, y3, color='green', linewidth=1, label='R-Learning(\u03B5=0.01, \u03B1=0.004, \u03B2=0.07 )')
# plt.plot(x4, y4, color='black', linewidth=1, label='Sarsa(\u03B5=0.12, \u03B1=0.24, \u03B3=0.08)')

# # OIV low traffic
# plt.title("Traffic light control, Optimistic initial values,  P(newCar)=0.2")
# plt.plot(x1, y1, color='red', linewidth=1, label='Random Action Selection')
# plt.plot(x2, y2, color='blue', linewidth=1, label='Q-learning(Q(0)=1.1, \u03B1=0.58, \u03B3=0.08)')
# plt.plot(x3, y3, color='green', linewidth=1, label='R-Learning(Q(0)=1.2, \u03B1=0.002, \u03B2=0.04 )')
# plt.plot(x4, y4, color='black', linewidth=1, label='Sarsa(Q(0)=2.0, \u03B1=0.30, \u03B3=0.25)')

# # OIV high traffic
# plt.title("Traffic light control, Optimistic initial values,  P(newCar)=0.4")
# plt.plot(x1, y1, color='red', linewidth=1, label='Random Action Selection')
# plt.plot(x2, y2, color='blue', linewidth=1, label='Q-learning(Q(0)=0.8, \u03B1=0.30, \u03B3=0.20)')
# plt.plot(x3, y3, color='green', linewidth=1, label='R-Learning(Q(0)=1.0, \u03B1=0.004, \u03B2=0.07 )')
# plt.plot(x4, y4, color='black', linewidth=1, label='Sarsa(Q(0)=2.5, \u03B1=0.24, \u03B3=0.08)')


plt.xlabel('Action Selections')
plt.ylabel('Total Average Waiting Time On Crossroad')
plt.legend()
plt.grid()
plt.show()