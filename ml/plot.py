#!/usr/bin/env python3.8

import pandas as pd
import plotly.graph_objects as go

A = pd.read_csv('in.csv')
B = pd.read_csv('out.csv')

f = go.Figure()
f.add_trace(go.Scatter(x=A['x'], y=A['y'], mode='markers', name='Training Set'))
f.add_trace(go.Scatter(x=B['x'], y=B['y^'], mode='lines', name='Predictions'))
f.show()
