function b = bathy(xc,yc)

parms = read_vars();

if parms.example == 0
    b = -ones(size(xc));
elseif parms.example == 1
    Rsphere = 1;
    
    Px = parms.Px;
    Py = parms.Py;
    Pz = parms.Pz;
    theta_ridge = parms.theta_ridge;
    alpha = parms.alpha;
    bc = parms.bathy;
    
    [xp,yp,zp] = mapc2m(xc,yc);
    d = Px*xp + Py*yp + Pz*zp;
    theta = asin(d/Rsphere);
    qb = exp(-alpha*(theta+theta_ridge).^2);
    b = -bc(1) + bc(2)*qb;
end    
