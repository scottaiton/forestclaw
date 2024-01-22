setviews;

parms = read_vars();

fprintf('%10s : %12.4e\n','qmin',qmin);
fprintf('%10s : %12.4e\n','qmax',qmax);

fprintf('%10s : %16.8f\n','|qmin-hout|',abs(qmin-parms.hout));
fprintf('%10s : %16.8f\n','|qmax-hout|',abs(qmax-parms.hout));



if (PlotType  == 4)
    if (parms.initial_condition == 2 && ...
            exist('./1d_latitude/_reference/fort.t0000','file') > 0)
        % Run code in 1d_latitude to get reference solution 
        [amr1d,t1d] = readamrdata(1,Frame,'./1d_latitude/_output','ascii','',false);
        if abs(t1d-t) > 1e-8
            error('afterframe : 1d and 2d times do not agree.')
        end
        % (-90,180) and (90,0) --> 180/(-180)*(x - 90) + 0 = 90-x
        amr1d.xlow =  0;  % (90-amr1d.xlow)*pi/180;  
        amr1d.dx = amr1d.dx*pi/180;   
        fprintf('%10s : %12.4e\n','qmin(1d)',min(amr1d.data(1,:)));
        fprintf('%10s : %12.4e\n','qmax(1d)',max(amr1d.data(1,:)));
    
        [q1d,xc1d,h1d]  = plotframe1ez(amr1d,1,'b-','sphere_1d','');
        set(h1d,'linewidth',2)
    end

    hold on;
    plot([0,pi],parms.hout + [1,1]*parms.refine_threshold,'b--');
    plot([0,pi],parms.hout + [-1,-1]*parms.refine_threshold,'b--');
    plot([0,pi],parms.hout + [1,1]*parms.coarsen_threshold,'r--');
    plot([0,pi],parms.hout + [-1,-1]*parms.coarsen_threshold,'r--');
    plot([0,pi],[1,1]*parms.hout,'k-');    
    hold off;
    

    qmin = min([qmin,min(q1d)]);
    qmax = max([qmax,max(q1d)]);

    xlabel('Latitude','fontsize',16)
    ylabel('h(phi,t)','fontsize',16)

    axis([0,pi,qmin,qmax])
    set(gca,'fontsize',16)

    ylim([0.5,2.1])
    [h,labels] = getlegendinfo();
    
    legend(h,labels);
else

    if (PlotParallelPartitions == 0)
        s = (qmax-qmin)/10;
        c1 = parms.hout-s;
        c2 = parms.hout+s;
        c1 = 0.8;
        c2 = 1.2;
        clim([c1, c2]);
    end
    colorbar;
    
    lv = linspace(c1,c2,11);
    % drawcontourlines(lv)
    showpatchborders(1:10);
    setpatchborderprops('linewidth',1)
    
    daspect([1,1,1]);
    s = 0.2;
    axis([-1-s,1+s,-1-s,1+s,-1-s,1+s])
    view([32,24])
    set(gca,'clipping','off')
end

NoQuery = 0;
prt = false;
MaxFrames = 81;
if (prt)
  filename = framename(Frame,'sphere0000','png');
  print('-dpng',filename);
end

shg