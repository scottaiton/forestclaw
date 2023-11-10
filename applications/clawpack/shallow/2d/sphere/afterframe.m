setviews;


fprintf('%10s : %12.4e\n','qmin',qmin);
fprintf('%10s : %12.4e\n','qmax',qmax);

if (PlotParallelPartitions == 0)
    clim([1,2]);
end
colorbar;


showpatchborders(1:10);

axis off
daspect([1,1,1]);
axis([-1-s,1+s,-1-s,1+s,-1-s,1+s])
view(([84.86,1.15]))

NoQuery = 0;
prt = false;
if (prt)
  filename = framename(Frame,'sphere0000','png');
  print('-dpng',filename);
end

shg