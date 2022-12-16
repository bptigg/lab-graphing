import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats
from scipy import optimize as spo
import numpy as np
import math

errorbars_on = 1
linear_reg_on = 1
deming_reg_on = 1
polynomial_fit_on = 1
exponetial_fit_on = 1


class polynomial:
    def __init__(self,order, variable, coeff):
        self.order = order + 1
        self.variable = variable
        self.coeff = coeff

    def return_value(self, value):
        r_value = 0
        index = 0
        for i in self.coeff:
            index = index + 1
            r_value = r_value + (i*(math.pow(value,self.order-index)))
        return r_value

    def print_polynomial(self):
        items = []
        for i, x in enumerate(reversed(self.coeff)):
            if not x:
                continue
            items.append('{}{}^{}'.format(x if x != 1 else '', self.variable,i))
        items.reverse()
        output = ' + '.join(items)
        output = output.replace('{}^0'.format(self.variable), '')
        output = output.replace('^1 ', ' ')
        output = output.replace('+ -', '- ')

        return output

    def intergrate(self):
        #lower_bound = int(input("Lower bound: "))
        lower_bound = 0
        #upper_bound = int(input("Upper bound: "))
        upper_bound = 600

        values = []
        index = 0
        new_power = self.order
        for i in self.coeff:
            values.append( i / new_power)
            new_power = new_power - 1
        values.append(0)
        integral = polynomial(self.order, 'x', values)
        upper = integral.return_value(upper_bound)
        lower = integral.return_value(lower_bound)

        area = upper - lower
        print(area)

    def find_maxima(self, guess):
        values = []
        current_power = self.order-1
        for i in self.coeff:
            values.append(i * current_power)
            current_power = current_power - 1
        diff = polynomial(self.order-1, 'x', values)
        
        values = []
        current_power = diff.order-1
        for i in diff.coeff:
            values.append(i * current_power)
            current_power = current_power - 1

        diff_2 = polynomial(diff.order - 1, 'x', values)
        root = newton_raphson(diff, diff_2, guess, 1e-10, 0)
        print(root)
        print(diff.return_value(root))
        print(self.return_value(root))
    
def newton_raphson(f, df, x_0, tol ,steps):
    if(steps > 100000):
        print("No solution found")
        return 0
    if abs(f.return_value(x_0)) < tol:
        return x_0
    else:
        return newton_raphson(f, df, x_0 - (f.return_value(x_0) / df.return_value(x_0)), tol, steps + 1)


class graph:
    #x = []
    #y = []
    #x_u = []
    #y_u = []
    #graph_title = ""
    #x_title = ""
    #y_title = ""
    #name = ""

    #figure = 0
    #axis = 0

    def __init__(self, x_data, y_data, X_unc, Y_unc, name, linreg, error_bars, polyfit, expo_fit, demreg):
        self.x = x_data
        self.y = y_data
        self.x_u = X_unc
        self.y_u = Y_unc
        self.graph_title = ""
        self.x_title = ""
        self.y_title = ""
        self.lin = linreg
        self.dem = demreg
        self.polfit = polyfit
        self.e_bars = error_bars
        self.expfit = expo_fit
        self.name = name + ("_e_bar_" if self.e_bars == True else "") + ("_lin_reg_" if self.lin == True else "") + ("_exp_fit_" if self.expfit == True else "") + ("_poly_fit_" if self.polfit == True else "") + ("_Dem_reg_" if self.dem == True else "")
    
    def set_titles(self,x,y,main):
        self.x_title = x
        self.y_title = y
        self.graph_title = main
    
    def demreg(self):
        var_x_u = varience(self.x_u)
        var_y_u = varience(self.y_u)

        delta = var_y_u / var_x_u

        mean_x = 0
        total = 0
        for i in range(0,len(self.x)):
            total = total + self.x[i]

        mean_x = total / len(self.x)

        mean_y = 0
        total = 0
        for i in range(0,len(self.y)):
            total = total + self.y[i]

        mean_y = total / len(self.y)

        s_xx = 0
        for i in range(0, len(self.x)):
            s_xx = s_xx + (self.x[i] - mean_x)**2
        
        s_yy = 0
        for i in range(0, len(self.y)):
            s_yy = s_yy + (self.y[i] - mean_y)**2

        s_xy = 0
        for i in range(0, len(self.y)):
            s_xy = s_xy + (self.x[i] - mean_x) * (self.y[i] - mean_y)

        gradient = (s_yy - (delta * s_xx) + ((s_yy - (delta * s_xx))**2 + 4 * delta * s_xy**2)**(1/2))/(2*s_xy)
        intercept = mean_y - mean_x * gradient
        
        print("{} : y = {}x + {}".format(self.name,gradient,intercept))
        line = lambda m,c,x: m*x + c
        predicted_y = []
        for i in self.x:
            predicted_y.append(line(gradient, intercept, i))
        self.axis.plot(self.x, predicted_y)


    def linreg(self):
        m,c,r,p,s = stats.linregress(self.x,self.y)
        print("{} : y = {}x + {}".format(self.name,m,c))
        line = lambda m,c,x : m*x + c
        predicted_y = []
        for i in self.x:
            predicted_y.append(line(m,c,i))
        self.axis.plot(self.x, predicted_y)

    def polynomial_fit(self):

        order = int(input("Polynomial order: "))
        coeff = np.polyfit(self.x, self.y, order)
        line = polynomial(order, 'x', coeff)
        print("{} : y = ".format(self.name) + line.print_polynomial())
        predicted_y = []
        sorted_x = self.x
        sorted_x.sort()
        x = np.arange(sorted_x[0], sorted_x[len(sorted_x)-1],0.0001)
        for i in x:
            predicted_y.append(line.return_value(i))
        
        #for i in self.x:
            #predicted_y.append(line.return_value(i))
        #self.axis.plot(self.x, predicted_y)
        self.axis.plot(x, predicted_y)
        line.find_maxima(3.2)

        #integrate = int(input("Integrate: (1)yes (0)no: "))
        integrate = 1
        if(integrate == 1):
            line.intergrate()
    
    def expo_fit(self):
        line = lambda x, a, b, c: a * np.exp(b * x) + c
        parameter, covarince = spo.curve_fit(line, self.x, self.y)
        a,b,c = parameter
        parameter, covarience = spo.curve_fit(line, self.x, self.y , p0=(a,b,c))
        a_2, b_2, c_2 = parameter
        print("{} : y = {}exp({}x) + {}".format(self.name, a_2, b_2, c_2))
        predicted_y = []
        for i in self.x:
            predicted_y.append(line(i,a_2, b_2, c_2))
        self.axis.plot(self.x, predicted_y)

    def plot_graph(self):
        self.figure = plt.figure(figsize=(12,9), dpi = 80)
        self.axis = self.figure.add_subplot(111)
        self.axis.scatter(self.x,self.y, color = 'black')
        self.axis.set_xlabel(self.x_title)
        self.axis.set_ylabel(self.y_title)
        self.axis.set_title(self.graph_title)

        if self.e_bars == True:
            self.axis.errorbar(self.x,self.y,xerr = self.x_u,yerr = self.y_u, barsabove = True, fmt = 'o', capsize = 2, ecolor = 'g', color = 'black')
        if self.lin == True:
            self.linreg()
        if self.polfit == True:
            self.polynomial_fit()
        if self.expfit == True:
            self.expo_fit()
        if self.dem == True:
            self.demreg()
        savefile = "Figures/" + self.name + ".png"
        self.figure.savefig(savefile, format = "png")
        
