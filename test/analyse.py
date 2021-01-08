#!/usr/bin/env python
# coding: utf-8

# In[78]:


import os
import pandas as pd
import numpy as np
import re
from json import load
import matplotlib.pyplot as plt
from matplotlib.markers import MarkerStyle
from pprint import PrettyPrinter
pp = PrettyPrinter(indent=2)

plot_infos = {
    'sudoku':{
        'marker':'o',
        'color':'red'
    },
    'cluster':{
        'marker':'s',
        'color':'red'
    },
    'menage':{
        'marker':'+',
        'color':'blue'
    },
    'cars':{
        'marker':'x',
        'color':'lime'
    },
    'queens':{
        'marker':'*',
        'color':'green'
    },
    'langford':{
        'marker':'o',
        'color':'orange'
    },
    'ex':{
        'marker':'s',
        'color':'black'
    },
    'graceful':{
        'marker':'+',
        'color':'purple'
    }
}

# markers list : https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.markers.MarkerStyle.html#matplotlib.markers.MarkerStyle
# color list : https://matplotlib.org/3.1.0/gallery/color/named_colors.html


# In[79]:


results_dir = 'results'
names = os.listdir(results_dir)
results = []
for name in names:
    filename = results_dir + '/' + name
    with open(filename) as f:
        entries = load(f)['results']
        for e in entries:
            for p in e['parameters']:
                e['p_'+p] = e['parameters'][p]
            del e['parameters']
        results.extend(entries)

df = pd.DataFrame(results)
df['category'] = df['p_file'].apply(lambda x: re.split(r'[_\-.0-9]',x)[0])
knuth = df[df.p_ex=='dlx2']
solnon = df[df.p_ex=='xcc-with-sparse-sets']
print(knuth)
print(solnon)


# In[104]:


plt.figure(figsize=(10,5))
plt.yscale('log')
plt.xscale('log')

#plt.scatter(knuth['mean'],solnon['mean'],c=solnon.color,cmap='tab10',marker='+')
for g in solnon.category.unique():
    s = solnon[solnon['category']==g]
    k = knuth[knuth['category']==g]
    try:
        plot_infos[g]
    except:
        print("No plot information for this category. Please fill the plot_infos dict at the beginning of the code.")
    plt.scatter(k['mean'], s['mean'], c = plot_infos[g]['color'],marker= plot_infos[g]['marker'],label = g,s=75.0)
plt.legend()
plt.xlabel("Knuth's algorithm execution time (s)")
plt.ylabel("Solnon's algorithm execution time (s)")
plt.plot([0,600],[0,600],color='grey') #diag line
plt.plot([0,600],[600,600],color='grey') #top line
plt.plot([0,0],[600,0],color='grey') #left line
plt.plot([0,600],[0,0],color='grey') #bottom line
plt.plot([600,600],[0,600],color='grey') #right line
plt.show()

print("Each point above the diagonal means that Knuth's is better than Solnon's.")
print("Each point under the diagonal means that Solnon's is better than Knuth's.")
print("Each point on the diagonal means that both are equal.")


# In[ ]:




