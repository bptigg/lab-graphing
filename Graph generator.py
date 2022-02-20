import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats

errorbars_on = 1
linear_reg_on = 1

class graph:
    x = []
    y = []
    x_u = []
    y_u = []
    graph_title = ""
    x_title = ""
    y_title = ""
    name = ""

    figure = 0
    axis = 0

    def __init__(self, x_data, y_data, X_unc, Y_unc, name, linreg, error_bars):
        self.x = x_data
        self.y = y_data
        self.x_u = X_unc
        self.y_u = Y_unc
        self.graph_title = ""
        self.x_title = ""
        self.y_title = ""
        self.name = name
        self.lin = linreg
        self.e_bars = error_bars
    
    def set_titles(self,x,y,main):
        self.x_title = x
        self.y_title = y
        self.graph_title = main
    
    def linreg(self):
        m,c,r,p,s = stats.linregress(self.x,self.y)
        print("{} : y = {}x + {}".format(self.name,m,c))
        line = lambda m,c,x : m*x + c
        predicted_y = []
        for i in self.x:
            predicted_y.append(line(m,c,i))
        self.axis.plot(self.x, predicted_y)

    def plot_graph(self):
        self.figure = plt.figure(figsize=(12,9), dpi = 80)
        self.axis = self.figure.add_subplot(111)
        self.axis.scatter(self.x,self.y)
        self.axis.set_xlabel(self.x_title)
        self.axis.set_ylabel(self.y_title)
        self.axis.set_title(self.graph_title)
        if self.e_bars == True:
            self.axis.errorbar(self.x,self.y,xerr = self.x_u,yerr = self.y_u, barsabove = True, fmt = 'o', capsize = 2, ecolor = 'g')
        if self.lin == True:
            self.linreg()
        plt.savefig('Figures/' + self.name + '.png')
        
    
sheets = ['rough,refraction','Snells_law_flat','Snells_law_curved']
file_name = "laws_of_refraction"
file_path = "Excel/" + file_name + ".xlsx"
xlsx = pd.ExcelFile(file_path)
data = []
for i in sheets:
    df = pd.read_excel(xlsx, i)

    #print(df)
    columns = ['X','Y','X_u', 'Y_u']
    var = []
    ds = pd.DataFrame(df, columns=['X','Y','X_u', 'Y_u'])
    for a in columns:
        col = ds[a]
        #print(col)
        dat = []
        for e in range(1,len(col)):
            dat.append(col[e])
        var.append(dat)

    errorbars_on = int(input("Error bars: "))
    linear_reg_on = int(input("Linear regression: "))
    data.append(graph(var[0],var[1],var[2],var[3],i,linear_reg_on,errorbars_on))

#titles = pd.read_excel(xlsx, "Titles")
#columns = ["sheet", "X","Y","Top"]
#dat = []
#for i in sheets:
#    temp = []
#    dat.append(temp)
#for i in range(0,len(columns)):
#    col = titles[columns[i]]
#    for a in range(0,len(sheets)):
#        dat[a].append(col[a])
#i = 0
#for e in data:
#    e.set_titles(dat[i][1],dat[i][2],dat[i][3])
#    i= i+1


for i in data:
    i.plot_graph()
plt.show()



