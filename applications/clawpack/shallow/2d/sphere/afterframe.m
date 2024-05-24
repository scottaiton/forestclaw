setviews;

parms = read_vars();

fprintf('%10s : %12.4e\n','qmin',qmin);
fprintf('%10s : %12.4e\n','qmax',qmax);

if (PlotType  == 4)
    if (parms.example == 0)
        dir_1d = './1d_latitude/_output/';
    elseif (parms.example == 1)
        dir_1d = './1d_ridge/_output/';
    end

    tfile = [dir_1d,'fort.t0000'];
    if exist(tfile,'file') > 0
        [amr1d,t1d] = readamrdata(1,Frame,dir_1d,'ascii','',false);
        if abs(t1d-t) > 1e-8
            error('afterframe : 1d and 2d times do not agree.')
        end

        mq1d = 3;
        [q1d,xc1d,h1d]  = plotframe1ez(amr1d,mq1d,'b-');
        set(h1d,'xdata',pi/180*xc1d);
        fprintf('%10s : %12.4e\n','qmin(1d)',min(q1d));
        fprintf('%10s : %12.4e\n','qmax(1d)',max(q1d));
        set(h1d,'linewidth',1)
        set(h1d,'linewidth',2);

        % X-axis in [-pi/2,pi/2]
        set(h1d,'xdata',xc1d*pi/180);
    end

    xlabel('Latitude','fontsize',16)
    ylabel('h(phi,t)','fontsize',16)

    % axis([-pi/2,pi/2,qmin,qmax])
    set(gca,'xlim',[-pi/2,pi/2])
    set(gca,'fontsize',16)

    [h,labels] = getlegendinfo();
    
    legend(h,labels);
    grid on;
    hold off;
else

    if (PlotParallelPartitions == 0)
        s = (qmax-qmin)/10;
        c1 = parms.hout-s;
        c2 = parms.hout+s;
        c1 = 0.8;
        c2 = 1.2;
%         clim([c1, c2]);
%         clim([0.95,1.05]);
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
%     view(vright)
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