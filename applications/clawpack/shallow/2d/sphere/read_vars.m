function parms = read_vars()

data = load('setprob.data');

parms.example = data(1);
parms.mapping = data(3);
parms.latitude = data(10:11);
parms.longitude = data(12:13);

end