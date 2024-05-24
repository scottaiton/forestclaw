

import os, sys
from imp import reload


from clawpack.visclaw import geoplot

import numpy

xlimits = [-90,90]

def setplot(plotdata=None):

    if plotdata is None:
        from clawpack.visclaw.data import ClawPlotData
        plotdata = ClawPlotData()


    plotdata.clearfigures()

    outdir1 = plotdata.outdir


    def fixticks1(current_data):
        from pylab import ticklabel_format, grid,tight_layout
        ticklabel_format(useOffset=False)
        grid(True)
        tight_layout()

    def fixticks(current_data):
        from pylab import ticklabel_format, plot,grid,gca
        ticklabel_format(useOffset=False)
        grid(True)

    plotfigure = plotdata.new_plotfigure(name='domain', figno=0)
    plotfigure.kwargs = {'figsize':(8,6)}
    plotaxes = plotfigure.new_plotaxes()
    plotaxes.axescmd = 'subplot(211)'
    plotaxes.xlimits = xlimits
    plotaxes.ylimits = [-6,8]
    plotaxes.title = 'Surface displacement'
    plotaxes.afteraxes = fixticks

    plotitem = plotaxes.new_plotitem(plot_type='1d_plot')
    plotitem.plot_var = geoplot.surface
    plotitem.color = 'b'

    plotitem = plotaxes.new_plotitem(plot_type='1d_plot')
    plotitem.show = False
    plotitem.plot_var = geoplot.topo
    plotitem.color = 'k'

    plotaxes = plotfigure.new_plotaxes()
    plotaxes.axescmd = 'subplot(212)'
    plotaxes.xlimits = xlimits
    plotaxes.ylimits = [-600,600]
    plotaxes.title = 'momentum'
    plotaxes.afteraxes = fixticks1

    plotitem = plotaxes.new_plotitem(plot_type='1d_fill_between')
    plotitem.show = False
    plotitem.plot_var = geoplot.surface
    plotitem.plot_var2 = geoplot.topo
    plotitem.color = 'b'

    plotitem = plotaxes.new_plotitem(plot_type='1d_plot')
    plotitem.show = False
    plotitem.plot_var = geoplot.topo
    plotitem.color = 'k'

    plotitem = plotaxes.new_plotitem(plot_type='1d_plot')
    plotitem.plot_var = 1
    plotitem.color = 'k'

    plotfigure = plotdata.new_plotfigure(name='surface', figno=1)
    plotfigure.kwargs = {'figsize':(8,3), 'facecolor':'w'}
    plotaxes = plotfigure.new_plotaxes()
    plotaxes.xlimits = xlimits
    plotaxes.ylimits = [-3,5]
    plotaxes.title = 'Surface displacement'

    def fixticks(current_data):
        from pylab import ticklabel_format, grid,title,xlabel,ylabel
        ticklabel_format(useOffset=False)
        grid(linewidth=0.7)
        thours = current_data.t / 3600
        title('Surface at t = %.1f hours' % thours, fontsize=15)
        

    plotaxes.afteraxes = fixticks

    plotitem = plotaxes.new_plotitem(plot_type='1d_plot')
    plotitem.plot_var = geoplot.surface
    plotitem.color = 'b'
    plotitem.kwargs = {'linewidth':2}


    #-----------------------------------------
    # Figures for gauges
    #-----------------------------------------
    plotfigure = plotdata.new_plotfigure(name='q', figno=300, \
                                         type='each_gauge')
    plotfigure.clf_each_gauge = True

    plotaxes = plotfigure.new_plotaxes()
    plotaxes.xlimits = 'auto'
    plotaxes.ylimits = [-2,2]
    plotaxes.title = 'Surface elevation eta'
    plotitem = plotaxes.new_plotitem(plot_type='1d_plot')
    plotitem.plot_var = 2  # eta
    plotitem.plotstyle = 'b-'


    plotdata.printfigs = True          # Whether to output figures
    plotdata.print_format = 'png'      # What type of output format
    plotdata.print_framenos = 'all'      # Which frames to output
    plotdata.print_fignos = 'all'      # Which figures to print
    plotdata.html = True               # Whether to create HTML files
    plotdata.latex = False             # Whether to make LaTeX output
    plotdata.parallel = True

    return plotdata