def varience(data):
    mean = 0
    total = 0
    for i in range(0,len(data)):
        total = total + data[i]

    mean = total / len(data)

    varience = 0
    for i in range(0,len(data)):
        temp = (data[i] - mean)**2
        varience = varience + temp
    
    varience = varience / (len(data)-1)
    return varience
    


#sheets = ['single_slit_0.02mm', 'single_slit_0.08mm', 'double_slit_a=0.08mm_d=0.025mm', 'double_slit_a=0.04mm,d=0.05mm', 'Single slit through 5 slit', 'quintuple_slit_a=0.04mm_d=0.125']
#sheets = ['Experiment 1', 'Experiment 2-1', 'Experiment 2-2', 'Experiment 3-1', 'Experiment 3-2', 'Experiment 3-3']
#sheets = ['Experiment 1']#
sheets = ['Efficency']
#sheets = ['Temp', 'Pd']
#sheets = ['Experiment 2-1', 'Experiment 2-2']
#sheets = ['Experiment 3-1', 'Experiment 3-2', 'Experiment 3-3', "Experiment 4-1", "Experiment 4-9", "Experiment 4-13", "Experiment 4-15", "Experiment 4-17", "Experiment 4-20"]
file_name = "Experiment_1"
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

    #errorbars_on = int(input("Error bars: "))
    #linear_reg_on = int(input("Linear regression: "))
    errorbars_on = 0
    linear_reg_on = 0
    exponetial_fit_on = 0
    deming_reg_on = 0
    #polynomial_fit_on = 0
    #polynomial_fit_on = int(input("Polynomial fit: "))
    #exponetial_fit_on = int(input("Exponetial fit: "))
    #deming_reg_on = int(input("Deming regression: "))
    name = "Figure " + str(i)
    data.append(graph(var[0],var[1],var[2],var[3],i,linear_reg_on,errorbars_on, polynomial_fit_on, exponetial_fit_on, deming_reg_on))

titles = pd.read_excel(xlsx, "Titles")
columns = ["sheet", "X","Y","Top"]
dat = []
for i in titles.values:
    temp = []
    dat.append(temp)
for i in range(0,len(columns)):
    col = titles[columns[i]]
    for a in range(0,len(titles.values)):
        dat[a].append(col[a])
e = 0
sheet = 0
for e in data:
    for i in range(0,len(dat)):
        if(dat[i][0] == sheets[sheet]):
            e.set_titles(dat[i][1],dat[i][2],dat[i][3])
        i= i+1
    sheet = sheet+1


for i in data:
    plt.rcParams['font.size'] = '16'
    i.plot_graph()
plt.show()



